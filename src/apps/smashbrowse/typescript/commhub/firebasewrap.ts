/// <reference path="D:\installs\srcdeps\ngsexternal\nodejs2\node_modules\firebase\firebase.d.ts"/>

import firebase = require("firebase");


export class FirebaseWrap {

    constructor() {
        var config = {
            apiKey: "AIzaSyCXGNlyRf5uk8Xk1bvKXUcA53TC6Lc3I-A",
            authDomain: " test-project-91c10.firebaseapp.com",
            databaseURL: "https://test-project-91c10.firebaseio.com/",
            storageBucket: "gs://test-project-91c10.appspot.com",
        };
        
        firebase.initializeApp(config); 
    }

    // User is automatically signed in if the account was created successfully.
    create_account(email: string, password: string): void {
        firebase.auth().createUserWithEmailAndPassword(email, password).catch(
            function(error: Error) {
                // Handle Errors here.
                let errorCode = error.name
                let errorMessage = error.message;
                let stack = error.stack 
            });
    }

    // Sign into account.
    sign_in(email: string, password: string): void {
        firebase.auth().signInWithEmailAndPassword(email, password).catch(
            function(error: Error) {
                // Handle Errors here.
                let errorCode = error.name
                let errorMessage = error.message;
                let stack = error.stack 
            });
    }

    // Sign out.
    sign_out(): void {
        firebase.auth().signOut().then(function() {
            // Sign-out successful.
        }, function(error) {
            // An error happened.
        });
    }

    // Write data.
    write_data(data: any): void {
        let userId = firebase.auth().currentUser.uid
        firebase.database().ref('users/' + userId).set(data);
    }

    // Read data.
    // Eg onResolve: function(snapshot: any) {console.log(snapshot.val());}
    // Eg onReject: function(error: Error) {console.log(error.name)}
    read_data(onResolve: (a: any) => any, onReject?: (a: Error) => any): any {
        var userId = firebase.auth().currentUser.uid;
        return firebase.database().ref('/users/' + userId).once('value').then(onResolve, onReject)
    }

    // Listen to changes.
    // Eg callback: function(snapshot) {console.log(snapshot.val());}
    listen_for_changes(path: string, callback: (a: firebase.database.DataSnapshot, b?: string) => any): void {
        let db_ref = firebase.database().ref(path)
        db_ref.on('value', callback)
    }

}