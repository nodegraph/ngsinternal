/// <reference path="D:\installs\srcdeps\ngsexternal\nodejs2\node_modules\firebase\firebase.d.ts"/>

import firebase = require("firebase");

import { AppSocketServer } from './commhub'

// Structure which keeps track of listeners for one node.
export class NodeListenerTracker {
    private listeners: { [s: string]: (a: firebase.database.DataSnapshot, b?: string) => any }
    constructor() {
        this.listeners = {}
    }
    add_listener(data_path: string, listener: (a: firebase.database.DataSnapshot, b?: string) => any) {
        this.listeners[data_path] = listener
    }
    get_listener(data_path: string) {
        if (this.listeners.hasOwnProperty(data_path)) {
            return this.listeners[data_path]
        }
        return undefined
    }
    remove_listener(data_path: string) {
        delete this.listeners[data_path]
    }
    clear() {
        this.listeners = {}
    }
}

// There is one firebase wrap per firebase node in the node graph.
export class FirebaseWrap {

    private firebase_app: firebase.app.App
    private app_server: AppSocketServer
    private signed_in: boolean

    // Maps node_path to its listener.
    private node_trackers: { [s: string] : NodeListenerTracker }

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

    constructor(config: any, group_path: string, app_server: AppSocketServer) {
        // Create our firebase app.
        this.firebase_app = firebase.initializeApp(config, group_path);
        this.app_server = app_server
        this.signed_in = false
        this.node_trackers = {} // the key is the data_path, the value is 
        console.log('firebase app initialized with config: ' + JSON.stringify(config))
    }

    destroy() {
        this.firebase_app.delete()
        this.signed_in = false
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

        // Add our next listener under the node path.
        this.add_listener(req.args.node_path, req.args.data_path)

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

    add_listener(node_path: string, data_path: string) {
        let l = (data: firebase.database.DataSnapshot) => {
            let info: any = {}
            info.node_path = node_path
            info.data_path = data_path
            info.value = data.exportVal()
            this.app_server.send_msg(new InfoMessage(-1, "-1", InfoType.kFirebaseChanged, info))
        }

        // Make sure a tracker exists at node_path.
        if (!this.node_trackers.hasOwnProperty(node_path)) {
            this.node_trackers[node_path] = new NodeListenerTracker()
        }

        // Grab some references.
        let tracker = this.node_trackers[node_path]
        let full_data_path = this.get_full_data_path(data_path)

        // Attach the listener to the database location.
        this.firebase_app.database().ref(full_data_path).on('value', l)

        // Add the listener to the node tracker.
        tracker.add_listener(full_data_path, l)
    }

    remove_listener(node_path: string, data_path: string): void {
        // If there is no tracker for the node_path then return.
        if (!this.node_trackers.hasOwnProperty(node_path)) {
            return
        }

        // Grab some references.
        let tracker = this.node_trackers[node_path]
        let full_data_path = this.get_full_data_path(data_path)

        // Detach the listener from the firebase location.
        let l = tracker.get_listener(full_data_path)
        this.firebase_app.database().ref(full_data_path).off('value', l)

        // Remove the listener from the node tracker.
        tracker.remove_listener(full_data_path)
    }

}


export class FirebaseWraps {
    private app_server: AppSocketServer
    private request_ids: Array<RequestType>
    private wraps: any

    constructor(app_server: AppSocketServer) {
        this.app_server = app_server
        this.request_ids = [
            RequestType.kFirebaseInit,
            RequestType.kFirebaseDestroy,
            RequestType.kFirebaseSignIn,
            RequestType.kFirebaseSignOut,
            RequestType.kFirebaseWriteData,
            RequestType.kFirebaseReadData,
            RequestType.kFirebaseSubscribe,
            RequestType.kFirebaseUnsubscribe]
        this.wraps = {}
    }

