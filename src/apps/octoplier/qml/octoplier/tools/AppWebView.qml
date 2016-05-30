import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtWebView 1.1

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0


WebView {
    id: web_view
    objectName: "web_view_object"

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Initial url.
    url: "http://www.google.com"

    // The recording property determines whether the connect or disconnect is called on octoplier when pulsing the browser.
    property bool recording: false

    // The replaying property determines whether replay is called on octoplier when pulsing the browser.
    property bool replaying: false
    property int event_index: 0 // The index currently being replayed.
    property bool single_replay_done: true // Whether a single replay event has finished.

    // Recorded Events.
    property var events: []

    function replay(next_func) {
        // Check if the last replay element has finished.
        if (! single_replay_done) {
            next_func()
            return
        }

        // Check if we're done.
        if (event_index >= events.length) {
            web_view.replaying = false
            next_func()
            return
        }

        // Check for our fake url event.
        if (web_view.events[event_index].type == "goto_url") {
            web_view.url = web_view.events[event_index].url
            web_view.event_index = web_view.event_index + 1
            next_func()
            return
        }

        var on_played = app_settings.try_catch_wrap(function (result) {
            if (String(result) == "true") {
                console.log("single replay event finished successfully ---------------------------")
                web_view.event_index = web_view.event_index + 1
            } else if (String(result) == "false") {
                console.log("single replay event FAILED ---------------------------")
            }
            web_view.single_replay_done = true
            next_func()
        })

        console.log("event string: " + JSON.stringify(web_view.events[event_index]))
        var script = "var result = false
                      try {
                          if (octoplier.event_proxy.replay_event(" + JSON.stringify(web_view.events[event_index]) + ")) {
                            result = true
                          }
                      } catch (e) {
                        octoplier.logger.log_exception(e)
                        result = false
                      }
                      result
                      "
        //console.log("running script: " + script)
        web_view.single_replay_done = false
        web_view.runJavaScript(script, on_played)
    }

    function start_replay() {
        event_index = 0
        web_view.replaying = true
    }

    function inject_octoplier(next_func) {
        var on_octoplier_loaded = app_settings.try_catch_wrap(function (result) {
            console.log("octoplier loaded")
            next_func()
        })

        var on_octoplier_checked = app_settings.try_catch_wrap(function (result) {
            console.log("is octoplier installed:--" + result + "--")
            if (String(result) == "false") {
                console.log("octoplier not loading .. trying to inject")
                web_view.runJavaScript(scripts.get_event_recorder_script(),on_octoplier_loaded)
            } else {
                console.log("wwwwwwwwwwwwweird")
                next_func()
            }
        })

        var script = "  var result = true
                        if (typeof(octoplier) === \"undefined\") {
                            result = false
                        }
                        result"
        web_view.runJavaScript(script, on_octoplier_checked)
    }

    function connect_octoplier(next_func) {
        var on_done = app_settings.try_catch_wrap(function (result) {
            next_func()
        })
        if (web_view.recording) {
            web_view.runJavaScript(scripts.get_start_recording_script(), on_done)
        } else {
            web_view.runJavaScript(scripts.get_stop_recording_script(), on_done)
        }
    }

    function retrieve_events(next_func) {
        var on_events_retrieved = app_settings.try_catch_wrap(function (result) {
            if (result != "") {
                var events = JSON.parse(result);
                for (var i=0; i<events.length; i++) {                    
                    var last = web_view.events[web_view.events.length-1]
                    if ((events[i].type == "input") && (last.type == "input") &&
                            (events[i].target == last.target)) {
                        web_view.events[web_view.events.length-1] = events[i]
                    } else {
                        web_view.events.push(events[i])
                    }
                    console.log("storing event: " + JSON.stringify(events[i]))
                }
                //console.log("dequeuing:" + result)
            }
            next_func()
        })
        // Grab the events.
        web_view.runJavaScript(scripts.get_event_retriever_script(), on_events_retrieved)
    }

    function retrieve_errors(next_func) {
        var on_errors_retrieved = app_settings.try_catch_wrap(function (result) {
            if (result != "[]") {
                console.log("debug messages:" + result)
            }
            next_func()
        })
        // Dequeue any debug messages that have built up.
        web_view.runJavaScript(scripts.get_error_retriever_script(), on_errors_retrieved)
    }

    function empty_func() {
        //console.log("empty func")
    }

    // Timer to pulse our web recorder.
    Timer {
        id: timer
        interval: 50
        running: web_browser_page.visible
        repeat: true
        onTriggered: {
            app_settings.try_catch_wrap(function() {
                if (!web_view.replaying) {
                    web_view.inject_octoplier(function() {
                        web_view.connect_octoplier(function() {
                             web_view.retrieve_events(function() {
                                 web_view.retrieve_errors(web_view.empty_func)
                            })
                        })
                    })
                } else {
                    web_view.inject_octoplier(function () {
                        web_view.replay(function() {
                            web_view.retrieve_errors(web_view.empty_func)
                        })
                    })
                }
            })()
        }
    }

    onLoadingChanged: {
        if (loadRequest.status == WebView.LoadStartedStatus) {
            //message_dialog.show(qsTr("page load has started"))
        } else if (loadRequest.status == WebView.LoadStoppedStatus) {
            //message_dialog.show(qsTr("page load has stopped"))
        } else if (loadRequest.status == WebView.LoadSucceededStatus) {
            // message_dialog.show(qsTr("page load has succeeded"))
        } else if (loadRequest.status == WebView.LoadFailedStatus) {
            //message_dialog.show(qsTr("page load has failed"))
        } else {
           // message_dialog.show(qsTr("got an unknown load status"))
        }
    }
}
