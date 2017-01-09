
class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.

    // Members used to collected info from our frames.
    private collected_elems: IElementInfo[] = []
    private collected_booleans: boolean[] = []
    private collected_clicks: IClickInfo[] = []
    private collected_drop_downs: IDropDownInfo[] = []


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
    collect_drop_down(drop_down: IDropDownInfo) {
        if (drop_down == undefined) {
            return
        }
        this.collected_drop_downs.push(drop_down)
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Initiate collection from all frames.
    // ------------------------------------------------------------------------------------------------------------------

    collect_from_frames(msg: BaseMessage, response_collector: (response: any) => void = function(){}) {
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_current_tab_id()}, (details: chrome.webNavigation.GetAllFrameResultDetails[]) => {
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
                chrome.tabs.sendMessage(this.bg_comm.get_current_tab_id(), msg, { frameId: frame.frameId }, collector_wrap);
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

    queue_collect_drop_down_from_frames(msg: BaseMessage) {
        this.queue(() => {
            this.collected_drop_downs.length = 0
            this.collect_from_frames(msg, (drop_down: IDropDownInfo) => {this.collect_drop_down(drop_down)})
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
                if ((elem.box.left == best.box.left) && (elem.box.top == best.box.top) && (elem.z_index > best.z_index)) {
                    best = elem
                } else if (elem.box.top < best.box.top) {
                    best = elem
                } else if ((elem.box.top == best.box.top) && (elem.box.left < best.box.left)) {
                    best = elem
                }
            }
        })
        return best
    }

    static get_array_from_frame_index_path(path: string) {
        let arr: number[] = []
        let splits = path.split('/')
        for (let i=0; i<splits.length; i++) {
            // Note when empty strings are split on '/', you get an array with one element which is an empty string.
            if (splits[i] === '') {
                continue
            }
            // Get the frame index as a number.
            arr.push(Number(splits[i]))
        }
        return arr
    }

    // This method is supposed to find the topmost frame, in a set of overlapping frames.
    // This will take some time to implement so for now it simply looks at the frame index path_length
    // and choose the shortest path, or if there are multiple paths with the same length then it
    // chooses the first one it encounters.
    // The other issue that needs to be investigated is that we seem to be getting frames that aren't
    // really visible and often they would be clipped by their parent iframes but our logic currently
    // doesn't take this into account.
    static find_top_iframe(elems: IClickInfo[]): IClickInfo {
        let best: IClickInfo = null
        let best_path_length: number = null
        elems.forEach((elem) => {
            let path_arr = BgCommHandler.get_array_from_frame_index_path(elem.frame_index_path)
            let path_length = path_arr.length
            if (best === null) {
                best = elem
                best_path_length = path_length
            } else {
                if (path_length < best_path_length) {
                    best = elem
                    best_path_length = path_length
                }
            }
        })
        return best
    }

    static find_closest_neighbor(src: IElementInfo, candidates: IElementInfo[], angle_in_degrees: number) {

        console.log('angle in degress: ' + angle_in_degrees)

        // Loop through each one trying to find the best one.
        let best: IElementInfo = null
        let best_distance = 0
        const off_axis_weight = 2 // This weight seems to work best. This makes our magnitube be the hamiltonian distance.

        let src_box = new Box(src.box)
        let src_center = src_box.get_center()

        let theta = angle_in_degrees / 180.0 * Math.PI
        let dir = new Point({x: Math.cos(theta), y: -1 * Math.sin(theta)}) // -1 is because y increases from top to bottom.
        let perp_dir = new Point({x: -dir.y, y: dir.x})

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((dest) => {
            // Skip the dest element if its equal to the src element.
            if ((dest.frame_index_path == src.frame_index_path) && (dest.xpath == src.xpath)) {
                return
            }

            // Determine the direction and magnitude.
            let dest_box = new Box(dest.box)
            let dest_center:Point = dest_box.get_center()
            let diff = dest_center.subtract(src_center)
            

            // Now choose the closest element on one side, according to the direction in the request.
            let parallel_dist = (diff.x * dir.x) + (diff.y * dir.y)
            let perp_dist = (diff.x * perp_dir.x) + (diff.y * perp_dir.y)
            let distance = parallel_dist + off_axis_weight * Math.abs(perp_dist)  //Math.pow(perp_dist,2)

            // We want to skip elements that are in the opposite direction and that are overlaid directly on top of us.
            // For example an image when hovered over may load in a video on top.
            if (parallel_dist < 5) {
                return
            }

            // Update best if the dest is closer to the src element.
            if (!best) {
                best = dest
                best_distance = distance
            } else if (Math.abs(distance - best_distance) < 0.0001) {
                // If the distance and best_distance are very close check their z_index values or their sizes.
                // We choose the one with larger z_index or smaller size.
                if (dest.z_index && best.z_index && (dest.z_index > best.z_index)) {
                    best = dest
                    best_distance = distance
                } else {
                    let dest_area = (dest.box.right - dest.box.left) * (dest.box.bottom - dest.box.top)
                    let best_area = (best.box.right - best.box.left) * (best.box.bottom - best.box.top)
                    if (dest_area < best_area) {
                        best = dest
                        best_distance = distance
                    }
                }
            } else if (distance < best_distance) {
                best = dest
                best_distance = distance
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
    }

    queue_unblock_events() {
        let req = new RequestMessage(-1, RequestType.kUnblockEvents)
        this.queue_collect_void_from_frames(req)
    }

    // Note this method modifies the stack while executing.
    // It continually pushes a task to query the frames about whether they are busy.
    queue_wait_until_loaded(callback: ()=>void) {
        let req = new RequestMessage(-1, RequestType.kWaitUntilLoaded)
        this.queue_collect_bool_from_frames(req)
        this.queue(() => {
            if (this.collected_booleans.length == 0) {
                clearInterval(this.timer)
                this.timer = null
                callback()
            } else if (this.timer == null) {
                this.timer = setInterval(() => { this.queue_wait_until_loaded(callback); this.run_next_task() }, 1000)
            }
        })
    }

    // Element mutation methods.
    queue_update_element() {
        let req = new RequestMessage(-1, RequestType.kUpdateElement)
        this.queue_collect_void_from_frames(req)
    }

    queue_clear_element() {
        let req = new RequestMessage(-1, RequestType.kClearElement)
        this.queue_collect_void_from_frames(req)
    }

    // Result will be in this.found_elem. Error will be in this.error_msg.
    queue_get_current_element() {
        this.found_elem = null
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kGetElement)
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
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

    queue_scroll_element_into_view() {
        this.found_elem = null
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kScrollElementIntoView)
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
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
            } else if (this.collected_elems.length == 0) {
                this.error_msg = "Unable to find the element to make current."
            } else {
                this.error_msg = "There were multiple elements matching the make current request."
            }
            this.run_next_task()
        })
    }

    queue_find_all_elements_by_type(wrap_type: WrapType) {
        this.found_elems = []
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kFindElementByType, { wrap_type: wrap_type })
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    queue_find_all_elements_by_values(wrap_type: WrapType, target_values: string[]) {
        this.found_elems = []
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kFindElementByValues, { wrap_type: wrap_type, target_values: target_values })
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
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
                BrowserWrap.get_zoom(this.bg_comm.get_current_tab_id(), done_get_zoom);
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
            case RequestType.kUpdateCurrentTab: {
                this.clear_tasks()
                this.bg_comm.update_current_tab_id()
                let response = new ResponseMessage(req.id, true, true)
                this.bg_comm.send_to_nodejs(response)
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
            case RequestType.kHasElement: {
                this.clear_tasks()
                this.queue_get_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                    	// We don't error out if there is no current element.
                    	// We just return a dummy info instead.
                        let dummy: IElementInfo = {
                            frame_index_path: "-1",
                            xpath: "",
                            box: {left: 0, right: 0, bottom: 0, top: 0},
                            z_index: 0
                        }
                        let response = new ResponseMessage(req.id, true, dummy)
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
            case RequestType.kScrollElementIntoView: {
                this.clear_tasks()
                this.queue_scroll_element_into_view()
                this.queue(() => {
                    // Note if there is no current element, there is nothing to scroll, but we always return success/true.
                    let response = new ResponseMessage(req.id, true, {})
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kFindElementByPosition: {
                this.clear_tasks()
                this.queue_find_all_elements_by_type(req.args.wrap_type)
                this.queue(() => {
                    // Loop through all the found elements of the given wrap type.
                    let best: IElementInfo = null
                    let best_area = 0
                    let best_path_length = 0
                    this.collected_elems.forEach((info: IElementInfo) => {
                        let box = new Box(info.box)
                        let point = new Point(req.args.global_mouse_position)
                        let path_length = info.frame_index_path.split('/').length
                        let area = box.get_area()
                        // Examine elements which contain our point of interest.
                        if (box.contains_point(point)) {
                            if (best == null) {
                                // The first element is automatically the best.
                                best = info
                                best_area = box.get_area()
                                best_path_length = path_length
                            } else if (path_length < best_path_length) {
                                // If the candidate's frame index path length is smaller, then it is now the best.
                                best = info
                                best_area = box.get_area()
                                best_path_length = path_length
                            } else if (path_length == best_path_length) {
                                // Otherwise if the candidate has a smaller area, then it is now the best.
                                if (area < best_area) {
                                    best = info
                                    best_area = area
                                    best_path_length = path_length
                                }
                            }
                        }
                    });
                    this.found_elem = JSON.parse(JSON.stringify(best))

                    if (this.found_elem != null) {
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
            case RequestType.kFindElementByValues: {
                this.clear_tasks()
                this.queue_find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                this.queue(() => {
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
                        let best: IElementInfo = BgCommHandler.find_closest_neighbor(this.found_elem, this.found_elems, req.args.angle_in_degrees)
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
                        let best: IElementInfo = BgCommHandler.find_closest_neighbor(this.found_elem, this.found_elems, req.args.angle_in_degrees)
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
                    if (this.collected_elems.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_elems[0])
                        this.bg_comm.send_to_nodejs(response)
                    } else if (this.collected_elems.length > 1) {
                        let response = new ResponseMessage(req.id, false, "Performed element action on multiple elements: " + this.collected_elems.length)
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
                    if (this.collected_clicks.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_clicks[0])
                        this.bg_comm.send_to_nodejs(response)
                    } else if (this.collected_clicks.length > 1) {
                        let best = BgCommHandler.find_top_iframe(this.collected_clicks)
                        let response = new ResponseMessage(req.id, true, best)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "The crosshair click point did not intersect any elements.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kGetElementValues: {
                this.clear_tasks()
                this.queue_collect_click_from_frames(req)
                this.queue(() => {
                    if (this.collected_clicks.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_clicks[0])
                        this.bg_comm.send_to_nodejs(response)
                    } else if (this.collected_clicks.length > 1) {
                        let best = BgCommHandler.find_top_iframe(this.collected_clicks)
                        let response = new ResponseMessage(req.id, true, best)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "The element's center point did not intersect any elements.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.run_next_task()
            } break
            case RequestType.kGetDropDownInfo: {
                this.clear_tasks()
                this.queue_collect_drop_down_from_frames(req)
                this.queue(() => {
                    if (this.collected_drop_downs.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_drop_downs[0])
                        this.bg_comm.send_to_nodejs(response)
                    } else if (this.collected_drop_downs.length > 1) {
                        let response = new ResponseMessage(req.id, false, "Received drop down info from multiple elements: " + this.collected_drop_downs.length)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "Did not receive any drop down info from any elements.")
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