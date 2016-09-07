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
    iframe: string
    webdriverwrap: WebDriverWrap
    constructor(webdriverwrap: WebDriverWrap) {
        this.webdriverwrap = webdriverwrap
        this.socket = null
        this.iframe = ''
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
        console.log('nodejs got message from extension: ' + json)
        let msg = BaseMessage.create_from_string(json)
        this.iframe = msg.iframe

        // Messages from chrome get passed straight to the app.
        send_msg_to_app(msg)
    }
}

class AppConnection extends BaseConnection {
    constructor(webdriverwrap: WebDriverWrap) {
        super(webdriverwrap)
        console.log('AppConnection driver wrap is: ' + this.webdriverwrap)
    }
    //The can send us messages either from c++ or from qml.
    //These requests will be handled by webdriverjs, or the extension scripts in the browser.
    //Requests will always return with a response message containing return values.
    receive_json(json: string): void {
        let msg = BaseMessage.create_from_string(json)
        console.log('nodejs got message from app: ' + msg.to_string())

        // Extract the frame from the msg.
        console.log('nodejs got message from extension: ' + json)
        this.iframe = msg.iframe

        // Make sure the message is a request.
        if (msg.get_msg_type() == MessageType.kResponseMessage || msg.get_msg_type() == MessageType.kInfoMessage) {
            console.log("Error receive_from_app received something other than a request.")
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
                        let url = "https://www.google.com/?" + get_ext_server_port()
                        this.webdriverwrap.navigate_to(url)
                    }
                }
                this.webdriverwrap.browser_is_open(on_response.bind(this))
                // No response is sent back to the app. Typically this is called in a polling manner.
                break
            case RequestType.kResizeBrowser:
                this.webdriverwrap.resize_browser(req.args.width, req.args.height)
                // No response is sent back to the app. Typically this is called in a polling manner.
                break
            case RequestType.kOpenBrowser:
                if (this.webdriverwrap.open_browser()) {
                    send_msg_to_app(new ResponseMessage(req.iframe, true))
                } else {
                    send_msg_to_app(new ResponseMessage(req.iframe, false))
                }
                break
            case RequestType.kCloseBrowser:
                this.webdriverwrap.close_browser()
                send_msg_to_app(new ResponseMessage(req.iframe, true))
                break
            case RequestType.kNavigateTo:
                this.webdriverwrap.navigate_to(req.args.url).then(function() {
                    // Reset the webdriverwrap's iframe to the top document.
                    this.webdriverwrap.iframe = ""
                    send_msg_to_app(new ResponseMessage(req.iframe, true))
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(req.iframe, false, error))
                })
                break
            case RequestType.kNavigateBack:
                this.webdriverwrap.navigate_back().then(function() {
                    send_msg_to_app(new ResponseMessage(req.iframe, true))
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(req.iframe, false, error))
                })
                break
            case RequestType.kNavigateForward:
                this.webdriverwrap.navigate_forward().then(function() {
                    send_msg_to_app(new ResponseMessage(req.iframe, true))
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(req.iframe, false, error))
                })
                break
            case RequestType.kNavigateRefresh:
                this.webdriverwrap.navigate_refresh().then(function() {
                    send_msg_to_app(new ResponseMessage(req.iframe, true))
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(req.iframe, false, error))
                })
                break
            case RequestType.kSwitchIFrame:
                this.webdriverwrap.switch_to_iframe(req.iframe).then(function() {
                    this.webdriverwrap.iframe = req.iframe
                    send_msg_to_app(new ResponseMessage(req.iframe, true))
                }, function(error) {
                    send_msg_to_app(new ResponseMessage(req.iframe, false, error))
                })
                break
            case RequestType.kPerformAction:
                if (req.xpath) {
                    // If the xpath has been resolved by the content script,
                    // then we let webdriver perform actions.
                    switch (req.args.action) {
                        case ActionType.kSendClick:
                            this.webdriverwrap.click_on_element(req.xpath)
                            break
                        case ActionType.kMouseOver:
                            this.webdriverwrap.mouse_over_element(req.xpath, req.args.x, req.args.y)
                            break
                        case ActionType.kSendText:
                            this.webdriverwrap.send_text(req.xpath, req.args.text)
                            break
                        case ActionType.kSendEnter:
                            this.webdriverwrap.send_key(req.xpath, Key.RETURN)
                            break
                        case ActionType.kGetText:
                            this.webdriverwrap.get_text(req.xpath)
                            break
                        case ActionType.kSelectOption:
                            this.webdriverwrap.select_option(req.xpath, req.args.option_text)
                            break
                        case ActionType.kScrollDown:
                        case ActionType.kScrollUp:
                        case ActionType.kScrollRight:
                        case ActionType.kScrollLeft:
                            // Scroll actions need to be performed by the extension
                            // so we pass it through.
                            send_msg_to_ext(req)
                            break
                    }
                } else {
                    // Send request to extension to resolve the xpath,
                    // and to unblock the events in the content script so that
                    // the webdriver actions can take effect on the elements.
                    send_msg_to_ext(req)
                }
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
        console.log('start_build got error!!!')
        if (error.code === 'EADDRINUSE') {
            console.log("XXXXX the port is in use already!")
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
            res.end("Http/s server with websockets is running!\n")
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
        console.error("SocketServer failed to build.")
    }
    clean_invalid_connections() {
        let conns = this.connections
        console.log('cleaning num connections ' + conns.length + " on port: " + this.port)
        for (let i = 0; i < conns.length; i++) {
            console.log('checking index: ' + i)
            if (!conns[i].is_alive()) {
                console.log('removing socket:' + i)
                conns.splice(i, 1)
                i--
            }
        }
    }
    send_msg(msg: BaseMessage) {
        // Clean any invalid connections.
        this.clean_invalid_connections()

        // Grab the iframe.
        let iframe = msg.iframe
        console.log('trying to find conn for: -->' + iframe + '<--')

        // Send the msg through a connection which matches the iframe.
        let conns = this.connections
        for (let i = 0; i < conns.length; i++) {
            let conn = conns[i]
            console.log('conn iframe[' + i + ']: -->' + conn.iframe + '<--')
            if (conn.is_alive()) { // && conn.iframe == iframe
                conn.send_json(msg.to_string())
                return
            }
        }

        // If we get here, we didn't a find a matching connection.
        console.error('Error: No connection with matching iframe found during attempt to send message.')
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
        // console.log('ChromeSocketServer built on port: ' + this.port)
        // let text = 'var g_nodejs_port = ' + this.port
        // FSWrap.write_to_file(ChromeSocketServer.extension_server_file, text)
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
        console.log('port:' + this.port)
        // FSWrap.write_to_file(AppSocketServer.app_server_file, this.port.toString())
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

