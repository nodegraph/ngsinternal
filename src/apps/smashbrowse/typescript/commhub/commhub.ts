/// <reference path="D:\dev\windows\DefinitelyTyped\node\node.d.ts"/>
/// <reference path="D:\dev\windows\DefinitelyTyped\ws\ws.d.ts"/>
/// <reference path="D:\dev\windows\DefinitelyTyped\request\request.d.ts"/>
/// <reference path="D:\dev\windows\DefinitelyTyped\selenium-webdriver\selenium-webdriver.d.ts"/>
/// <reference path="..\message\message.d.ts"/>

//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

// Note default arguments for function requires ES6.
// This version of nodejs doesn't seem to have ES6 yet.

// We use strict mode. In strict mode we can't use undeclared variables.
//"use strict";

// Our imports.
import ws = require('ws') // Note although similar to WebSocket (es6), ws is the nodejs type that we want to use not WebSocket.
import Http = require('http')
import Https = require('https')
import Request = require('request');
import Path = require('path')

// Our own modules.
import {FSWrap} from './fswrap'
import {WebDriverWrap, Key} from './webdriverwrap'
import {DebugUtils} from './debugutils'

// Secure web socket to communicate with chrome extension.
let ext_server: ChromeSocketServer = null

// Regular WebSocket to communicate with app.
let app_server: AppSocketServer = null

// Make sure we catch any uncaught exceptions.
DebugUtils.catch_uncaught_exceptions()

//------------------------------------------------------------------------------------------------
//Socket server creation.
//------------------------------------------------------------------------------------------------

class BaseConnection {
    socket: ws
    webdriverwrap: WebDriverWrap
    constructor(webdriverwrap: WebDriverWrap) {
        this.webdriverwrap = webdriverwrap
        this.socket = null
    }
    test(): boolean {
        return true
    }
    is_alive(): boolean {
        if (this.socket && this.socket.readyState === 1) {
            return true
        }
        return false
    }
    send_json(json: string): void {
        this.socket.send(json)
    }
    receive_json(json: string): void {
    }
}

class ChromeConnection extends BaseConnection {
    constructor(webdriverwrap: WebDriverWrap) {
        super(webdriverwrap)
    }
    receive_json(json: string): void {
        // Extract the frame from the msg.
        let msg = BaseMessage.create_from_string(json)
        console.log('frame[' + msg.iframe + '] --> commhub: ' + json)

        // Messages from chrome get passed straight to the app.
        send_msg_to_app(msg)
    }
}

