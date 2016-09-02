/// <reference path="D:\dev\windows\DefinitelyTyped\node\node.d.ts"/>
/// <reference path="D:\dev\windows\DefinitelyTyped\ws\ws.d.ts"/>
/// <reference path="D:\dev\windows\DefinitelyTyped\request\request.d.ts"/>
/// <reference path="D:\dev\windows\DefinitelyTyped\selenium-webdriver\selenium-webdriver.d.ts"/>


//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

// Note default arguments for function requires ES6.
// This version of nodejs doesn't seem to have ES6 yet.

// We use strict mode. In strict mode we can't use undeclared variables.
//"use strict";

// Our imports.
import ws = require('ws')
import HttpServer = require('http')
import HttpsServer = require('https')
import request = require('request');
import Path = require('path')

import FSWrapModule = require('./fswrap')
let FSWrap = FSWrapModule.FSWrap

import WebDriverWrapModule = require('./webdriverwrap')
let WebDriverWrap = WebDriverWrapModule.WebDriverWrap

// import SocketMessageModule = require('./socketmessage')
// let SocketMessage = SocketMessageModule.SocketMessage

import SM = require('./socketmessage')

// Secure web socket to communicate with chrome extension.
let extension_server: ChromeSocketServer = null

// Regular WebSocket to communicate with app.
let app_server: AppSocketServer = null

// Make sure the nodejs dir exists in the user data dir.
{
    let dir = Path.join(FSWrap.g_user_data_dir, "nodejs")
    if (!FSWrap.file_exists(dir)) {
        FSWrap.create_dir(dir)
    }
}

//------------------------------------------------------------------------------------------------
//Socket server creation.
//------------------------------------------------------------------------------------------------

class BaseConnection {
    socket: ws
    constructor() {
    }
    send_message(msg: string) :void {
        this.socket.send(msg)
    }
    receive_message(msg: string): void {
    }
}

class ChromeConnection extends BaseConnection{
    receive_message(json: string): void {
        // Messages from chrome get passed straight to the app.
        console.log('nodejs got message from extension: ' + json)
        send_to_app(json)
    }
}

class AppConnection extends BaseConnection {
    //The can send us messages either from c++ or from qml.
    //These requests will be handled by webdriverjs, or the extension scripts in the browser.
    //Requests will always return with a response message containing return values.
    receive_message(json: string): void {
        let bm = SM.BaseMessage.create_from_string(json)
        console.log('nodejs got message from app: ' + bm.to_string())

        // Make sure the message is a request.
        if (!bm.is_request()) {
            console.log("Error receive_from_app received something other than a request.")
        }
        let rm = <SM.RequestMessage>bm

        // Handle the request.
        switch (rm.request) {
            case SM.RequestType.kShutdown: {
                WebDriverWrap.close_browser().then(function () { process.exit(-1) })
                break;
            }
            case SM.RequestType.kCheckBrowserIsOpen:
                function on_response(open: boolean) {
                    if (!open) {
                        WebDriverWrap.open_browser()
                    }
                }
                WebDriverWrap.browser_is_open(on_response)
                // No response is sent back to the app. Typically this is called in a polling manner.
                break
            case SM.RequestType.kResizeBrowser:
                WebDriverWrap.resize_browser(rm.args.width, rm.args.height)
                // No response is sent back to the app. Typically this is called in a polling manner.
                break
            case SM.RequestType.kOpenBrowser:
                WebDriverWrap.open_browser()
                send_message_to_app(new SM.ResponseMessage(true))
                break
            case SM.RequestType.kCloseBrowser:
                WebDriverWrap.close_browser()
                send_message_to_app(new SM.ResponseMessage(true))
                break
            case SM.RequestType.kNavigateTo:
                WebDriverWrap.navigate_to(rm.args.url).then(function () {
                    send_message_to_app(new SM.ResponseMessage(true))
                }, function (error) {
                    send_message_to_app(new SM.ResponseMessage(false))
                })
                break
            case SM.RequestType.kNavigateBack:
                WebDriverWrap.navigate_back().then(function () {
                    send_message_to_app(new SM.ResponseMessage(true))
                }, function (error) {
                    send_message_to_app(new SM.ResponseMessage(false))
                })
                break
            case SM.RequestType.kNavigateForward:
                WebDriverWrap.navigate_forward().then(function () {
                    send_message_to_app(new SM.ResponseMessage(true))
                }, function (error) {
                    send_message_to_app(new SM.ResponseMessage(false))
                })
                break
            case SM.RequestType.kNavigateRefresh:
                WebDriverWrap.navigate_refresh().then(function () {
                    send_message_to_app(new SM.ResponseMessage(true))
                }, function (error) {
                    send_message_to_app(new SM.ResponseMessage(false))
                })
                break
            case SM.RequestType.kPerformAction:
                if (rm.xpath) {
                    // If the xpath has been resolved by the content script,
                    // then we let webdriver perform actions.
                    switch (rm.args.action) {
                        case SM.ActionType.kSendClick:
                            WebDriverWrap.click_on_element(rm.xpath)
                            break
                        case SM.ActionType.kSendText:
                            WebDriverWrap.send_text(rm.xpath, rm.args.text)
                            break
                        case SM.ActionType.kSendEnter:
                            WebDriverWrap.send_key(rm.xpath, WebDriverWrapModule.Key.RETURN)
                            break
                        case SM.ActionType.kGetText:
                            WebDriverWrap.get_text(rm.xpath)
                            break
                        case SM.ActionType.kSelectOption:
                            WebDriverWrap.select_option(rm.xpath, rm.args.option_text)
                            break
                        case SM.ActionType.kScrollDown:
                        case SM.ActionType.kScrollUp:
                        case SM.ActionType.kScrollRight:
                        case SM.ActionType.kScrollLeft:
                            // Scroll actions need to be performed by the extension
                            // so we pass it through.
                            send_to_extension(json)
                            break
                    }
                } else {
                    // Send request to extension to resolve the xpath,
                    // and to unblock the events in the content script so that
                    // the webdriver actions can take effect on the elements.
                    send_to_extension(json)
                }
                break
            default:
                // By default we send requests to the extension. 
                // (It goes through bg script, and then into content script.)
                console.log('zzzzzzzzzzzzzzzzzz')
                send_to_extension(json)
                break

        }
    }
}

