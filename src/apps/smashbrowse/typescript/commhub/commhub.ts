//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

// Our imports.
import ws = require('ws') // Note although similar to WebSocket (es6), ws is the nodejs type that we want to use not WebSocket.
import Http = require('http')
import Https = require('https')
import Request = require('request');
import Path = require('path')
import firebase = require("firebase");

// Our own modules.
import {FSWrap} from './fswrap'
import {WebDriverWrap, Key} from './webdriverwrap'
import {DebugUtils} from './debugutils'
import {FirebaseWraps} from './firebasewrap'

// Other libs that needed to be separated due to cross platform import issues.
import {robotjs, active_window} from './libs'

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
    protected socket: ws
    protected webdriverwrap: WebDriverWrap
    constructor(webdriverwrap: WebDriverWrap) {
        this.webdriverwrap = webdriverwrap
        this.socket = null
    }
    set_socket(socket: ws) {
        this.socket = socket
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
        console.log('ext --> commhub: ' + json)

        // Process info messages.
        if (msg.get_msg_type() == MessageType.kInfoMessage) {
            let info_msg = <InfoMessage>msg
            switch(info_msg.info) {
                case InfoType.kTabCreated: {
                    // Don't do anything. 
                    // Each node will have a sequence of tasks all queued up.
                    // And each of the tasks is expecting to operate on the old existing tab
                    // instead of the newly created tab. 
                    // The last task is usually to update to the latest tab.
                } return
                case InfoType.kTabDestroyed: {
                    this.webdriverwrap.update_current_tab()
                } return
            }
        }

        // Messages from chrome get passed straight to the app.
        send_msg_to_app(msg)
    }
}

class AppConnection extends BaseConnection {
    timer: number
    constructor(webdriverwrap: WebDriverWrap) {
        super(webdriverwrap)
        this.timer = null
    }

