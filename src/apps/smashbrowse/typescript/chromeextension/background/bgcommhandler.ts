
class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.
    
    // Used when searching for iframes.
    private collected_elems: IElementInfo[] = []
    private collected_booleans: boolean[] = []
    private collected_clicks: ICrosshairInfo[] = []
    
    
    // Caches used in our calculations.
    private found_elem: IElementInfo = null
    private found_elems: IElementInfo[] = []
    private error_msg: string = ""
    
    // Timer.
    private timer: number

    // Task Queue.
    private tasks: (()=>void) [] = []

    // Constructor.
    constructor(bc: BgComm, bw: BrowserWrap) {
        this.bg_comm = bc
        this.browser_wrap = bw
    }

    // Collectors.
    collect_element(elem: IElementInfo) {
        this.collected_elems.push(elem)
    }
    collect_elements(elems: IElementInfo[]) {
        this.collected_elems = this.collected_elems.concat(elems);
        console.log('collected elems: ' + JSON.stringify(this.collected_elems))
    }
    collect_boolean(b: boolean) {
        this.collected_booleans.push(b)
    }
    collect_click(click: ICrosshairInfo) {
        this.collected_clicks.push(click)
    }

    // Element static methods.
    static sort_elements(elements: IElementInfo[]) {
        let max_page_width = 10000 // We assume this is the maximum width of all web pages.
        let max_iframe_width = 10000 // We assume this is the maximum width of all iframes.
        for (let i=0; i<elements.length; i++) {
            let info = elements[i]
            let box_width = info.box.right - info.box.left
            let box_height = info.box.bottom - info.box.top
            // The whole weight orders the elements by their top left coordinate.
            let whole_weight = (info.box.top * max_page_width) + info.box.left
            // The fraction weight orders elements with the same top coordinate in dereasing order of surface area.
            let fraction_weight = 1.0 - (box_height * box_width) / (max_iframe_width * max_iframe_width)
            // The whole weight and fraction weight are combined to create an absolute weighting.
            info.weight = whole_weight + fraction_weight
        }
        // Sort the weighted infos.
        elements.sort(function(a, b) {
            return a.weight - b.weight;
        });
    }

    static find_neighbor(elem: IElementInfo, candidates: IElementInfo[], dir: DirectionType) {
        // Loop through each one trying to find the best one.
        let best: IElementInfo = elem
        candidates.forEach((candidate) => {
            // Now choose the closest element on one side, according to the direction in the request.
            switch(dir) {
                case DirectionType.left: {
                    if (BgCommHandler.elements_in_row(candidate, elem)) {
                        if (candidate.weight < best.weight) {
                            best = candidate
                        }
                    }
                } break
                case DirectionType.right: {
                    if (BgCommHandler.elements_in_row(candidate, elem)) {
                        if (candidate.weight > best.weight) {
                            best = candidate
                        }
                    }
                } break
                case DirectionType.down: {
                    if (BgCommHandler.elements_in_column(candidate, elem)) {
                        if (candidate.weight > best.weight) {
                            best = candidate
                        }
                    }
                } break
                case DirectionType.up: {
                    if (BgCommHandler.elements_in_column(candidate, elem)) {
                        if (candidate.weight < best.weight) {
                            best = candidate
                        }
                    }
                } break
            }
        })
        if (best == elem) {
            best = null
        } 
        return best
    }

    static elements_in_row(e1: IElementInfo, e2: IElementInfo) {
        return (e1.box.top <= e2.box.bottom) && (e2.box.top <= e1.box.bottom)
    }

    static elements_in_column(e1: IElementInfo, e2: IElementInfo) {
        return (e1.box.left <= e2.box.right) && (e2.box.left <= e1.box.right)
    }

    // Functor Queue.
    queue(task: ()=>void) {
        this.tasks.push(task)
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

    // Other.
    block_events() {
        let req = new RequestMessage(-1, RequestType.kBlockEvents)
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log('finished block events')
            this.run_next_task()
        })
    }

    unblock_events() {
        let req = new RequestMessage(-1, RequestType.kUnblockEvents)
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log('finished unblock events')
            this.run_next_task()
        })
    }

    wait_until_loaded() {
        let req = new RequestMessage(-1, RequestType.kWaitUntilLoaded)
        this.collected_booleans.length = 0
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log('finished waiting until loaded')
            if (this.collected_booleans.length == 0) {
                clearInterval(this.timer)
                this.timer = null
                this.run_next_task()
            } else if (this.timer == null) {
                this.timer = setInterval(() => { this.wait_until_loaded() }, 1000)
            }
        })
    }

    // Element mutation methods.
    update_element() {
        let req = new RequestMessage(-1, RequestType.kUpdateElement)
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log('finished update element')
            this.run_next_task()
        })
    }

    clear_element() {
        let req = new RequestMessage(-1, RequestType.kClearElement)
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log('finished clear element')
            this.run_next_task()
        })
    }

    // Result will be in this.found_elem. Error will be in this.error_msg.
    get_current_element() {
        this.found_elem = null
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kGetElement)
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
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
    // The result wil be the element info argument on success. Null otherwise.
    set_current_element(info: IElementInfo) {
        this.found_elem = null
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kSetElement, info)
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log('finished set current element')
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

    // Result will be in this.found_elems. Error will be in this.error_msg.
    find_all_elements_by_type(wrap_type: WrapType) {
        this.found_elems = []
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kFindElementByType, {wrap_type: wrap_type})
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    // Result will be in this.found_elems. Error will be in this.error_msg.
    find_all_elements_by_values(wrap_type: WrapType, target_values: string[]) {
        this.found_elems = []
        this.error_msg = ""
        this.collected_elems.length = 0
        let req = new RequestMessage(-1, RequestType.kFindElementByValues, {wrap_type: wrap_type, target_values: target_values})
        console.log("find_all_elements_by_values 1111")
        this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
            console.log("find_all_elements_by_values 2222")
            this.found_elems = JSON.parse(JSON.stringify(this.collected_elems))
            this.run_next_task()
        })
    }

    // Handler.
    handle_nodejs_request(req: RequestMessage) {
        //console.log('handling request from nodejs: ' + JSON.stringify(req))
        
        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.

        switch (req.request) {

            // --------------------------------------------------
            // Request handled by BrowserWrap.
            // --------------------------------------------------
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
                    let response = new ResponseMessage(req.id, true, {'zoom': zoom})
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom);
            } break


            // --------------------------------------------------
            // Requests that are broadcast to all frames.
            // --------------------------------------------------
            case RequestType.kBlockEvents: {
                this.clear_tasks()
                this.queue(() => {
                    this.block_events()
                })
                this.queue(()=> {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kUnblockEvents: {
                this.clear_tasks()
                this.queue(() => {
                    this.unblock_events()
                })
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kWaitUntilLoaded: {
                this.clear_tasks()
                this.queue(() => {
                    this.wait_until_loaded()
                })
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kUpdateElement: {
                this.clear_tasks()
                this.queue(() => {
                    this.update_element()
                })
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kClearElement:{
                this.clear_tasks()
                this.queue(() => {
                    this.clear_element()
                })
                this.queue(() => {
                    let response = new ResponseMessage(req.id, true, true)
                    this.bg_comm.send_to_nodejs(response)
                })
                this.run_next_task()
            } break
            case RequestType.kGetElement: {
                this.clear_tasks()
                this.queue(() => {
                    this.get_current_element()
                })
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
                this.queue(() => {
                    this.set_current_element(req.args)
                })
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
                this.queue(() => {
                    this.find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                    console.log("zzzzzzzzzzzzzzz")
                })
                this.queue(() => {
                    console.log("find_all_elements_by_values 3333" + this.found_elems.length)
                    if (this.found_elems.length > 0) {
                        BgCommHandler.sort_elements(this.found_elems)
                        this.found_elem = this.found_elems[0]
                        this.run_next_task()
                    } else {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements which match the given values.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.queue(() => {
                    this.set_current_element(this.found_elem)
                })
                this.queue(() => {
                    if (this.found_elem) {
                        let response = new ResponseMessage(req.id, true, this.found_elem)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                console.log("yyyyyyyyyyyyyyyyy")
                this.run_next_task()
            } break
            case RequestType.kFindElementByType: {
                this.clear_tasks()
                this.queue(() => {
                    this.find_all_elements_by_type(req.args.wrap_type)
                })
                this.queue(() => {
                    if (this.found_elems.length > 0) {
                        BgCommHandler.sort_elements(this.found_elems)
                        this.found_elem = this.found_elems[0]
                        this.run_next_task()                        
                    } else {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements which match the given type.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
                this.queue(() => {
                    this.set_current_element(this.found_elem)
                })
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
                this.queue(() => {
                    // Get our current element's info.
                    this.get_current_element()
                })
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        this.run_next_task()
                    }
                })
                this.queue(() => {
                    // Now get all the possible elements that we can shift to.
                    this.find_all_elements_by_type(req.args.wrap_type)
                })
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements to shift to.")
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let best = BgCommHandler.find_neighbor(this.found_elem, this.found_elems, req.args.direction)
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
                this.queue(() => {
                    this.set_current_element(this.found_elem)
                })
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
                this.queue(() => {
                    // Get our current element's info.
                    this.get_current_element()
                })
                this.queue(() => {
                    if (!this.found_elem) {
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, this.error_msg)
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        this.run_next_task()
                    }
                })
                this.queue(() => {
                    // Now get all the possible elements that we can shift to.
                    this.find_all_elements_by_values(req.args.wrap_type, req.args.target_values)
                })
                this.queue(() => {
                    if (this.found_elems.length == 0) {
                        // Wipe out the queue.
                        this.clear_tasks()
                        let response = new ResponseMessage(req.id, false, "Unable to find any elements with the given values to shift to.")
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let best = BgCommHandler.find_neighbor(this.found_elem, this.found_elems, req.args.direction)
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
                this.queue(() => {
                    this.set_current_element(this.found_elem)
                })
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
                this.collected_elems.length = 0
                this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
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
                console.log('finished performing element action 222')
            } break
            case RequestType.kGetCrosshairInfo: {
                this.collected_clicks.length = 0
                this.bg_comm.send_to_content(req, (unused_return_value: any) =>{
                    console.log('finished getting crosshair info 111')
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
                console.log('finished getting crosshair info 222')
            } break
            default: {
                console.error("BgCommHandler got an unknown request.")
            } break
        }

    }
}