class BaseSocketServer{
    // This can easily be upgraded to handle non ssl http as well.
    // let http_server: HttpServer.Server
    // http_server = HttpServer.createServer(dummy_handler)

    static ssl = true
    static ssl_key =  './key.pem'
    static ssl_cert = './cert.pem'

    protected port: number
    private https_server: HttpsServer.Server
    private server: ws.Server
    private sockets: BaseConnection[]

    constructor() {
        this.port = 8093
        this.https_server = null
        this.server = null
        this.sockets = []
    }

    protected start_build<C extends BaseConnection>(ConnConstructor: { new (): C; }) {
        // Dummy handler for http/s requests.
        let dummy_handler = function (req: HttpServer.IncomingMessage, res: HttpServer.ServerResponse) {
            res.writeHead(200);
            res.end("Http/s server with websockets is running!\n")
        }.bind(this);

        // Create the http/s server.
        this.https_server = HttpsServer.createServer({
            key: FSWrap.read_from_file(BaseSocketServer.ssl_key),
            cert: FSWrap.read_from_file(BaseSocketServer.ssl_cert)
        }, dummy_handler)

        // Once the https server reports an error, we try to create the https server on a different port.
        this.https_server.once('error', function (error: Error) {
            console.log('start_build got error!!!')
            if (error.name === 'EADDRINUSE') {
                console.log("XXXXX the port is in use already!")
                // The current config port number is in use.
                // So we increment it and recurse.
                this.port += 1
                this.build(ConnConstructor)
            } else {
                // Call the callback with failed result.
                this.on_built(false)
            }
        }.bind(this));

        // Once the https server is listening, we try to create the WebSocket server.
        this.https_server.once('listening', function () {
            // Create the socket server.
            // The following arg's type should be ws.IServerOptions but DefinitelyTyped seems to have a bug.
            this.server = new ws.Server({ server: this.https_server })
            this.server.on('connection', function (socket: ws) {
                let conn: C = new ConnConstructor();
                conn.socket = socket
                socket.on('message', conn.receive_message.bind(conn))
                this.sockets.push(conn)
            }.bind(this))
            // Now call the callback.
            this.on_built(true)
        }.bind(this));

        this.https_server.listen(this.port)
    }

    // These should be overridden in derived classes.
    build() {}
    on_built(success: boolean) {}
    send_message_to_all_sockets(json: string) {
        let sockets = this.sockets
        console.log('num sockets is: ' + sockets.length + " on port: " + this.port)
        for (let i = 0; i<sockets.length; i++) {
            sockets[i].send_message(json)
        }
    }
}

class ChromeSocketServer extends BaseSocketServer {
    static extension_server_file = Path.join(FSWrap.g_nodejs_dir, 'chromeextension', 'extensionserverport.js') // g_user_data_dir

    build() {
        this.port = 8093
        this.start_build(ChromeConnection)
    }

    on_built(success: boolean) {
        let text = 'var g_nodejs_port = ' + this.port
        FSWrap.write_to_file(ChromeSocketServer.extension_server_file, text)
    }
}

class AppSocketServer extends BaseSocketServer {
    static app_server_file = Path.join(FSWrap.g_user_data_dir, 'nodejs','appserverport.txt')

    build() {
        this.port = 8094
        this.start_build(AppConnection)
    }

    on_built(success: boolean) {
        FSWrap.write_to_file(AppSocketServer.app_server_file, this.port.toString())
    }
}


// Secure web socket to communicate with chrome extension.
extension_server = new ChromeSocketServer()
extension_server.build()

// Regular WebSocket to communicate with app.
app_server = new AppSocketServer()
app_server.build()

// Send an obj as a message to the app.
export function send_to_app(json: string) {
    app_server.send_message_to_all_sockets(json)
}

export function send_message_to_app(msg: SM.BaseMessage) {
    app_server.send_message_to_all_sockets(msg.to_string())
}

// Send an obj as a message to the extension.
export function send_to_extension(json: string) {
    extension_server.send_message_to_all_sockets(json)
} 

export function send_message_to_extension(msg: SM.BaseMessage) {
    extension_server.send_message_to_all_sockets(msg.to_string())
} 