    // The can send us messages either from c++ or from qml.
    // These requests will be handled by webdriverjs, or the extension scripts in the browser.
    // Requests will always return with a response message containing return values.
    receive_json(json: string): void {
        console.log('app --> commhub: ' + json)

        // Build the message.
        let msg = BaseMessage.create_from_string(json)



        // Make sure the message is a request.
        if (msg.get_msg_type() == MessageType.kResponseMessage || msg.get_msg_type() == MessageType.kInfoMessage) {
            console.error("Error: AppConnection currently always expects a request.")
        }
        let req = <RequestMessage>msg

        // Handle the request.
        switch (req.request) {
            case RequestType.kShutdown: {
                this.webdriverwrap.close_browser().then(() => { process.exit(-1) })
                break;
            }
            case RequestType.kOpenBrowser: {
                let on_response = (open: boolean) => {
                    if (!open) {
                        if (!this.webdriverwrap.open_browser()) {
                            send_msg_to_app(new ResponseMessage(msg.id, false, false))
                            return
                        }
                        // Now convert this request to navigate to a default url with a port number.
                        let url = "https://www.google.com/?" + get_ext_server_port()
                        let relay = new RequestMessage(msg.id, RequestType.kNavigateTo, { url: url })
                        this.receive_json(relay.to_string())
                    } else {
                        send_msg_to_app(new ResponseMessage(msg.id, true, true))
                    }
                }
                this.webdriverwrap.browser_is_open(on_response)
            } break
            case RequestType.kResizeBrowser: {
                this.webdriverwrap.resize_browser(req.args.width, req.args.height).then(
                    () => { send_msg_to_app(new ResponseMessage(msg.id, true, true)) },
                    () => { send_msg_to_app(new ResponseMessage(msg.id, false, false)) }
                    )
            } break
            case RequestType.kIsBrowserOpen: {
                let on_response = (open: boolean) => {
                    if (open) {
                        send_msg_to_app(new ResponseMessage(msg.id, true, true))
                    } else {
                        send_msg_to_app(new ResponseMessage(msg.id, true, false))
                    }
                }
                this.webdriverwrap.browser_is_open(on_response)
            } break
            case RequestType.kCloseBrowser: {
                // Clear all the connections to our extension socket server.
                ext_server.clear_connections()
                let on_response = (open: boolean) => {
                    if (!open) {
                        send_msg_to_app(new ResponseMessage(msg.id, true, true))
                    } else {
                        this.webdriverwrap.close_browser().then(
                            () => { send_msg_to_app(new ResponseMessage(msg.id, true, true)) },
                            () => { send_msg_to_app(new ResponseMessage(msg.id, false, false)) })
                    }
                }
                this.webdriverwrap.browser_is_open(on_response)

            } break
            case RequestType.kNavigateTo: {
                this.webdriverwrap.navigate_to(req.args.url).then(() => {
                    send_msg_to_app(new ResponseMessage(msg.id, true, true))
                }, (error: Error) => {
                    send_msg_to_app(new ResponseMessage(msg.id, false, error.message))
                })
            } break
            case RequestType.kNavigateBack: {
                this.webdriverwrap.navigate_back().then(() => {
                    send_msg_to_app(new ResponseMessage(msg.id, true, true))
                }, (error: Error) => {
                    send_msg_to_app(new ResponseMessage(msg.id, false, error.message))
                })
            } break
            case RequestType.kNavigateForward: {
                this.webdriverwrap.navigate_forward().then(() => {
                    send_msg_to_app(new ResponseMessage(msg.id, true, true))
                }, (error: Error) => {
                    send_msg_to_app(new ResponseMessage(msg.id, false, error.message))
                })
            } break
            case RequestType.kNavigateRefresh: {
                this.webdriverwrap.navigate_refresh().then(() => {
                    send_msg_to_app(new ResponseMessage(msg.id, true, true))
                }, (error: Error) => {
                    send_msg_to_app(new ResponseMessage(msg.id, false, error.message))
                })
            } break
            case RequestType.kUpdateCurrentTab: {
                this.webdriverwrap.update_current_tab().then(
                    () => {send_msg_to_ext(req)},
                    (error) => {send_msg_to_app(new ResponseMessage(msg.id, false, error.message))}
                )
            } break
            case RequestType.kDestroyCurrentTab: {
                this.webdriverwrap.destroy_current_tab().then(
                    () => {send_msg_to_app(new ResponseMessage(msg.id, true, true))},
                    (error) => {send_msg_to_app(new ResponseMessage(msg.id, false, error.message))}
                )
            } break
            case RequestType.kPerformMouseAction: {
                switch (req.args.mouse_action) {
                    case MouseActionType.kSendClick: {
                        let p = this.webdriverwrap.click_on_element(req.args.frame_index_path, req.args.xpath, req.args.local_mouse_position, false)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    case MouseActionType.kSendCtrlClick: {
                        let p = this.webdriverwrap.click_on_element(req.args.frame_index_path, req.args.xpath, req.args.local_mouse_position, true)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    case MouseActionType.kMouseOver: {
                        let p = this.webdriverwrap.mouse_over_element(req.args.frame_index_path, req.args.xpath, req.args.local_mouse_position)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    default: {
                        send_msg_to_app(new ResponseMessage(msg.id, false, "unknown mouse action"))
                    }
                }
            } break
            case RequestType.kPerformTextAction: {
                switch (req.args.text_action) {
                    case TextActionType.kSendText: {
                        let p = this.webdriverwrap.send_text(req.args.frame_index_path, req.args.xpath, req.args.text)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    case TextActionType.kSendEnter: {
                        let p = this.webdriverwrap.send_key(req.args.frame_index_path, req.args.xpath, Key.RETURN)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    default: {
                        send_msg_to_app(new ResponseMessage(msg.id, false, "unknown text action"))
                    }
                }
            } break
            case RequestType.kPerformElementAction: {
                switch (req.args.element_action) {
                    case ElementActionType.kGetText: {
                        let p = this.webdriverwrap.get_text(req.args.frame_index_path, req.args.xpath)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    case ElementActionType.kChooseOption: {
                        let p = this.webdriverwrap.choose_option(req.args.frame_index_path, req.args.xpath, req.args.option_text)
                        WebDriverWrap.terminate_chain(p, req.id)
                    } break
                    case ElementActionType.kScroll: {
                        // Scroll actions need to be performed by the extension
                        // so we pass it through.
                        send_msg_to_ext(req)
                    } break
                    default: {
                        send_msg_to_app(new ResponseMessage(msg.id, false, "unknown element action"))
                    }
                }
            } break
            case RequestType.kGetCurrentURL: {
                let p = this.webdriverwrap.get_current_url()
                WebDriverWrap.terminate_chain(p, req.id)
            } break
            case RequestType.kOpenTab: {
                send_msg_to_ext(req)
            } break
            case RequestType.kDownloadFiles: {
                let p = this.webdriverwrap.download_files()
                WebDriverWrap.terminate_chain(p, req.id)
            } break
            case RequestType.kGetActiveWindowInOS: {
                let callback = (window: any) => {
                    console.log("App: " + window.app);
                    console.log("Title: " + window.title);
                    // The name of the app is in window.app.
                    // The title of the app window is in window.title.
                    send_msg_to_app(new ResponseMessage(msg.id, true, window))
                }
                active_window.getActiveWindow(callback, 1, 0)
            } break
            case RequestType.kTapKeysInOS: {
                // Finds all windows with the specified application name and window title, and interacts with it.
                // We detect if we've looped through all the windows by counting how many times we see the smashbrowse app.
                let smashbrowse_count = 0
                let callback = (window: any) => {
                    console.log("eeeeeeeeeeeeee examining app name: " + window.app + " title: " + window.title)
                    if ((window.app == req.args.application_name) && (window.title == req.args.window_title)) {
                        // Tap out the keys. Todo: maybe later we will support modifiers.
                        for (let i = 0; i < req.args.keys.length; i++) {
                            robotjs.keyTap(req.args.keys[i]);
                        }
                        // If all_matching_windows is true, then we tap out the keys for all windows with the same app name and title.
                        if (!req.args.all_matching_windows) {
                            send_msg_to_app(new ResponseMessage(msg.id, true, true))
                            return
                        }
                    } else if ((window.app == "smashbrowse") && (window.title == "Smash Browse")) {
                        smashbrowse_count += 1
                        // If we've seen the smash browse window twice then we've seen all the windows.
                        if (smashbrowse_count >= 2) {
                            send_msg_to_app(new ResponseMessage(msg.id, true, true))
                            return
                        }
                    }
                    // Switch focus to another window.
                    robotjs.keyTap("escape", "alt");
                    // Get the active window info. (Recursion)
                    active_window.getActiveWindow(callback, 1, 0)
                }
                active_window.getActiveWindow(callback, 1, 0)
            } break
            default: {
                console.log('checking if firebase can handle request')
                if (firebasewraps.handle_request(req)) {
                    console.log('yup it can')
                    return
                }
                // By default we send requests to the extension. 
                // (It goes through bg script, and then into content script.)
                send_msg_to_ext(req)
            } break
        }
    }
}

class BaseSocketServer {
    // This can easily be upgraded to handle non ssl http as well.
    // let http_server: Http.Server
    // http_server = Http.createServer(dummy_handler)

    private use_ssl: boolean
    private ssl_key: string
    private ssl_cert: string

    public port: number
    private web_server: Https.Server | Http.Server
    private socket_server: ws.Server
    private connections: BaseConnection[]

    webdriverwrap: WebDriverWrap

    constructor(webdriverwrap: WebDriverWrap) {
        this.webdriverwrap = webdriverwrap
        this.port = 8093
        this.web_server = null
        this.socket_server = null
        this.connections = []
        this.use_ssl = true
        this.ssl_key = './key.pem'
        this.ssl_cert = './cert.pem'
    }

    protected on_web_server_error(error: NodeJS.ErrnoException): void {
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
        let options: any = { server: this.web_server }
        this.socket_server = new ws.Server(options)
        let on_connection = (socket: ws) => {
            let conn: C = new ConnConstructor(this.webdriverwrap);
            conn.set_socket(socket)
            socket.on('message', (json: string) => { conn.receive_json(json) })
            this.connections.push(conn)
        }
        this.socket_server.on('connection', on_connection)
        // Now call the callback.
        this.on_built()
    }

    protected start_build<C extends BaseConnection>(ConnConstructor: { new (wdw: WebDriverWrap): C; }) {
        // Dummy handler for http/s requests.
        let dummy_handler = (req: Http.IncomingMessage, res: Http.ServerResponse) => {
            res.writeHead(200);
            res.end("Https server is running.\n")
        }

        // Create the http/s server.
        if (this.use_ssl) {
            this.web_server = Https.createServer({
                key: FSWrap.read_from_file(this.ssl_key),
                cert: FSWrap.read_from_file(this.ssl_cert)
            }, dummy_handler)
        } else {
             this.web_server = Http.createServer(dummy_handler)
        }

        // Once the https server reports an error, we try to create the https server on a different port.
        this.web_server.once('error', (error: NodeJS.ErrnoException) => { this.on_web_server_error(error) })

        // Once the https server is listening, we try to create the WebSocket server.
        this.web_server.once('listening', (): void => { this.on_https_server_listening(ConnConstructor) })

        this.web_server.listen(this.port)
    }

    // These should be overridden in derived classes.
    build() { }
    on_built() { }
    on_build_failed() {
        console.error("Error: SocketServer failed to build.")
    }
    clear_connections() {
        this.connections.length = 0
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


        if (this.connections.length != 1) {
            console.error('Error: base socket server was expecting just one connection instead of: ' + this.connections.length)
        }

        // Send the msg through all connections which are alive.
        let sent = false
        let conns = this.connections
        for (let i = 0; i < conns.length; i++) {
            let conn = conns[i]
            if (conn.is_alive()) {
                conn.send_json(msg.to_string())
                sent = true
            }
        }
        if (!sent) {
            console.error('Error: base socket server could not find a connection to send a message.')
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

export class AppSocketServer extends BaseSocketServer {
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


// Our firebase wrapper.
let firebasewraps: FirebaseWraps = new FirebaseWraps(app_server)
