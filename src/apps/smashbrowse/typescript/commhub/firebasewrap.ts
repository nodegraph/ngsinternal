/// <reference path="D:\installs\srcdeps\ngsexternal\nodejs2\node_modules\firebase\firebase.d.ts"/>

import firebase = require("firebase");

import {AppSocketServer} from './commhub'

// There is one firebase wrap per firebase node in the node graph.
export class FirebaseWrap {

    private firebase_app: firebase.app.App
    private app_server: AppSocketServer
    private signed_in: boolean
    
    // Maps node_path to its listener.
    private listeners: any

    static js_to_value(js: string): any {
        try {
            let expr = 'unique_variable_to_eval_js = ' + js
            return eval(expr) 
        } catch (error) {
            return js
        }
    }

    static value_to_js(value: any): string {
        return JSON.parse(value)
    }

    // let config = {
    //     apiKey: "AIzaSyCXGNlyRf5uk8Xk1bvKXUcA53TC6Lc3I-A",
    //     authDomain: "test-project-91c10.firebaseapp.com",
    //     databaseURL: "https://test-project-91c10.firebaseio.com/",
    //     storageBucket: "gs://test-project-91c10.appspot.com",
    // };
    constructor(config: any, config_key: string, app_server: AppSocketServer) {
        this.firebase_app = firebase.initializeApp(config, config_key); 
        this.app_server = app_server
        this.signed_in = false
        this.listeners = {}

        console.log('firebase app initialized with config: ' + JSON.stringify(config))
    }

    // Returns true if the request type matches something we can handle.
    handle_request(req: RequestMessage): boolean {
        switch (req.request) {
            case RequestType.kFirebaseSignIn: {
                this.sign_in(req)
            } break
            case RequestType.kFirebaseSignOut: {
                this.sign_out(req)
            } break
            case RequestType.kFirebaseWriteData: {
                this.write_data(req)
            } break
            case RequestType.kFirebaseReadData: {
                this.read_data(req)
            } break
            case RequestType.kFirebaseSubscribe: {
                this.subscribe(req)
            } break
            case RequestType.kFirebaseUnsubscribe: {
                this.unsubscribe(req)
            } break
            default: {
                return false
            }
        }
        return true
    }

    // Attempts to sign into account. This will create an account if one does not exist for the given email.
    sign_in(req: RequestMessage): void {
        console.log('got sign in request')
        try {
            this.firebase_app.auth().signInWithEmailAndPassword(req.args.email, req.args.password).then(
                (a: any) => {
                    this.signed_in = true
                    this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
                },
                (error: Error) => {
                    // The user account may not exist yet if this is the first time trying to sign in.
                    // So we try to create an account.
                    this.firebase_app.auth().createUserWithEmailAndPassword(req.args.email, req.args.password).then(
                        (a: any) => {
                            this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
                        },
                        (error: Error) => {
                            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
                        }
                    )
                } 
            )
        } catch (e) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, e.message))
        }
    }

    // Sign out.
    sign_out(req: RequestMessage): void {
        this.firebase_app.auth().signOut().then(
            () => {
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
            }, 
            (error: Error) => {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
        });
    }

    get_full_data_path(data_path: string) {
        let currentUser = this.firebase_app.auth().currentUser
        let full_data_path = 'users/' + currentUser.uid;
        if (data_path.length) {
            if (data_path.charAt(0) == '/') {
                full_data_path = full_data_path + data_path
            } else {
                full_data_path = full_data_path + '/' + data_path
            }
        }
        return full_data_path
    }

    // Write data.
    write_data(req: RequestMessage): void {
        if (!this.signed_in) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, "Error: Not signed into firebase. Check surrounding firebase group settings."))
            return
        }
        let full_data_path = this.get_full_data_path(req.args.data_path)
        this.firebase_app.database().ref(full_data_path).set(FirebaseWrap.js_to_value(req.args.value)).then(
            (a: any) => {
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
            },
            (error: Error) => {
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
            }
        )
    }

    // Read data.
    read_data(req: RequestMessage): void {
        if (!this.signed_in) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, "Error: Not signed into firebase. Check surrounding firebase group settings."))
            return
        }
        let full_data_path = this.get_full_data_path(req.args.data_path)
        this.firebase_app.database().ref(full_data_path).once('value').then(
            (data: firebase.database.DataSnapshot) => {
                console.log('got value: ' + JSON.stringify(data.exportVal()))
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, data.exportVal()))
            },
            (error: Error) => {
                console.log('got error: ' + error.message)
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
            }
        )
    }

    // Listen to changes.
    subscribe(req: RequestMessage): void {
        if (!this.signed_in) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, "Error: Not signed into firebase. Check surrounding firebase group settings."))
            return
        }
        let full_data_path = this.get_full_data_path(req.args.data_path)

        // Remove previous listener under the node path.
        this.remove_listener(req.args.node_path, req.args.data_path)

        // Cache our next listener under the node path.
        this.listeners[req.args.node_path] =  (data: firebase.database.DataSnapshot) => {
            let info: any = {}
            info.node_path = req.args.node_path
            info.data_path = req.args.data_path
            info.value = data.exportVal()
            this.app_server.send_msg(new InfoMessage(req.id, "-1", InfoType.kFirebaseChanged, info))
        }

        // Attach the listener.
        this.firebase_app.database().ref(full_data_path).on('value', this.listeners[req.args.node_path])
        // Send our response.
        this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
    }

    unsubscribe(req: RequestMessage): void {
        if (!this.signed_in) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, "Error: Not signed into firebase. Check surrounding firebase group settings."))
            return
        }
        this.remove_listener(req.args.node_path, req.args.data_path)
        // Send our response.
        this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
    }

    remove_listener(node_path: string, data_path: string):void {
        let full_data_path = this.get_full_data_path(data_path)
        // Use the node path to get the cached listener. Use it to remove the listener.
        this.firebase_app.database().ref(full_data_path).off('value', this.listeners[node_path])
        // Destroy the listener under the node path.
        delete this.listeners[node_path]
    }

}