class AppConnection extends BaseConnection {
    constructor(webdriverwrap: WebDriverWrap) {
        super(webdriverwrap)
    }
    //The can send us messages either from c++ or from qml.
    //These requests will be handled by webdriverjs, or the extension scripts in the browser.
    //Requests will always return with a response message containing return values.
    receive_json(json: string): void {
        console.log('app --> commhub: ' + json)

        // Build the message.
        let msg = BaseMessage.create_from_string(json)

        // The msg.frame should always be -1 here.
        if (msg.iframe != '-1') {
            console.error('nodejs expects msg.iframe from app to be -1')
        }

        // Make sure the message is a request.
        if (msg.get_msg_type() == MessageType.kResponseMessage || msg.get_msg_type() == MessageType.kInfoMessage) {
            console.error("Error: AppConnection currently always expects a request.")
        }
        let req = <RequestMessage>msg

        // Handle the request.
        switch (req.request) {
            case RequestType.kShutdown: {
                this.webdriverwrap.close_browser().then(function() { process.exit(-1) })
                break;
            }
            case RequestType.kCheckBrowserIsOpen:
                function on_response(open: boolean) {
                    if (!open) {
                        this.webdriverwrap.open_browser()
                        // Now convert this request to navigate to a default url with a port number.
                        let url = "https://www.google.com/?" + get_ext_server_port()
                        let relay = new RequestMessage(msg.id, '-1', RequestType.kNavigateTo, {url: url})
                        this.receive_json(relay.to_string())
                    } else {
                        send_msg_to_app(new ResponseMessage(msg.id, '-1', true))
                    }
                }
                console.log('got check browser is open')
                this.webdriverwrap.browser_is_open(on_response.bind(this))
                break
            case RequestType.kResizeBrowser:
                this.webdriverwrap.resize_browser(req.args.width, req.args.height).then(
                    function(){send_msg_to_app(new ResponseMessage(msg.id, '-1', true))},
                    function(){send_msg_to_app(new ResponseMessage(msg.id, '-1', false))}
                )
                break
            case RequestType.kOpenBrowser:
                if (this.webdriverwrap.open_browser()) {
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', true))
                } else {
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', false))
                }
                break
            case RequestType.kCloseBrowser:
                this.webdriverwrap.close_browser()
                send_msg_to_app(new ResponseMessage(msg.id, '-1', true))
                break
            case RequestType.kNavigateTo:
                this.webdriverwrap.navigate_to(req.args.url).then(function() {
                    let relay_req = new RequestMessage(msg.id, "-1", RequestType.kSwitchIFrame, {iframe: ''})
                    send_msg_to_ext(relay_req)
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', false, error))
                })
                break
            case RequestType.kNavigateBack:
                this.webdriverwrap.navigate_back().then(function() {
                    let relay_req = new RequestMessage(msg.id, "-1",RequestType.kSwitchIFrame, {iframe: ''})
                    send_msg_to_ext(relay_req)
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', false, error))
                })
                break
            case RequestType.kNavigateForward:
                this.webdriverwrap.navigate_forward().then(function() {
                    let relay_req = new RequestMessage(msg.id, "-1",RequestType.kSwitchIFrame, {iframe: ''})
                    send_msg_to_ext(relay_req)
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', false, error))
                })
                break
            case RequestType.kNavigateRefresh:
                this.webdriverwrap.navigate_refresh().then(function() {
                    let relay_req = new RequestMessage(msg.id, "-1",RequestType.kSwitchIFrame, {iframe: ''})
                    send_msg_to_ext(relay_req)
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', false, error))
                })
                break
            case RequestType.kSwitchIFrame:
                this.webdriverwrap.switch_to_iframe(req.args.iframe).then(function() {
                    // Update the chrome extension.
                    send_msg_to_ext(req)
                }, function(error) {
                    console.log('comm hub failed to set iframe to ' + req.args.iframe)
                    send_msg_to_app(new ResponseMessage(msg.id, '-1', false, error))
                })
                break
            case RequestType.kPerformAction:
                //if (req.xpath) {
                    // If the xpath has been resolved by the content script,
                    // then we let webdriver perform actions.
                    switch (req.args.action) {
                        case ActionType.kSendClick: {
                            let p = this.webdriverwrap.click_on_element(req.args.xpath, req.args.overlay_rel_click_pos.x, req.args.overlay_rel_click_pos.y)
                            WebDriverWrap.terminate_chain(p, req.id)
                        } break
                        case ActionType.kMouseOver: {
                            let p = this.webdriverwrap.mouse_over_element(req.args.xpath, req.args.overlay_rel_click_pos.x, req.args.overlay_rel_click_pos.y)
                            WebDriverWrap.terminate_chain(p, req.id)
                        } break
                        case ActionType.kSendText: {
                            let p = this.webdriverwrap.send_text(req.args.xpath, req.args.text)
                            WebDriverWrap.terminate_chain(p, req.id)
                        } break
                        case ActionType.kSendEnter: {
                            let p = this.webdriverwrap.send_key(req.args.xpath, Key.RETURN)
                            WebDriverWrap.terminate_chain(p, req.id)
                        } break
                        case ActionType.kGetText: {
                            let p = this.webdriverwrap.get_text(req.args.xpath)
                            WebDriverWrap.terminate_chain(p, req.id)
                        } break
                        case ActionType.kSelectOption: {
                            let p = this.webdriverwrap.select_option(req.args.xpath, req.args.option_text)
                            WebDriverWrap.terminate_chain(p, req.id)
                        } break
                        case ActionType.kScrollDown:
                        case ActionType.kScrollUp:
                        case ActionType.kScrollRight:
                        case ActionType.kScrollLeft: {
                            // Scroll actions need to be performed by the extension
                            // so we pass it through.
                            send_msg_to_ext(req)
                        } break
                    }
                //} 
                // else {
                //     // Send request to extension to resolve the xpath,
                //     // and to unblock the events in the content script so that
                //     // the webdriver actions can take effect on the elements.
                //     send_msg_to_ext(req)
                // }
                break
            default:
                // By default we send requests to the extension. 
                // (It goes through bg script, and then into content script.)
                send_msg_to_ext(req)
                break
        }
    }
}

class BaseSocketServer {
    // This can easily be upgraded to handle non ssl http as well.
    // let http_server: HttpServer.Server
    // http_server = HttpServer.createServer(dummy_handler)

    static ssl = true
    static ssl_key = './key.pem'
    static ssl_cert = './cert.pem'

    public port: number
    private https_server: Https.Server
    private server: ws.Server
    private connections: BaseConnection[]

