
class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.
    private frame_infos: FrameInfos = new FrameInfos() 

    // Members used to collected info from our frames.
    private collected_elems: IElementInfo[] = []
    private collected_booleans: boolean[] = []
    private collected_clicks: IClickInfo[] = []
    private collected_drop_downs: IDropDownInfo[] = []

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
    // Frame Window and Frame Element Index Path Info Distribution.
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
                if (info) {
                    let offset = info.calculate_offset()
                    let fe_index_path = info.calculate_fe_index_path()
                    //console.log('distributing frame index path: ' + info.fw_index_path + ' element index path: ' + fe_index_path)
                    let msg = new InfoMessage(-1, InfoType.kDistributeFrameOffsets, {fe_index_path: fe_index_path, offset: offset})
                    chrome.tabs.sendMessage(this.bg_comm.get_current_tab_id(), msg, { frameId: frame.frameId }, collector);
                }
            });
        }
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_current_tab_id()}, f);
    }

    // ------------------------------------------------------------------------------------------------------------------
    // Specialized method to collect from our frames use by various methods below.
    // ------------------------------------------------------------------------------------------------------------------
    
    collect_from_frames(msg: BaseMessage, response_collector: (response: any) => void = function(){}) {
        chrome.webNavigation.getAllFrames({tabId: this.bg_comm.get_current_tab_id()}, (details: chrome.webNavigation.GetAllFrameResultDetails[]) => {
            // Initialize our response counters, so that we know when all responses have come back.
            this.response_count = 0
            this.expected_response_count = details.length
            console.log('expecting num responses: ' + this.expected_response_count)
            // Wrap the given response collector in logic to increment the response counters.
            let collector_wrap = (response: any) => {
                response_collector(response)
                this.response_count += 1
                console.log('got response count: ' + this.response_count)
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

    // ------------------------------------------------------------------------------------------------------------------
    // Base Task Queue methods.
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
    // Base Collection/Distribution Task Queue Methods.
    // ------------------------------------------------------------------------------------------------------------------

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
    // Specialized Task Queue Methods.
    // ------------------------------------------------------------------------------------------------------------------

    // Note this method modifies the stack while executing.
    // It continually pushes a task to query the frames about whether they are busy.
    queue_wait_until_loaded(callback: ()=>void) {
        console.log('waiting for frames to finish loading')
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
                    let response = new ResponseMessage(req.id, true, {value: true})
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.clear_all_cookies(done_clear_all_cookies)
            } break
            case ChromeRequestType.kGetAllCookies: {
                let done_get_all_cookies = (cookies: chrome.cookies.Cookie[]) => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true, {value: cookies})
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
                        let response = new ResponseMessage(req.id, true, {value: true})
                        this.bg_comm.send_to_app(response)
                    }
                }
                BrowserWrap.set_all_cookies(cookies, done_set_all_cookies)
            } break
            case ChromeRequestType.kGetZoom: {
                let done_get_zoom = (zoom: number) => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true, { value: zoom })
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.get_zoom(this.bg_comm.get_current_tab_id(), done_get_zoom);
            } break
            case ChromeRequestType.kSetZoom: {
                let done_set_zoom = () => {
                    // Send response to app.
                    let response = new ResponseMessage(req.id, true, {value: true})
                    this.bg_comm.send_to_app(response)
                }
                BrowserWrap.set_zoom(this.bg_comm.get_current_tab_id(), req.args.zoom, done_set_zoom);
            } break
            case ChromeRequestType.kUpdateCurrentTab: {
                this.clear_tasks()
                this.bg_comm.update_current_tab_id()
                let response = new ResponseMessage(req.id, true, {value: true})
                this.bg_comm.send_to_app(response)
            } break
            case ChromeRequestType.kOpenTab: {
                //let response = new ResponseMessage(req.id, true, {value: true})
                //this.bg_comm.send_to_app(response)
                chrome.tabs.create({'url': req.args.url}, 
                    (tab) => {
                        // Tab opened successfully.
                        let response = new ResponseMessage(req.id, true, {value: true})
                        this.bg_comm.send_to_app(response)
                    });
            } break
            case ChromeRequestType.kGetActiveTabTitle: {
                chrome.tabs.query({ active: true },
                    (tabs) => {
                        if (tabs.length > 1) {
                            console.log('Error: More than 1 tab is active. Choosing the first one.')
                        }
                        let response = new ResponseMessage(req.id, true, {value: tabs[0].title})
                        this.bg_comm.send_to_app(response)
                    });
            } break

            // --------------------------------------------------------------
            // Requests that are broadcast to all frames.
            // --------------------------------------------------------------
            case ChromeRequestType.kWaitUntilLoaded: {
                this.clear_tasks()
                this.queue_wait_until_loaded(() => {
                    let response = new ResponseMessage(req.id, true, {value: true})
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kUpdateFrameOffsets: {
                this.clear_tasks()
                this.queue_frame_info_sharing()
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, {value: true})
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kGetAllElements: {
                this.clear_tasks()
                this.queue_collect_elements_from_frames(req)
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, {elements: this.collected_elems})
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            case ChromeRequestType.kBlockEvents:
            case ChromeRequestType.kUnblockEvents:
            case ChromeRequestType.kHighlightElements:
            case ChromeRequestType.kUpdateElementHighlights: 
            case ChromeRequestType.kClearElementHighlights:
            case ChromeRequestType.kScrollElementIntoView: 
            case ChromeRequestType.kScrollElement: 
            case ChromeRequestType.kReleaseBrowser: {
                this.clear_tasks()
                this.queue_collect_void_from_frames(req)
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, {value: true})
                    this.bg_comm.send_to_app(response)
                })
                this.run_next_task()
            } break
            default: {
                console.error("BgCommHandler got an unknown request.")
            } break
        }
    }
}