export class FirebaseWraps {
    private wraps: any
    private app_server: AppSocketServer
    private request_ids: Array<RequestType>

    constructor(app_server: AppSocketServer) {
        this.app_server = app_server
        this.clear()
        this.request_ids = [
            RequestType.kFirebaseInit,
            RequestType.kFirebaseSignIn,
            RequestType.kFirebaseSignOut,
            RequestType.kFirebaseWriteData,
            RequestType.kFirebaseReadData,
            RequestType.kFirebaseSubscribe,
            RequestType.kFirebaseUnsubscribe]
    }

    can_handle_request(type: RequestType): boolean {
        if (this.request_ids.indexOf(type) >= 0) {
            return true
        }
        return false
    }

    // Clear our existing wraps.
    private clear() {
        this.wraps = {}
    }

    private get_group_path(node_path: string) {
        let splits = node_path.split('/')
        splits.pop()
        return splits.join('/')
    }

    // Creates a firebase wrap and caches it under its key for easy access at a later time.
    private create_wrap(group_path: string, config: any) {
        let wrap = new FirebaseWrap(config, group_path, this.app_server)
        this.wraps[group_path] = wrap
    }

    // Checks to see if a wrap exists for certain config.
    private wrap_exists(group_path: string) {
        if (this.wraps.hasOwnProperty(group_path)) {
            return true
        }
        return false
    }

    private get_wrap(group_path: string) {
        if (this.wrap_exists(group_path)) {
            return this.wraps[group_path]
        }
        return undefined
    }

    handle_request(req: RequestMessage): boolean {
        // Return right away if we can't handle this request type.
        if (!this.can_handle_request(req.request)) {
            return false
        }

        // Get the group path which surrounds the node making the request.
        let group_path = this.get_group_path(req.args.node_path)

        // Handle creating the firebase wrapper.
        if (req.request == RequestType.kFirebaseInit) {
            let config: any = {}
            config.apiKey = req.args.apiKey
            config.authDomain = req.args.authDomain
            config.databaseURL = req.args.databaseURL
            config.storageBucket = req.args.storageBucket
            config.email = req.args.email
            config.password = req.args.password
            console.log('setting current firebase config: ' + JSON.stringify(config))
            // Make sure the wrap is created.
            if (!this.wrap_exists(group_path)) {
                this.create_wrap(group_path, config)
            }
            // Send back a response.
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
            return true
        } else if (req.request == RequestType.kFirebaseDestroy) {
            console.log('firebase destroy called')
            delete this.wraps[group_path]
        }

        // Otherwise we let the existing firebase wrap try to handle it.
        let wrap = this.get_wrap(group_path)
        return wrap.handle_request(req)
    }
}