    webdriverwrap: WebDriverWrap

    constructor(webdriverwrap: WebDriverWrap) {
        this.webdriverwrap = webdriverwrap
        this.port = 8093
        this.https_server = null
        this.server = null
        this.connections = []
    }

    protected on_https_server_error(error: NodeJS.ErrnoException): void {
        if (error.code === 'EADDRINUSE') {
            // The current config port number is in use.
            // So we increment it and recurse.
            this.port += 1
            this.build()
        } else {
            // Call the callback with failed result.
            this.on_build_failed()
        }
    }

    protected on_https_server_listening<C extends BaseConnection>(ConnConstructor: { new (wdw: WebDriverWrap): C; }): void {
        // Create the socket server.
        // The following options variable should have type ws.IServerOptions but DefinitelyTyped seems to have a bug with internal server type.
        let options: any = { server: this.https_server }
        this.server = new ws.Server(options)
        this.server.on('connection', function(socket: ws) {
            let conn: C = new ConnConstructor(this.webdriverwrap);
            conn.socket = socket
            socket.on('message', conn.receive_json.bind(conn))
            this.connections.push(conn)
        }.bind(this))
        // Now call the callback.
        this.on_built()
    }

    protected start_build<C extends BaseConnection>(ConnConstructor: { new (wdw: WebDriverWrap): C; }) {
        // Dummy handler for http/s requests.
        let dummy_handler = function(req: Http.IncomingMessage, res: Http.ServerResponse) {
            res.writeHead(200);
            res.end("Https server is running.\n")
        }.bind(this);

        // Create the http/s server.
        this.https_server = Https.createServer({
            key: FSWrap.read_from_file(BaseSocketServer.ssl_key),
            cert: FSWrap.read_from_file(BaseSocketServer.ssl_cert)
        }, dummy_handler)

        // Once the https server reports an error, we try to create the https server on a different port.
        this.https_server.once('error', this.on_https_server_error.bind(this));

        // Once the https server is listening, we try to create the WebSocket server.
        this.https_server.once('listening', this.on_https_server_listening.bind(this, ConnConstructor));

        this.https_server.listen(this.port)
    }

    // These should be overridden in derived classes.
    build() { }
    on_built() { }
    on_build_failed() {
        console.error("Error: SocketServer failed to build.")
    }
    clean_invalid_connections() {
        let conns = this.connections
        for (let i = 0; i < conns.length; i++) {
            if (!conns[i].is_alive()) {
                conns.splice(i, 1)
                i--
            }
        }
    }
    send_msg(msg: BaseMessage) {
        // Clean any invalid connections.
        this.clean_invalid_connections()

        // Send the msg through all connections which are alive.
        let sent = false
        let conns = this.connections
        for (let i = 0; i < conns.length; i++) {
            let conn = conns[i]
            if (conn.is_alive()) {
                if (msg.to_string() == '{}') {
                    console.error("error sending empty message")
                }
                conn.send_json(msg.to_string())
                sent = true
            }
        }

        if (!sent) {
            console.error('Error: BaseSocketServer could not find a connection to send a message.')
        }
    }
}

class ChromeSocketServer extends BaseSocketServer {
    constructor(public webdriverwrap: WebDriverWrap) {
        super(webdriverwrap)
        this.port = 8093
    }
    build() {
        this.start_build(ChromeConnection)
    }

    on_built() {
    }
}

class AppSocketServer extends BaseSocketServer {
    constructor(public webdriverwrap: WebDriverWrap) {
        super(webdriverwrap)
        this.port = 8094
    }
    build() {
        this.start_build(AppConnection)
    }

    on_built() {
        // This std output is read by the app, so that the app can open a socket to this port.
        console.log('port:' + this.port)
    }
}

// Our FSWrap.
let fswrap: FSWrap = new FSWrap()

// Our webdriver wrapper.
let webdriverwrap: WebDriverWrap = new WebDriverWrap(fswrap)

// Secure web socket to communicate with chrome extension.
ext_server = new ChromeSocketServer(webdriverwrap)
ext_server.build()

// Regular WebSocket to communicate with app.
app_server = new AppSocketServer(webdriverwrap)
app_server.build()

export function get_app_server_port() {
    return app_server.port
}

export function get_ext_server_port() {
    return ext_server.port
}

export function send_msg_to_app(msg: BaseMessage) {
    app_server.send_msg(msg)
}

export function send_msg_to_ext(msg: BaseMessage) {
    ext_server.send_msg(msg)
} 

