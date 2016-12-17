
class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.

    // Used when searching for iframes.
    private collected_elems: IElementInfo[] = []
    private collected_booleans: boolean[] = []
    private collected_clicks: IClickInfo[] = []


    // Caches used in our calculations.
    private found_elem: IElementInfo = null
    private found_elems: IElementInfo[] = []
    private error_msg: string = ""

    // Timer.
    private timer: number

    // Task Queue.
    private tasks: (() => void)[] = []

    private response_count: number = 0
    private expected_response_count: number = 0

    // Constructor.
    constructor(bc: BgComm, bw: BrowserWrap) {
        this.bg_comm = bc
        this.browser_wrap = bw
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Collector Methods.
    // ------------------------------------------------------------------------------------------------------------------
    collect_elements(elems: IElementInfo[]) {
        if (elems == undefined) {
            return
        }
        this.collected_elems = this.collected_elems.concat(elems);
    }
    collect_boolean(b: boolean) {
        if (b == undefined) {
            return
        }
        this.collected_booleans.push(b)
    }
    collect_click(click: IClickInfo) {
        if (click == undefined) {
            return
        }
        this.collected_clicks.push(click)
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Initiate collection from all frames.
    // ------------------------------------------------------------------------------------------------------------------

    collect_from_frames(msg: BaseMessage, response_collector: (response: any) => void = function(){}) {
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_tab_id()}, (details: chrome.webNavigation.GetAllFrameResultDetails[]) => {
            // Initialize our response counters, so that we know when all responses have come back.
            this.response_count = 0
            this.expected_response_count = details.length
            // Wrap the given response collector in logic to increment the response counters.
            let collector_wrap = (response: any) => {
                response_collector(response)
                this.response_count += 1
                if (this.response_count == this.expected_response_count) {
                    this.run_next_task()
                }
            }
            // Now send a message to each frame.
            details.forEach((frame: chrome.webNavigation.GetAllFrameResultDetails) => {
                chrome.tabs.sendMessage(this.bg_comm.get_tab_id(), msg, { frameId: frame.frameId }, collector_wrap);
            });
        });
    }

    queue_collect_void_from_frames(msg: BaseMessage) {
        this.queue(() =>{
            this.collect_from_frames(msg, (b: any) => {})
        })
    }

    queue_collect_bool_from_frames(msg: BaseMessage) {
        this.queue(() =>{
            this.collected_booleans.length = 0
            this.collect_from_frames(msg, (b: boolean) => {this.collect_boolean(b)})
        })
    }

    queue_collect_elements_from_frames(msg: BaseMessage) {
        this.queue(() => {
            this.collected_elems.length = 0
            this.collect_from_frames(msg, (elems: IElementInfo[]) => {this.collect_elements(elems)})
        })
        
    }

    queue_collect_click_from_frames(msg: BaseMessage) {
        this.queue(() => {
            this.collected_clicks.length = 0
            this.collect_from_frames(msg, (click_info: IClickInfo) => {this.collect_click(click_info)})
        })
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Static element methods.
    // ------------------------------------------------------------------------------------------------------------------

    static find_top_leftmost(elems: IElementInfo[]): IElementInfo {
        let best: IElementInfo = null
        elems.forEach((elem) => {
            if (best === null) {
                best = elem
            } else {
                if ( (elem.box.left <= best.box.left) && (elem.box.top <= best.box.top) ) {
                    best = elem
                }
            }
        })
        return best
    }

    static find_neighbor_internal(origin: IElementInfo, candidates: IElementInfo[], dir: DirectionType) {
        // Loop through each one trying to find the best one.
        let best: IElementInfo = null
        console.log('elem: ' + JSON.stringify(origin))
        candidates.forEach((candidate) => {
            // Make sure the candidate intersect the origin element.
            if (!BgCommHandler.elements_in_row(candidate, origin) || !BgCommHandler.elements_in_column(candidate, origin)) {
                return
            }

            // Make sure the candidate is not the same element as the origin.
            if ( (candidate.iframe_index_path == origin.iframe_index_path) && (candidate.xpath == origin.xpath) ) {
                return
            }

            // Now analyze the candidate according to the search direction.
            switch (dir) {
                case DirectionType.left: {
                    if (candidate.box.left < origin.box.right) {
                        if (!best) {
                            best = candidate
                            console.log('found first: ' + JSON.stringify(best))
                        } else if (candidate.box.left > best.box.left) {
                            best = candidate
                            console.log('found better: ' + JSON.stringify(best))
                        }
                    }
                } break
                case DirectionType.right: {
                    if (candidate.box.right > origin.box.left) {
                        if (!best) {
                            best = candidate
                            console.log('found first: ' + JSON.stringify(best))
                        } else if (candidate.box.right < best.box.right) {
                            best = candidate
                            console.log('found better: ' + JSON.stringify(best))
                        }
                    }
                } break
                case DirectionType.down: {
                    if (candidate.box.bottom > origin.box.top) {
                        if (!best) {
                            best = candidate
                            console.log('found first: ' + JSON.stringify(best))
                        } else if (candidate.box.bottom < best.box.bottom) {
                            best = candidate
                            console.log('found better: ' + JSON.stringify(best))
                        }
                    }
                } break
                case DirectionType.up: {
                    if (candidate.box.top < origin.box.bottom) {
                        if (!best) {
                            best = candidate
                            console.log('found first: ' + JSON.stringify(best))
                        } else if (candidate.box.top > best.box.top) {
                            best = candidate
                            console.log('found better: ' + JSON.stringify(best))
                        }
                    }
                } break
            }
        })
        return best
    }

    static find_neighbor_external(origin: IElementInfo, candidates: IElementInfo[], dir: DirectionType) {
        // Loop through each one trying to find the best one.
        let best: IElementInfo = null
        console.log('elem: ' + JSON.stringify(origin))
        candidates.forEach((candidate) => {
            // Now choose the closest element on one side, according to the direction in the request.
            switch (dir) {
                case DirectionType.left: {
                    if (BgCommHandler.elements_in_row(candidate, origin)) {
                        if (candidate.box.right < origin.box.left) {
                            if (!best) {
                                best = candidate
                                console.log('found first: ' + JSON.stringify(best))
                            } else if (candidate.box.right > best.box.right) {
                                best = candidate
                                console.log('found better: ' + JSON.stringify(best))
                            }
                        }
                    }
                } break
                case DirectionType.right: {
                    if (BgCommHandler.elements_in_row(candidate, origin)) {
                        if (candidate.box.left > origin.box.right) {
                            if (!best) {
                                best = candidate
                                console.log('found first: ' + JSON.stringify(best))
                            } else if (candidate.box.left < best.box.left) {
                                best = candidate
                                console.log('found better: ' + JSON.stringify(best))
                            }
                        }
                    }
                } break
                case DirectionType.down: {
                    if (BgCommHandler.elements_in_column(candidate, origin)) {
                        if (candidate.box.top > origin.box.bottom) {
                            if (!best) {
                                best = candidate
                                console.log('found first: ' + JSON.stringify(best))
                            } else if (candidate.box.top < best.box.top) {
                                best = candidate
                                console.log('found better: ' + JSON.stringify(best))
                            }
                        }
                    }
                } break
                case DirectionType.up: {
                    if (BgCommHandler.elements_in_column(candidate, origin)) {
                        if (candidate.box.bottom < origin.box.top) {
                            if (!best) {
                                best = candidate
                                console.log('found first: ' + JSON.stringify(best))
                            } else if (candidate.box.bottom > best.box.bottom) {
                                best = candidate
                                console.log('found better: ' + JSON.stringify(best))
                            }
                        }
                    }
                } break
            }
        })
        return best
    }

    static find_neighbor_closest(src: IElementInfo, candidates: IElementInfo[], dir: DirectionType) {
        // Loop through each one trying to find the best one.
        let best: IElementInfo = null
        let best_mag = 0
        const off_axis_weight = 1 // This weight seems to work best. This makes our magnitube be the hamiltonian distance.

        console.log('elem: ' + JSON.stringify(src))
        let src_box = new Box(src.box)
        let src_center = src_box.get_center()

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((dest) => {
            // Skip the dest element if its equal to the src element.
            if ((dest.iframe_index_path == src.iframe_index_path) && (dest.xpath == src.xpath)) {
                return
            }

            // Determine the direction and magnitude.
            let dest_box = new Box(dest.box)
            let dest_center:Point = dest_box.get_center()
            let diff = dest_center.subtract(src_center)
            let mag = 0

            // Now choose the closest element on one side, according to the direction in the request.
            switch (dir) {
                case DirectionType.left: {
                    if (diff.x >= 0) {
                        return
                    }
                    mag = Math.abs(diff.x) + Math.abs(diff.y) * off_axis_weight
                } break
                case DirectionType.right: {
                    if (diff.x <= 0) {
                        return
                    }
                    mag = Math.abs(diff.x) + Math.abs(diff.y) * off_axis_weight
                } break
                case DirectionType.down: {
                    if (diff.y <= 0) {
                        return
                    }
                    mag = Math.abs(diff.y) + Math.abs(diff.x) * off_axis_weight
                } break
                case DirectionType.up: {
                    if (diff.y >= 0) {
                        return
                    }
                    mag = Math.abs(diff.y) + Math.abs(diff.x) * off_axis_weight
                } break
            }

            // Update best if the dest is closer to the src element.
            if (!best) {
                best = dest
                best_mag = mag
                console.log('found first: ' + JSON.stringify(best))
            } else if (mag < best_mag) {
                best = dest
                best_mag = mag
                console.log('found better: ' + JSON.stringify(best))
            }
        })
        return best
    }

    static elements_in_row(e1: IElementInfo, e2: IElementInfo) {
        return (e1.box.top <= e2.box.bottom) && (e2.box.top <= e1.box.bottom)
    }

    static elements_in_column(e1: IElementInfo, e2: IElementInfo) {
        return (e1.box.left <= e2.box.right) && (e2.box.left <= e1.box.right)
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Task queue methods.
    // ------------------------------------------------------------------------------------------------------------------

    // Functor Queue.
    queue(task: () => void) {
        this.tasks.push(task)
    }

    queue_debug_msg(msg: string) {
        this.queue(()=> {
            console.log(msg)
            this.run_next_task()
        })
    }

    clear_tasks() {
        this.tasks.length = 0
    }

    run_next_task() {
        if (this.tasks.length == 0) {
            return
        }
        let t = this.tasks.shift()
        t()
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Helper methods.
    // ------------------------------------------------------------------------------------------------------------------

    // Other.
    queue_block_events() {
        let req = new RequestMessage(-1, RequestType.kBlockEvents)
        this.queue_collect_void_from_frames(req)
        this.queue_debug_msg("done blocking events")
    }

    queue_unblock_events() {
        let req = new RequestMessage(-1, RequestType.kUnblockEvents)
        this.queue_collect_void_from_frames(req)
        this.queue_debug_msg("done unblocking events")
    }

    // Note this method modifies the stack while executing.
    // It continually pushes a task to query the frames about whether they are busy.
    queue_wait_until_loaded(callback: ()=>void) {
        let req = new RequestMessage(-1, RequestType.kWaitUntilLoaded)
        this.queue_collect_bool_from_frames(req)
        this.queue(() => {
            if (this.collected_booleans.length == 0) {
                console.log('finished waiting until loaded')
                clearInterval(this.timer)
                this.timer = null
                callback()
            } else if (this.timer == null) {
                console.log('requeuing wait until loaded')
                this.timer = setInterval(() => { this.queue_wait_until_loaded(callback); this.run_next_task() }, 1000)
            }
        })
    }

    // Element mutation methods.
    queue_update_element() {
        let req = new RequestMessage(-1, RequestType.kUpdateElement)
        this.queue_collect_void_from_frames(req)
        this.queue_debug_msg('finished update element')
    }

    queue_clear_element() {
        let req = new RequestMessage(-1, RequestType.kClearElement)
        this.queue_collect_void_from_frames(req)
        this.queue_debug_msg('finished clear element')
    }

    // Result will be in this.found_elem. Error will be in this.error_msg.
    queue_get_current_element() {
        this.found_elem = null
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kGetElement)
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
            console.log('finished get current element')
            if (this.collected_elems.length == 1) {
                // Make a deep copy of the current element info.
                this.found_elem = JSON.parse(JSON.stringify(this.collected_elems[0]))
            } else if (this.collected_elems.length == 0) {
                this.error_msg = "Unable to find the current element."
            } else {
                this.error_msg = "There were multiple current elements."
            }
            this.run_next_task()
        })
    }

    // Result will be in this.found_elem. Error will be in this.error_msg.
    // Note this uses the dynamic value of this.found_elem to set the current element.
    queue_set_current_element() {
        this.error_msg = ""
        this.collected_elems.length = 0
        
        // We skip using queue_collect_elements_from_frames, because we need to dynamically create the message
        // using the current value of this.found_elem.
        this.queue(() => {
            let req = new RequestMessage(-1, RequestType.kSetElement, this.found_elem)
            this.collected_elems.length = 0
            this.collect_from_frames(req, (elems: IElementInfo[]) => {this.collect_elements(elems)})
        })
        this.queue(() => {
            if (this.collected_elems.length == 1) {
                // Make a deep copy of the current element info.
                this.found_elem = JSON.parse(JSON.stringify(this.collected_elems[0]))
                console.log('finished set current element: ' + JSON.stringify(this.found_elem))
            } else if (this.collected_elems.length == 0) {
                this.error_msg = "Unable to find the element to make current."
            } else {
                this.error_msg = "There were multiple elements matching the make current request."
            }
            this.run_next_task()
        })
    }

    // Result will be in this.found_elems. Error will be in this.error_msg.
    queue_find_all_elements_by_type(wrap_type: WrapType) {
        this.found_elems = []
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kFindElementByType, { wrap_type: wrap_type })
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
            console.log("queue_find_all_elements_by_type")
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    // Result will be in this.found_elems. Error will be in this.error_msg.
    queue_find_all_elements_by_values(wrap_type: WrapType, target_values: string[]) {
        this.found_elems = []
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kFindElementByValues, { wrap_type: wrap_type, target_values: target_values })
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
            console.log("find_all_elements_by_values")
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Main message handler.
    // ------------------------------------------------------------------------------------------------------------------
    handle_nodejs_request(req: RequestMessage) {
        //console.log('handling request from nodejs: ' + JSON.stringify(req))

        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.

        switch (req.request) {

            // --------------------------------------------------------------
            // Requests that are handled by the background script on its own.
            // --------------------------------------------------------------

            case RequestType.kClearAllCookies: {
                let done_clear_all_cookies = () => {
                    // Send response to nodejs.
                    let response = new ResponseMessage(req.id, true)
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.clear_all_cookies(done_clear_all_cookies)
            } break
            case RequestType.kGetAllCookies: {
                let done_get_all_cookies = (cookies: chrome.cookies.Cookie[]) => {
                    // Send response to nodejs.
                    let response = new ResponseMessage(req.id, true, cookies)
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.get_all_cookies(done_get_all_cookies);
            } break
            case RequestType.kSetAllCookies: {
                let cookies = req.args.cookies
                let count = 0
                let done_set_all_cookies = () => {
                    count += 1
                    if (count == cookies.length) {
                        // Send response to nodejs.
                        let response = new ResponseMessage(req.id, true)
                        this.bg_comm.send_to_nodejs(response)
                    }
                }
                BrowserWrap.set_all_cookies(cookies, done_set_all_cookies)
            } break
            case RequestType.kGetZoom: {
                let done_get_zoom = (zoom: number) => {
                    // Send response to nodejs.
                    let response = new ResponseMessage(req.id, true, { 'zoom': zoom })
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom);
            } break

            // --------------------------------------------------------------
            // Requests that are broadcast to all frames.
            // --------------------------------------------------------------

            case RequestType.kBlockEvents: {
                this.clear_tasks()
                this.queue_block_events()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kUnblockEvents: {
                this.clear_tasks()
                this.queue_unblock_events()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kWaitUntilLoaded: {
                this.clear_tasks()
                this.queue_wait_until_loaded(() => {
                        let response = new ResponseMessage(req.id, true, true)
                        this.bg_comm.send_to_nodejs(response)
                    })
                this.run_next_task()
            } break
            case RequestType.kUpdateElement: {
                this.clear_tasks()
                this.queue_update_element()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kClearElement: {
                this.clear_tasks()
                this.queue_clear_element()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kGetElement: {
                this.clear_tasks()
                this.queue_get_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kSetElement: {
                this.clear_tasks()
                this.found_elem = req.args
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kFindElementByValues: {
                this.clear_tasks()
                this.queue_find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                this.queue(() => {
                    console.log("find_all_elements_by_values 3333" + this.found_elems.length)
                    if (this.found_elems.length > 0) {
                        this.found_elem = BgCommHandler.find_top_leftmost(this.found_elems)
                        this.run_next_task()
                    } else {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements which match the given values.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kFindElementByType: {
                this.clear_tasks()
                this.queue_find_all_elements_by_type(req.args.wrap_type)
                this.queue(() => {
                    if (this.found_elems.length > 0) {
                        this.found_elem = BgCommHandler.find_top_leftmost(this.found_elems)
                        this.run_next_task()
                    } else {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements which match the given type.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kShiftElementByType: {
                this.clear_tasks()
                // Make sure we have a current element to shift from.
                this.queue_get_current_element()
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        console.log('shift from origin element: ' + JSON.stringify(this.found_elem))
                        this.run_next_task()
                    }
                })
                // Now get all the possible elements that we can shift to.
                this.queue_find_all_elements_by_type(req.args.wrap_type)
                // Select one to shift to.
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements to shift to.")
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let best: IElementInfo = null
                        best = BgCommHandler.find_neighbor_closest(this.found_elem, this.found_elems, req.args.direction)
                        // if (req.args.allow_internal_elements) {
                        //     best = BgCommHandler.find_neighbor_internal(this.found_elem, this.found_elems, req.args.direction)
                        // } else {
                        //     best = BgCommHandler.find_neighbor_external(this.found_elem, this.found_elems, req.args.direction)
                        // }
                        if (!best) {
                            // Wipe out the queue.
                            this.clear_tasks()
                            let response = new ResponseMessage(req.id, false, "Unable to find any elements to shift to in the specified direction.")
                            this.bg_comm.send_to_nodejs(response)
                        } else {
                            this.found_elem = best
                            this.run_next_task()
                        }
                    }
                })
                // Shift the element.
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        console.log('found element to shift to: ' + this.found_elem.iframe_index_path + this.found_elem.xpath)
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kShiftElementByValues: {
                this.clear_tasks()
                // Make sure we have a current element to shift from.
                this.queue_get_current_element()
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        this.run_next_task()
                    }
                })
                // Now get all the possible elements that we can shift to.
                this.queue_find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                // Select one to shift to.
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements with the given values to shift to.")
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let best: IElementInfo = null
                        best = BgCommHandler.find_neighbor_closest(this.found_elem, this.found_elems, req.args.direction)
                        // if (req.args.allow_internal_elements) {
                        //     best = BgCommHandler.find_neighbor_internal(this.found_elem, this.found_elems, req.args.direction)
                        // } else {
                        //     best = BgCommHandler.find_neighbor_external(this.found_elem, this.found_elems, req.args.direction)
                        // }
                        if (!best) {
                            // Wipe out the queue.
                            this.clear_tasks()
                            let response = new ResponseMessage(req.id, false, "Unable to find any elements with the given values in the specified direction.")
                            this.bg_comm.send_to_nodejs(response)
                        } else {
                            this.found_elem = best
                            this.run_next_task()
                        }
                    }
                })
                // Shift the element.
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kPerformElementAction: {
                this.clear_tasks()
                this.queue_collect_elements_from_frames(req)
                this.queue(() => {
                    console.log('finished performing element action 111')
                    if (this.collected_elems.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_elems[0])
                        this.bg_comm.send_to_nodejs(response)
                    } else if (this.collected_elems.length > 1) {
                        let response = new ResponseMessage(req.id, false, "Performed element action on multiple elements.")
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "No current element to perform element action on.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
        } break
            case RequestType.kGetCrosshairInfo: {
            this.clear_tasks()
            this.queue_collect_click_from_frames(req)
            this.queue(() => {
                console.log('finished getting crosshair info: num hits:' + this.collected_clicks.length)
                if (this.collected_clicks.length == 1) {
                    let response = new ResponseMessage(req.id, true, this.collected_clicks[0])
                    this.bg_comm.send_to_nodejs(response)
                } else if (this.collected_clicks.length > 1) {
                    let response = new ResponseMessage(req.id, false, "The crosshair click point intersected multiple elements.")
                    this.bg_comm.send_to_nodejs(response)
                } else {
                    let response = new ResponseMessage(req.id, false, "The crosshair click point did not intersect any elements.")
                    this.bg_comm.send_to_nodejs(response)
                }
            })
            this.run_next_task()
        } break
            default: {
                console.error("BgCommHandler got an unknown request.")
            } break
        }

    }
}