/// <reference path="D:\installs\srcdeps\ngsexternal\nodejs2\node_modules\firebase\firebase.d.ts"/>

import firebase = require("firebase");

import {AppSocketServer} from './commhub'

export class FirebaseWrap {

    private firebase_app: firebase.app.App
    private app_server: AppSocketServer
    private signed_in: boolean

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
            case RequestType.kFirebaseListenToChanges: {
                this.listen_for_changes(req)
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
        this.firebase_app.auth().signInWithEmailAndPassword(req.args.email, req.args.password).then(
            (a: any) => {
                this.signed_in = true
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true))
            },
            (error: Error) => {
                // The user account may not exist yet if this is the first time trying to sign in.
                // So we try to create an account.
                this.firebase_app.auth().createUserWithEmailAndPassword(req.args.email, req.args.password).then(
                    (a: any) => {
                        this.app_server.send_msg(new ResponseMessage(req.id, '-1', true))
                    },
                    (error: Error) => {
                        this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
                    }
                )
            } 
        )
    }

    // Sign out.
    sign_out(req: RequestMessage): void {
        this.firebase_app.auth().signOut().then(
            () => {
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true))
            }, 
            (error: Error) => {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, error.message))
        });
    }

    // Write data.
    write_data(req: RequestMessage): void {
        if (!this.signed_in) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, "Error: Not signed into firebase. Check surrounding firebase group settings."))
            return
        }
        let currentUser = this.firebase_app.auth().currentUser
        let userId = currentUser.uid
        let path = 'users/' + userId;
        if (req.args.path.length) {
            if (req.args.path.charAt(0) == '/') {
                path = path + req.args.path
            } else {
                path = path + '/' + req.args.path
            }
        }
        this.firebase_app.database().ref(path).set(req.args.value).then(
            (a: any) => {
                this.app_server.send_msg(new ResponseMessage(req.id, '-1', true))
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
        let currentUser = this.firebase_app.auth().currentUser
        let userId = currentUser.uid
        let path = 'users/' + userId;
        if (req.args.path.length) {
            if (req.args.path.charAt(0) == '/') {
                path = path + req.args.path
            } else {
                path = path + '/' + req.args.path
            }
        }
        this.firebase_app.database().ref(path).once('value').then(
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
    listen_for_changes(req: RequestMessage): void {
        if (!this.signed_in) {
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', false, "Error: Not signed into firebase. Check surrounding firebase group settings."))
            return
        }
        let currentUser = this.firebase_app.auth().currentUser
        let userId = currentUser.uid
        let path = 'users/' + userId;
        if (req.args.path.length) {
            if (req.args.path.charAt(0) == '/') {
                path = path + req.args.path
            } else {
                path = path + '/' + req.args.path
            }
        }
        this.firebase_app.database().ref(path).on('value', (data: firebase.database.DataSnapshot) => {
            this.app_server.send_msg(new InfoMessage(req.id, "-1", InfoType.kFirebaseChanged, data.exportVal()))
        })
        this.app_server.send_msg(new ResponseMessage(req.id, '-1', true))
    }

}


export class FirebaseWraps {
    private wraps: any
    private app_server: AppSocketServer
    private current_config: any

    constructor(app_server: AppSocketServer) {
        this.app_server = app_server
        this.clear()
    }

    // Clear our existing wraps.
    private clear() {
        this.wraps = {}
    }

    private set_current_config(firebase_config: any) {
        this.current_config = firebase_config
    }

    // The key which represents the firebase config.
    private create_key(config: any) {
        // We form the key by adding the following fields of the config into a giant string.
        //     apiKey: "AIzaSyCXGNlyRf5uk8Xk1bvKXUcA53TC6Lc3I-A",
        //     authDomain: "test-project-91c10.firebaseapp.com",
        //     databaseURL: "https://test-project-91c10.firebaseio.com/",
        //     storageBucket: "gs://test-project-91c10.appspot.com",
        let key = config.apiKey + config.authDomain + config.databaseURL + config.storageBucket
        return key
    }

    // Creates a firebase wrap and caches it under its key for easy access at a later time.
    private create_wrap(config: any) {
        let key = this.create_key(config)
        let wrap = new FirebaseWrap(config, key, this.app_server)
        this.wraps[key] = wrap
    }

    // Checks to see if a wrap exists for certain config.
    private wrap_exists(config: any) {
        let key = this.create_key(config)
        if (this.wraps.hasOwnProperty(key)) {
            return true
        }
        return false
    }

    // Attempts to get a firebase wrap. If it doesn't exist, it will try to create one.
    private get_wrap(config: any) {
        let key = this.create_key(config)
        if (this.wrap_exists(config)) {
            return this.wraps[key]
        }
        // Otherwise create a wrap.
        this.create_wrap(config)
        return this.wraps[key]
    }

    handle_request(req: RequestMessage): boolean {
        if (req.request == RequestType.kFirebaseInit) {
            let config: any = {}
            config.apiKey = req.args.apiKey
            config.authDomain = req.args.authDomain
            config.databaseURL = req.args.databaseURL
            config.storageBucket = req.args.storageBucket
            console.log('setting current firebase config: ' + JSON.stringify(config))
            // Make sure the wrap is created.
            this.get_wrap(config)
            // Record the current config.
            this.set_current_config(config)
            // Send back a response.
            this.app_server.send_msg(new ResponseMessage(req.id, '-1', true))
            return true
        }

        // Otherwise we let the firebase base try to handle it.
        let wrap = this.get_wrap(this.current_config)
        return wrap.handle_request(req)
    }
}