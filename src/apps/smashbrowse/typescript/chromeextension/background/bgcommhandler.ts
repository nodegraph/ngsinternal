
class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.
    private frame_infos: FrameInfos

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
        this.frame_infos = new FrameInfos() 
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

    collect_fw_index_paths() {
        let f = (details: chrome.webNavigation.GetAllFrameResultDetails[]) => {

            // Initialize the frame hierarchy.
            this.frame_infos.initialize(details)

            // Initialize our response counters, so that we know when all responses have come back.
            this.response_count = 0
            this.expected_response_count = details.length
            
            console.log('expection num responses 111: ' + this.expected_response_count)

            // Wrap the given response collector in logic to increment the response counters.
            let make_collector = (frame_id: number) => {
                let collector_wrap = (response: string) => {
                    let info = this.frame_infos.get_info_by_id(frame_id)
                    if (!info) {
                        console.error("could not find frame info by id: " + frame_id)
                    } else {
                    	console.log("got fw path 111: " + response)
                        info.fw_index_path = response
                    }
                    this.response_count += 1
                    if (this.response_count == this.expected_response_count) {
                        //this.frame_infos.dump_parenting()
                        //this.frame_infos.dump_tree()
                        //this.frame_infos.dump_array()

                        console.log('done getting fw index paths')
                        this.run_next_task()
                    }
                }
                return collector_wrap
            }

            // Now send a message to each frame.
            details.forEach((frame: chrome.webNavigation.GetAllFrameResultDetails) => {
                let collector = make_collector(frame.frameId)
                let msg = new InfoMessage(-1, InfoType.kCollectFrameIndexPaths, {frame_id: frame.frameId})
                chrome.tabs.sendMessage(this.bg_comm.get_current_tab_id(), msg, { frameId: frame.frameId }, collector);
            });
        }
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_current_tab_id()}, f);
    }
    
    // This assumes collect_fw_index_paths has already been run.
    collect_frame_offsets() {
        let f = (details: chrome.webNavigation.GetAllFrameResultDetails[]) => {

            // Initialize our response counters, so that we know when all responses have come back.
            this.response_count = 0
            this.expected_response_count = details.length
            
            console.log('expection num responses 222: ' + this.expected_response_count)

            // Wrap the given response collector in logic to increment the response counters.
            let make_collector = (frame_id: number) => {
                let collector_wrap = (response: {fw_index_path: string, offsets: { fw_index_path: string, element_index: number, bounds: IBox }[]}) => {
                    console.log('got response from: ' + response.fw_index_path)
                    for (let r of response.offsets) {
                        let info = this.frame_infos.get_info_by_fw_index_path(r.fw_index_path)
                        if (!info) {
                            console.error("could not find frame info by index path: " + r.fw_index_path)
                            console.log('bounds: ' + JSON.stringify(r.bounds))
                            continue
                        } else {
                            console.log("got fw path 222: " + r.fw_index_path)
                            console.log('bounds: ' + JSON.stringify(r.bounds))
                        }

                        info.element_index = r.element_index
                        info.bounds = r.bounds
                    }

                    this.response_count += 1
                    if (this.response_count == this.expected_response_count) {
                        console.log('done getting frame offsets')
                        this.run_next_task()
                    }
                }
                return collector_wrap
            }

            // Now send a message to each frame.
            details.forEach((frame: chrome.webNavigation.GetAllFrameResultDetails) => {
                let collector = make_collector(frame.frameId)
                let msg = new InfoMessage(-1, InfoType.kCollectFrameOffsets, {frame_id: frame.frameId})
                chrome.tabs.sendMessage(this.bg_comm.get_current_tab_id(), msg, { frameId: frame.frameId }, collector);
            });
        }
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_current_tab_id()}, f);
    }

    distribute_frame_offsets() {
        let f = (details: chrome.webNavigation.GetAllFrameResultDetails[]) => {

            // Initialize our response counters, so that we know when all responses have come back.
            this.response_count = 0
            this.expected_response_count = details.length

            // Wrap the given response collector in logic to increment the response counters.
            let make_collector = (frame_id: number) => {
                let collector_wrap = (response: any) => {
                    // We don't do anything.
                    this.response_count += 1
                    if (this.response_count == this.expected_response_count) {
                        console.log('done distributing frame offsets')
                        this.run_next_task()
                    }
                }
                return collector_wrap
            }

            // Now send a message to each frame.
            details.forEach((frame: chrome.webNavigation.GetAllFrameResultDetails) => {
                let collector = make_collector(frame.frameId)
                let info = this.frame_infos.get_info_by_id(frame.frameId)
                let offset = info.calculate_offset()
                let fe_index_path = info.calculate_fe_index_path()
                //console.log('distributing frame index path: ' + info.fw_index_path + ' element index path: ' + fe_index_path)
                let msg = new InfoMessage(-1, InfoType.kDistributeFrameOffsets, {fe_index_path: fe_index_path, offset: offset})
                chrome.tabs.sendMessage(this.bg_comm.get_current_tab_id(), msg, { frameId: frame.frameId }, collector);
            });
        }
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_current_tab_id()}, f);
    }
    
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

    queue_frame_info_sharing() {
        this.queue(() => {this.collect_fw_index_paths()})
        this.queue(() => {this.collect_frame_offsets()})
        this.queue(() => {this.collect_frame_offsets()})
        this.queue(() => {this.distribute_frame_offsets()})
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

    // This method is supposed to find the topmost frame, in a set of overlapping frames.
    // This will take some time to implement so for now it simply looks at the frame index path_length
    // and choose the shortest path, or if there are multiple paths with the same length then it
    // chooses the first one it encounters.
    // The other issue that needs to be investigated is that we seem to be getting frames that aren't
    // really visible and often they would be clipped by their parent frames but our logic currently
    // doesn't take this into account.
    static find_top_frame(elems: IClickInfo[]): IClickInfo {
        let best: IClickInfo = null
        let best_path_length: number = null
        elems.forEach((elem) => {
            let path_arr = get_array_from_index_path(elem.fw_index_path)
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

    static find_next_along_rows(src: IElementInfo, candidates: IElementInfo[], max_width_diff: number, max_height_diff: number, max_angle_diff: number) {
        let next: IElementInfo = BgCommHandler.find_closest_neighbor(src, candidates, 0, max_width_diff, max_height_diff, max_angle_diff);
        if (next) {
            return next
        }

        // Otherwise we need to find the first element of rows below the current row.
        let best: IElementInfo = null

        let src_box = new Box(src.box)
        const src_bottom = src_box.bottom

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((dest) => {
            let dest_box = new Box(dest.box)
            
            // Determine if the candidate is of the right size.
            let dest_width = dest_box.get_width()
            let dest_height = dest_box.get_height()
            let min_width_pixels = src_box.get_width() * (1.0 - (max_width_diff * 0.01))
            let max_width_pixels = src_box.get_width() * (1.0 + (max_width_diff * 0.01))
            let min_height_pixels = src_box.get_height() * (1.0 - (max_height_diff * 0.01));
            let max_height_pixels = src_box.get_height() * (1.0 + (max_height_diff * 0.01));
            if ((dest_width < min_width_pixels) || (dest_width > max_width_pixels)) {
                return
            }
            if ((dest_height < min_height_pixels) || (dest_height > max_height_pixels)) {
                return
            }

            // Make sure the dest element is below our current row.
            if (dest.box.top < src_bottom) {
                return
            }

            // If best hasn't been set yet, we take the first dest element.
            if (!best) {
                best = dest
                return
            }

            // If the dest element is more top-left than our current best, it becomes the next best element.
            if (dest.box.top <= best.box.top) {
                if (dest.box.left <= best.box.left) {
                    best = dest
                    return
                }
            }
        })
        return best
    }

    static find_closest_neighbor(src: IElementInfo, candidates: IElementInfo[], angle: number, max_width_diff: number, max_height_diff: number, max_angle_diff: number) {

        // Loop through each one trying to find the best one.
        let best: IElementInfo = null
        let best_distance = 0

        let src_box = new Box(src.box)
        let src_center = src_box.get_center()

        let theta = angle / 180.0 * Math.PI
        let dir = new Point({x: Math.cos(theta), y: -1 * Math.sin(theta)}) // -1 is because y increases from top to bottom.
        let perp_dir = new Point({x: -dir.y, y: dir.x})

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((dest) => {
            // Skip the dest element if its equal to the src element.
            if ((dest.fw_index_path == src.fw_index_path) && (dest.xpath == src.xpath)) {
                return
            }

            // Determine the candidates center point information.
            let dest_box = new Box(dest.box)
            let dest_center:Point = dest_box.get_center()
            let diff = dest_center.subtract(src_center)
            
            // Determine if the candidate is of the right size.
            let dest_width = dest_box.get_width()
            let dest_height = dest_box.get_height()
            let min_width_pixels = src_box.get_width() * (1.0 - (max_width_diff * 0.01))
            let max_width_pixels = src_box.get_width() * (1.0 + (max_width_diff * 0.01))
            let min_height_pixels = src_box.get_height() * (1.0 - (max_height_diff * 0.01));
            let max_height_pixels = src_box.get_height() * (1.0 + (max_height_diff * 0.01));
            if ((dest_width < min_width_pixels) || (dest_width > max_width_pixels)) {
                return
            }
            if ((dest_height < min_height_pixels) || (dest_height > max_height_pixels)) {
                return
            }

            // Determine the angle difference from the desired angle.
            // First determine the x and y in the tilted frame of the desired angle.
            let parallel_dist = (diff.x * dir.x) + (diff.y * dir.y)
            let perp_dist = (diff.x * perp_dir.x) + (diff.y * perp_dir.y)
            // Next determine the angle in radians.
            let theta = Math.atan2(perp_dist, parallel_dist);
            let angle = -1 * theta / 3.141592653 * 180.0; // -1 is because y increases from top to bottom, and we want users to think that 0 degress is to the right, and 90 degress is up.
            if (Math.abs(angle) > Math.abs(max_angle_diff)) {
                return
            }

            // Determine the distance.
            let distance = (diff.x*diff.x) + (diff.y * diff.y)

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
        let req = new RequestMessage(-1, ChromeRequestType.kBlockEvents)
        this.queue_collect_void_from_frames(req)
    }

    queue_unblock_events() {
        let req = new RequestMessage(-1, ChromeRequestType.kUnblockEvents)
        this.queue_collect_void_from_frames(req)
    }

    // Note this method modifies the stack while executing.
    // It continually pushes a task to query the frames about whether they are busy.
    queue_wait_until_loaded(callback: ()=>void) {
        let req = new RequestMessage(-1, ChromeRequestType.kWaitUntilLoaded)
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
        let req = new RequestMessage(-1, ChromeRequestType.kUpdateElement)
        this.queue_collect_void_from_frames(req)
    }

    queue_clear_element() {
        let req = new RequestMessage(-1, ChromeRequestType.kClearElement)
        this.queue_collect_void_from_frames(req)
    }

    // Result will be in this.found_elem. Error will be in this.error_msg.
    queue_get_current_element() {
        this.found_elem = null
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, ChromeRequestType.kGetElement)
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
        let req = new RequestMessage(-1, ChromeRequestType.kScrollElementIntoView)
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
            let req = new RequestMessage(-1, ChromeRequestType.kSetElement, this.found_elem)
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
        let req = new RequestMessage(-1, ChromeRequestType.kFindElementByType, { wrap_type: wrap_type })
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    queue_find_all_elements_by_values(wrap_type: WrapType, target_values: string[]) {
        this.found_elems = []
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, ChromeRequestType.kFindElementByValues, { wrap_type: wrap_type, target_values: target_values })
        this.queue_collect_elements_from_frames(req)
        this.queue(() => {
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Main message handler.
    // ------------------------------------------------------------------------------------------------------------------
    handle_app_request(req: RequestMessage) {
        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.

        switch (req.request) {

            // --------------------------------------------------------------
            // Requests that are handled by the background script on its own.
            // --------------------------------------------------------------

            case ChromeRequestType.kClearAllCookies: {
                let done_clear_all_cookies = () => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true)
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.clear_all_cookies(done_clear_all_cookies)
            } break
            case ChromeRequestType.kGetAllCookies: {
                let done_get_all_cookies = (cookies: chrome.cookies.Cookie[]) => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true, cookies)
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.get_all_cookies(done_get_all_cookies);
            } break
            case ChromeRequestType.kSetAllCookies: {
                let cookies = req.args.cookies
                let count = 0
                let done_set_all_cookies = () => {
                    count += 1
                    if (count == cookies.length) {
                        // Send response to app.
                        let response = new ResponseMessage(req.id, true)
                        this.bg_comm.send_to_app(response)
                    }
                }
                BrowserWrap.set_all_cookies(cookies, done_set_all_cookies)
            } break
            case ChromeRequestType.kGetZoom: {
                let done_get_zoom = (zoom: number) => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true, { 'zoom': zoom })
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.get_zoom(this.bg_comm.get_current_tab_id(), done_get_zoom);
            } break
            case ChromeRequestType.kSetZoom: {
                let done_set_zoom = () => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.set_zoom(this.bg_comm.get_current_tab_id(), req.args.zoom, done_set_zoom);
            } break
            case ChromeRequestType.kUpdateCurrentTab: {
                this.clear_tasks()
                this.bg_comm.update_current_tab_id()
                let response = new ResponseMessage(req.id, true, true)
                this.bg_comm.send_to_app(response)
            } break
            case ChromeRequestType.kOpenTab: {
                //let response = new ResponseMessage(req.id, true, true)
                //this.bg_comm.send_to_app(response)
                chrome.tabs.create({'url': 'http://www.google.com'}, 
                    (tab) => {
                        // Tab opened successfully.
                        let response = new ResponseMessage(req.id, true, true)
                        this.bg_comm.send_to_app(response)
                    });
            } break
            case ChromeRequestType.kGetActiveTabTitle: {
                chrome.tabs.query({ active: true },
                    (tabs) => {
                        if (tabs.length > 1) {
                            console.log('Error: More than 1 tab is active. Choosing the first one.')
                        }
                        let response = new ResponseMessage(req.id, true, tabs[0].title)
                        this.bg_comm.send_to_app(response)
                    });
            } break

            // --------------------------------------------------------------
            // Requests that are broadcast to all frames.
            // --------------------------------------------------------------

            case ChromeRequestType.kBlockEvents: {
                this.clear_tasks()
                this.queue_block_events()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kUnblockEvents: {
                this.clear_tasks()
                this.queue_unblock_events()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kWaitUntilLoaded: {
                this.clear_tasks()
                this.queue_wait_until_loaded(() => {
                        let response = new ResponseMessage(req.id, true, true)
                        this.bg_comm.send_to_app(response)
                    })
                this.run_next_task()
            } break
            case ChromeRequestType.kUpdateFrameOffsets: {
                this.clear_tasks()
                this.queue_frame_info_sharing()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kUpdateElement: {
                this.clear_tasks()
                this.queue_update_element()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kClearElement: {
                this.clear_tasks()
                this.queue_clear_element()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kGetElement: {
                this.clear_tasks()
                this.queue_get_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kHasElement: {
                this.clear_tasks()
                this.queue_get_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_app(response)
                    } else {
                    	// We don't error out if there is no current element.
                    	// We just return a dummy info instead.
                        let dummy: IElementInfo = {
                            fw_index_path: "-1",
                            fe_index_path: "-1",
                            xpath: "",
                            href: "",
                            box: {left: 0, right: 0, bottom: 0, top: 0},
                            z_index: 0
                        }
                        let response = new ResponseMessage(req.id, true, dummy)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kSetElement: {
                this.clear_tasks()
                this.found_elem = req.args
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kScrollElementIntoView: {
                this.clear_tasks()
                this.queue_scroll_element_into_view()
                this.queue(() => {
                    // Note if there is no current element, there is nothing to scroll, but we always return success/true.
                    let response = new ResponseMessage(req.id, true, {})
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kFindElementByPosition: {
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
                        let path_length = info.fw_index_path.split('/').length
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
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kFindElementByValues: {
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
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kFindElementByType: {
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
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.queue_set_current_element()
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kShiftElementByType: {
                this.clear_tasks()
                // Make sure we have a current element to shift from.
                this.queue_get_current_element()
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    } else {
                        this.run_next_task()
                    }
                })
                // Now get all the possible elements that we can shift to.
                this.queue_find_all_elements_by_type(req.args.wrap_type)
                // Select one to shift to.
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Unable to find any candidate elements to shift to.
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let best: IElementInfo = BgCommHandler.find_closest_neighbor(this.found_elem, this.found_elems, req.args.angle, req.args.max_width_difference, req.args.max_height_difference, req.args.max_angle_difference)
                        if (!best) {
                            // Unable to find the next element.
                            // Wipe out the queue.
                            this.clear_tasks()
                            let response = new ResponseMessage(req.id, true, false)
                            this.bg_comm.send_to_app(response)
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
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kShiftElementByValues: {
                this.clear_tasks()
                // Make sure we have a current element to shift from.
                this.queue_get_current_element()
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    } else {
                        this.run_next_task()
                    }
                })
                // Now get all the possible elements that we can shift to.
                this.queue_find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                // Select one to shift to.
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Unable to find any candidate elements to shift to.
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let best: IElementInfo = BgCommHandler.find_closest_neighbor(this.found_elem, this.found_elems, req.args.angle, req.args.max_width_difference, req.args.max_height_difference, req.args.max_angle_difference)
                        if (!best) {
                            // Wipe out the queue.
                            // Unable to find the next element.
                            this.clear_tasks()
                            let response = new ResponseMessage(req.id, true, false)
                            this.bg_comm.send_to_app(response)
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
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kShiftElementByTypeAlongRows: {
                this.clear_tasks()
                // Make sure we have a current element to shift from.
                this.queue_get_current_element()
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    } else {
                        this.run_next_task()
                    }
                })
                // Now get all the possible elements that we can shift to.
                this.queue_find_all_elements_by_type(req.args.wrap_type)
                // Select one to shift to.
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Unable to find any candidate elements to shift to.
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let best: IElementInfo = BgCommHandler.find_next_along_rows(this.found_elem, this.found_elems, req.args.max_width_difference, req.args.max_height_difference, req.args.max_angle_difference)
                        if (!best) {
                            // Unable to find the next element.
                            // Wipe out the queue.
                            this.clear_tasks()
                            let response = new ResponseMessage(req.id, true, false)
                            this.bg_comm.send_to_app(response)
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
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kShiftElementByValuesAlongRows: {
                this.clear_tasks()
                // Make sure we have a current element to shift from.
                this.queue_get_current_element()
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_app(response)
                    } else {
                        this.run_next_task()
                    }
                })
                // Now get all the possible elements that we can shift to.
                this.queue_find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                // Select one to shift to.
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Unable to find any candidate elements to shift to.
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let best: IElementInfo = BgCommHandler.find_next_along_rows(this.found_elem, this.found_elems, req.args.max_width_difference, req.args.max_height_difference, req.args.max_angle_difference)
                        if (!best) {
                            // Unable to find the next element.
                            // Wipe out the queue.
                            this.clear_tasks()
                            let response = new ResponseMessage(req.id, true, false)
                            this.bg_comm.send_to_app(response)
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
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, true, false)
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            // case ChromeRequestType.kPerformElementAction: {
            //     this.clear_tasks()
            //     this.queue_collect_elements_from_frames(req)
            //     this.queue(() => {
            //         if (this.collected_elems.length == 1) {
            //             let response = new ResponseMessage(req.id, true, this.collected_elems[0])
            //             this.bg_comm.send_to_app(response)
            //         } else if (this.collected_elems.length > 1) {
            //             let response = new ResponseMessage(req.id, false, "Performed element action on multiple elements: " + this.collected_elems.length)
            //             this.bg_comm.send_to_app(response)
            //         } else {
            //             let response = new ResponseMessage(req.id, false, "No current element to perform element action on.")
            //             this.bg_comm.send_to_app(response)
            //         }
            //     })
            //     this.run_next_task()
            // } break
            case ChromeRequestType.kGetCrosshairInfo: {
                this.clear_tasks()
                this.queue_collect_click_from_frames(req)
                this.queue(() => {
                    if (this.collected_clicks.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_clicks[0])
                        this.bg_comm.send_to_app(response)
                    } else if (this.collected_clicks.length > 1) {
                        let best = BgCommHandler.find_top_frame(this.collected_clicks)
                        let response = new ResponseMessage(req.id, true, best)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "The crosshair click point did not intersect any elements.")
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kGetElementValues: {
                this.clear_tasks()
                this.queue_collect_click_from_frames(req)
                this.queue(() => {
                    if (this.collected_clicks.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_clicks[0])
                        this.bg_comm.send_to_app(response)
                    } else if (this.collected_clicks.length > 1) {
                        let best = BgCommHandler.find_top_frame(this.collected_clicks)
                        let response = new ResponseMessage(req.id, true, best)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "The element's center point did not intersect any elements.")
                        this.bg_comm.send_to_app(response)
                    }
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kGetDropDownInfo: {
                this.clear_tasks()
                this.queue_collect_drop_down_from_frames(req)
                this.queue(() => {
                    if (this.collected_drop_downs.length == 1) {
                        let response = new ResponseMessage(req.id, true, this.collected_drop_downs[0])
                        this.bg_comm.send_to_app(response)
                    } else if (this.collected_drop_downs.length > 1) {
                        let response = new ResponseMessage(req.id, false, "Received drop down info from multiple elements: " + this.collected_drop_downs.length)
                        this.bg_comm.send_to_app(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, "Did not receive any drop down info from any elements.")
                        this.bg_comm.send_to_app(response)
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