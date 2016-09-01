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
    }
    receive_message(msg: string): void {
    }
}

class ChromeConnection extends BaseConnection{
    receive_message(s: string): void {
        console.log('nodejs got message from extension: ' + s)
        let msg = new SocketMessage()
        msg.set_from_string(s)
        if (msg.is_request()) {
            // We have a request from the extension.
            send_to_app(s);
        } else {
            // We have a response from the extension.
            // We don't handle any responses ourself. We just relay them to the app.
            send_to_app(msg.get_obj())
        }
    }
}

class AppConnection extends BaseConnection {
    //The can send us messages either from c++ or from qml.
    //These requests will be handled by webdriverjs, or the extension scripts in the browser.
    //Requests will always return with a response message containing return values.
    receive_message(message: string): void {
        let msg = new SocketMessage()
        msg.set_from_string(message)
        console.log('nodejs got message from app: ' + message)

        // Make sure the message is a request.
        if (!msg.is_request()) {
            console.log("Error receive_from_app received something other than a request.")
        }

        // Handle the request.
        let request = msg.get_obj()
        switch (request.request) {
            case 'shutdown': {
                WebDriverWrap.close_browser().then(function () { process.exit(-1) })
                break;
            }
            case 'check_browser_is_open':
                function on_response(open: boolean) {
                    if (!open) {
                        WebDriverWrap.open_browser()
                    }
                }
                WebDriverWrap.browser_is_open(on_response)
                // No response is sent back to the app. Typically this is called in a polling manner.
                break
            case 'resize_browser':
                WebDriverWrap.resize_browser(request.width, request.height)
                // No response is sent back to the app. Typically this is called in a polling manner.
                break
            case 'open_browser':
                WebDriverWrap.open_browser()
                send_to_app({ response: true })
                break
            case 'close_browser':
                WebDriverWrap.close_browser()
                send_to_app({ response: true })
                break
            case 'navigate_to':
                WebDriverWrap.navigate_to(request.url).then(function () {
                    send_to_app({ response: true })
                }, function (error) {
                    send_to_app({ response: false })
                })
                break
            case 'navigate_back':
                WebDriverWrap.navigate_back().then(function () {
                    send_to_app({ response: true })
                }, function (error) {
                    send_to_app({ response: false })
                })
                break
            case 'navigate_forward':
                WebDriverWrap.navigate_forward().then(function () {
                    send_to_app({ response: true })
                }, function (error) {
                    send_to_app({ response: false })
                })
                break
            case 'navigate_refresh':
                WebDriverWrap.navigate_refresh().then(function () {
                    send_to_app({ response: true })
                }, function (error) {
                    send_to_app({ response: false })
                })
                break
            case 'perform_action':
                if (request.xpath) {
                    // If the xpath has been resolved by the content script,
                    // then we let webdriver perform actions.
                    switch (request.action) {
                        case 'send_click':
                            WebDriverWrap.click_on_element(request.xpath)
                            break
                        case 'send_text':
                            WebDriverWrap.send_text(request.xpath, request.text)
                            break
                        case 'send_enter':
                            WebDriverWrap.send_key(request.xpath, WebDriverWrapModule.Key.RETURN)
                            break
                        case 'get_text':
                            WebDriverWrap.get_text(request.xpath)
                            break
                        case 'select_option':
                            WebDriverWrap.select_option(request.xpath, request.option_text)
                            break
                        case 'scroll_down':
                        case 'scroll_up':
                        case 'scroll_right':
                        case 'scroll_left':
                            // Scroll actions need to be performed by the extension
                            // so we pass it through.
                            send_to_extension(request)
                            break
                    }
                } else {
                    // Send request to extension to resolve the xpath,
                    // and to unblock the events in the content script so that
                    // the webdriver actions can take effect on the elements.
                    send_to_extension(request)
                }
                break
            default:
                // By default we send requests to the extension. 
                // (It goes through bg script, and then into content script.)
                console.log('zzzzzzzzzzzzzzzzzz')
                send_to_extension(request)
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
    send_message_to_all_sockets(obj: any) {
        let socket_message = new SocketMessage()
        socket_message.set_from_obj(obj)
        let sockets = extension_server.sockets
        for (let i = 0; i<sockets.length; i++) {
            sockets[i].send_message(socket_message.get_text())
        }
    }
}

class ChromeSocketServer extends BaseSocketServer {
    static extension_server_file = Path.join(FSWrap.g_nodejs_dir, 'chromeextension', 'extensionserverport.js') // g_user_data_dir

    build() {
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

//Send an obj as a message to the app.
export function send_to_app(obj: any) {
    var socket_message = new SocketMessage()
    socket_message.set_from_obj(obj)
    app_server.send_message_to_all_sockets(socket_message.get_text())
}

//Send an obj as a message to the extension.
export function send_to_extension(obj: any) {
    var socket_message = new SocketMessage()
    socket_message.set_from_obj(obj)
    extension_server.send_message_to_all_sockets(socket_message.get_text())
} 



