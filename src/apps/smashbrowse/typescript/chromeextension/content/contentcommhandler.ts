
class ContentCommHandler {
    // Our Dependencies.
    content_comm: ContentComm
    gui_collection: GUICollection

    // Our Members.
    

    constructor(cc: ContentComm, gc: GUICollection) {
        this.content_comm = cc
        this.content_comm.register_bg_request_handler(this)
        this.gui_collection = gc
    }

    handle_bg_response(resp: ResponseMessage) {
        console.error('Got unexpected reponse message from bg.')
    }

    handle_bg_info(info: InfoMessage) {
        console.error('Got unexpected info message from bg.')
    }

    handle_bg_request(req: RequestMessage, send_response: (response: any) => void) {
        let iframe_index_path = PageWrap.get_iframe_index_path_as_string(window) 
        console.log('content script ' + iframe_index_path + ' received message from bg: ' + JSON.stringify(req))
        let success_msg = new ResponseMessage(req.id, true)
        switch (req.request) {
            case RequestType.kBlockEvents: {
                // There is no information to send back from this call.
                this.gui_collection.event_blocker.block_events()
            } break
            case RequestType.kUnblockEvents: {
                // There is no information to send back from this call.
                this.gui_collection.event_blocker.unblock_events()
            } break
            case RequestType.kWaitUntilLoaded: {
                if (mutation_monitor.is_loading()) {
                    //mutation_monitor.add_loaded_callback(
                    //    () => { this.content_comm.send_to_bg(success_msg) }
                    //)
                    let msg = new InfoMessage(0, InfoType.kCollectBoolean, false)
                    this.content_comm.send_to_bg(msg)
                }
            } break
            case RequestType.kUpdateElement: {
                // There is no information to send back from this call.
                this.gui_collection.overlay_sets.update()
            } break
            case RequestType.kClearElement: {
                // There is no information to send back from this call.
                this.gui_collection.overlay_sets.destroy()
            }
            case RequestType.kGetElement: {
                // Only one or none of the frames should have an element.
                let xpath = this.gui_collection.overlay_sets.get_xpath(0, 0)
                // If the xpath is not empty then we are that one element.
                if (xpath != "") {
                    let value: IElementInfo = { iframe_index_path: iframe_index_path, xpath: xpath }
                    let msg = new InfoMessage(0, InfoType.kCollectElement, value)
                    this.content_comm.send_to_bg(msg)
                }
            } break
            case RequestType.kSetElement: {
                // Clear out our current element, if any.
                this.gui_collection.overlay_sets.destroy()
                // Now if our frame matches the request, then try to find the element.
                if (req.args.iframe_index_path == iframe_index_path) {
                    let elem_wraps = this.gui_collection.page_wrap.get_visible_by_xpath(req.args.xpath)
                    if (elem_wraps.length == 1) {
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()

                        // We send the element info back to bg comm. Only one of these frames should be returning a value.
                        let value: IElementInfo = { iframe_index_path: iframe_index_path, xpath: req.args.xpath }
                        let im = new InfoMessage(0, InfoType.kCollectElement, value)
                        this.content_comm.send_to_bg(im)
                    } else {
                        console.error("More than one element is selected in frame: " + iframe_index_path)
                    }
                }
            } break
            case RequestType.kFindElementByValues: {
                let elem_wraps: ElemWrap[] = this.gui_collection.page_wrap.get_by_all_values(req.args.wrap_type, req.args.target_values)
                if (elem_wraps.length > 0) {
                    let values : IElementInfo[] = []
                    elem_wraps.forEach((e)=>{
                        e.update()
                        let box = new Box(e.get_box())
                        box.to_local_client_space(window)
                        box.to_global_client_space(window)
                        values.push({iframe_index_path: iframe_index_path, xpath: e.get_xpath(), box: box })
                    })
                    let im = new InfoMessage(0, InfoType.kCollectElements, values)
                    this.content_comm.send_to_bg(im)
                }
            } break
            case RequestType.kFindElementByType: {
                let elem_wraps = this.gui_collection.page_wrap.get_by_any_value(req.args.wrap_type, [])
                if (elem_wraps.length > 0) {
                    let values : IElementInfo[] = []
                    elem_wraps.forEach((e)=>{
                        e.update()
                        let box = new Box(e.get_box())
                        box.to_local_client_space(window)
                        box.to_global_client_space(window)
                        values.push({iframe_index_path: iframe_index_path, xpath: e.get_xpath(), box: box })
                    })
                    let im = new InfoMessage(0, InfoType.kCollectElements, values)
                    this.content_comm.send_to_bg(im)
                }
            } break
            case RequestType.kPerformElementAction: {
                // On the content script side we only handle the scrolling actions.
                // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
                // In this case the scroll may not move fully to the requested position.
                // The user must submit another scroll request in this case to go further.
                if (this.gui_collection.overlay_sets.get_num_sets() == 0) {
                    return
                }
                if (req.args.action == ElementActionType.kScroll) {
                    switch (req.args.direction) {
                        case DirectionType.down: {
                            this.gui_collection.overlay_sets.scroll_down(0, 0);
                        } break
                        case DirectionType.up: {
                            this.gui_collection.overlay_sets.scroll_up(0, 0);
                        } break
                        case DirectionType.right: {
                            this.gui_collection.overlay_sets.scroll_right(0, 0);
                        } break
                        case DirectionType.left: {
                            this.gui_collection.overlay_sets.scroll_left(0, 0);
                        } break
                    }
                }
                // Only one of the frames will actually send a response back to bgcomm.
                let xpath = this.gui_collection.overlay_sets.get_xpath(0, 0)
                let info: IElementInfo = { iframe_index_path: iframe_index_path, xpath: xpath }
                let im = new InfoMessage(0, InfoType.kCollectElement, info)
                this.content_comm.send_to_bg(im)
            } break
            case RequestType.kGetCrosshairInfo: {
                let pos = new Point(req.args.click_pos)
                // Get the frame bounds in global client space.
                let box = PageWrap.get_iframe_global_client_bounds(window)
                // If this frame doesn't contain the click point, return.
                if (!box.contains_point(pos)) {
                    console.log('111')
                    return
                }
                // If any of our child frames contains the click point, return.
                {
                    var iframes = window.document.getElementsByTagName('iframe');
                    for (let i = 0; i < iframes.length; i++) {
                        let child_box = PageWrap.get_iframe_global_client_bounds(iframes[i].contentWindow)
                        if (child_box.contains_point(pos)) {
                            console.log('222')
                            return
                        }
                    }
                }
                // Otherwise we get crosshair info.
                let info = this.gui_collection.get_crosshair_info(new Point(req.args.click_pos))
                let resp = new InfoMessage(-1, InfoType.kCollectClick, info)
                this.content_comm.send_to_bg(resp)
                console.log('333' + JSON.stringify(info))
            } break

        }

    }

}