import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtWebView 1.1

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0


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

    // Recorded Events.
    property var events: []
    property var script: ""

    function convert_key_identifier(id) {
        if (!is_modifier(id)) {
            return "'" + id.replace('U+','\\u') + "'"
        }
        return "Key." + id.toUpperCase()
    }

    function is_modifier(id) {
        if ((id.indexOf('U+') != 0)) {
            return true
        }
        return false
    }

    function on_recording_received(r) {
        script = r
    }

    function send_result() {
        return "send_result()\n"
    }

    function get_wait(xpath) {
        return "wait_for_element('"+xpath+"')\n"
    }

    function element_wrap(command, xpath) {
        return "driver.findElements(By.xpath('" + xpath + "')).then(function(elements){ if (elements.length) { " + command + "}})\n"
    }

    function try_catch_wrap(command, index) {
        return "try {" + command + "}catch(e){dump('caught exception at command " + index + ": ' + e.message + ' stacktrace: ' + e.stack)}\n"
    }

    function replay() {
        // Convert the events to a webdriver script.
        //var building_key_seq = false
        var script = "function (env) {\n"
        script += "ws = env.ws\n"
        script += "webdriver = env.webdriver\n"
        script += "driver = env.driver\n"
        script += "Key = env.Key\n"
        script += "By = env.By\n"
        script += "until = env.until\n"
        script += "flow = env.flow\n"
        script += "resize_browser = env.resize_browser\n"
        script += "dump = env.dump\n"
        script += "wait_for_element = env.wait_for_element\n"
        script += "click_on_element = env.click_on_element\n"
        script += "mouse_over_element = env.mouse_over_element\n"
        script += "send_result = env.send_result\n"
        script += "\n"
        script += "resize_browser(" + width + ", " + height + ")\n"

        for (var i=0; i<events.length; i++) {
            var event = events[i]
            var type = event.type
            if (type == 'goto_url') {
                script += "driver.get('" + event.url + "')\n"
            } else if (type == 'click') {
                //script += "wait_for_element('" + event.target + "')\n"
                script += "click_on_element('" + event.target +"')\n"
            } else if ((type == 'keydown')) {
                // Handling enter as a special case.
                if (event.keyIdentifier == 'Enter') {
                    //script += "wait_for_element('" + event.target + "')\n"
                    script += "driver.findElement(By.xpath('" + event.target + "')).sendKeys(" + convert_key_identifier(event.keyIdentifier) + ")\n"
                }
            } else if (type == 'input') {
                // On input we overwrite the entire field.
                //script += "wait_for_element('" + event.target + "')\n"
                script += "driver.findElement(By.xpath('" + event.target + "')).sendKeys(Key.HOME,Key.chord(Key.SHIFT,Key.END),'" + event.text + "')\n"
            } else if (type == 'mousemove') {
                //script += "wait_for_element('" + event.target + "')\n"
                script += "mouse_over_element('" + event.target + "', " + event.elementX + ", " + event.elementY + ")\n"
            } else if (type == 'scroll') {
                script += "driver.executeScript('window.scrollBy("+ event.scrollX +","+ event.scrollY +")')\n"
            } else {
            }
        }
        script += "//return our results\n"
        script += send_result()
        script += "}\n"
        app_comm.push_recording(script)
    }

    function inject_jquery(next_func) {
        var on_jquery_loaded = app_settings.try_catch_wrap(function (result) {
            console.log("jquery loaded")
            next_func()
        })

        var on_jquery_checked = app_settings.try_catch_wrap(function (result) {
            if (String(result) == "false") {
                console.log("jquery not loaded .. trying to inject")
                web_view.runJavaScript(scripts.get_jquery() + scripts.get_jquery_simulate(),on_jquery_loaded)
            } else {
                next_func()
            }
        })

        var script = "var result = true
                      if(!window.jQuery) {
                        result = false
                      }
                      result"
        web_view.runJavaScript(script, on_jquery_checked)
    }

    function inject_octoplier(next_func) {
        var on_octoplier_loaded = app_settings.try_catch_wrap(function (result) {
            console.log("octoplier loaded")
            next_func()
        })

        var on_octoplier_checked = app_settings.try_catch_wrap(function (result) {
            //console.log("is octoplier installed:--" + result + "--")
            if (String(result) == "false") {
                console.log("octoplier not loaded .. trying to inject")
                web_view.runJavaScript(scripts.get_event_recorder_script(),on_octoplier_loaded)
            } else {
                //console.log("wwwwwwwwwwwwweird")
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
                var returned = JSON.parse(result);
                for (var i=0; i<returned.length; i++) {
                    web_view.events.push(returned[i])
                    console.log("storing event: " + JSON.stringify(events[i]))
                }
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

//    // Timer to pulse our web recorder.
//    Timer {
//        id: timer
//        interval: 50
//        running: false
//        repeat: true
//        onTriggered: {
//            app_settings.try_catch_wrap(function() {
//                web_view.inject_jquery(function() {
//                    web_view.inject_octoplier(function() {
//                        web_view.connect_octoplier(function() {
//                             web_view.retrieve_events(function() {
//                                 web_view.retrieve_errors(web_view.empty_func)
//                            })
//                        })
//                    })
//                })
//            })()
//        }
//    }

    onLoadingChanged: {
        if (loadRequest.status == WebView.LoadStartedStatus) {
            console.log("page load has started")
        } else if (loadRequest.status == WebView.LoadStoppedStatus) {
            console.log("page load has stopped")
        } else if (loadRequest.status == WebView.LoadSucceededStatus) {
            console.log("page load has succeeded")
        } else if (loadRequest.status == WebView.LoadFailedStatus) {
            console.log("page load has failed")
        } else {
            console.log("got an unknown load status")
        }
    }
}