    can_handle_request(type: RequestType): boolean {
        if (this.request_ids.indexOf(type) >= 0) {
            return true
        }
        return false
    }

    private get_group_path(node_path: string) {
        let splits = node_path.split('/')
        splits.pop()
        return splits.join('/')
    }

    // Creates a firebase wrap and caches it under its key for easy access at a later time.
    private create_firebase_wrap(group_path: string, config: any) {
        this.wraps[group_path] = new FirebaseWrap(config, group_path, this.app_server)
    }

    private firebase_wrap_exists(group_path: string): boolean {
        if (this.wraps.hasOwnProperty(group_path)) {
            return true
        }
        return true
    }

    private get_firebase_wrap(group_path: string): FirebaseWrap {
        if (this.firebase_wrap_exists(group_path)) {
            return this.wraps[group_path]
        }
        return undefined
    }

    private firebase_app_exists(group_path: string): boolean {
        for (let i = 0; i < firebase.apps.length; i++) {
            if (firebase.apps[i].name == group_path) {
                return true
            }
        }
        return false
    }

    private get_firebase_app(group_path: string): firebase.app.App {
        for (let i = 0; i < firebase.apps.length; i++) {
            if (firebase.apps[i].name == group_path) {
                return firebase.apps[i]
            }
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

        // Handle creating and destroying the firebase wrapper.
        if (req.request == RequestType.kFirebaseInit) {
            let config: any = {}
            config.apiKey = req.args.apiKey
            config.authDomain = req.args.authDomain
            config.databaseURL = req.args.databaseURL
            config.storageBucket = req.args.storageBucket
            config.email = req.args.email
            config.password = req.args.password
            console.log('setting current firebase config: ' + JSON.stringify(config))

            // Whether we really need to create a wrap.
            let need_to_create = true

            // Does the firebase wrapper already exist with the right config.
            if (this.firebase_app_exists(group_path) && this.firebase_wrap_exists(group_path)) {
                let app = this.get_firebase_app(group_path)
                let c = app.options
                if (c == config) {
                    need_to_create = false
                }
            }

            // Create the wrapper.
            if (!need_to_create) {
                // If we don't need to create a wrapper, then we're done.
                // Send back a response.
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
                return true
            } else {
                // Try to find an existing app under the group_path.
                let existing_app: firebase.app.App = undefined
                for (let i = 0; i < firebase.apps.length; i++) {
                    if (firebase.apps[i].name == group_path) {
                        existing_app = firebase.apps[i]
                    }
                }
                // If there is an existing firebase app at the group_path, we need to destroy it.
                if (existing_app) {
                    existing_app.delete().then(
                        (a: any) => {
                            // Now we just need to create a wrapper.
                            this.create_firebase_wrap(group_path, config)
                            // And we're done.
                            this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
                            return true
                        },
                        (error: Error) => {
                            console.log('got error: ' + error.message)
                            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
                            return true
                        })
                } else {
                    // Otherwise we just create a wrapper.
                    this.create_firebase_wrap(group_path, config)
                    // Send back a response.
                    this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
                    return true
                }
            }
        } else if (req.request == RequestType.kFirebaseDestroy) {
            console.log('firebase destroy called')
            // Try to find an existing app under the group_path.
            let existing_app: firebase.app.App = undefined
            for (let i = 0; i < firebase.apps.length; i++) {
                if (firebase.apps[i].name == group_path) {
                    existing_app = firebase.apps[i]
                }
            }
            // If there is an existing firebase app at the group_path, we need to destroy it.
            if (existing_app) {
                existing_app.delete() // Note we don't wait for the promise.
            }
            // Send back a response.
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', true, true))
            return true
        }

        // Otherwise we let the existing firebase wrap try to handle it.
        let wrap = this.get_firebase_wrap(group_path)
        if (wrap) {
            console.log('wrap is not null')
        } else {
            console.log('wrap is null')
        }
        return wrap.handle_request(req)
    }
}