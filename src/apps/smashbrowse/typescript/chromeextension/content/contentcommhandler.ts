
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
        let frame_index_path = PageWrap.get_frame_index_path(window) 
        console.log('content script ' + frame_index_path + ' received message from bg: ' + JSON.stringify(req))
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
                    send_response(false)
                }
            } break
            case RequestType.kUpdateElement: {
                // There is no information to send back from this call.
                this.gui_collection.overlay_sets.update()
            } break
            case RequestType.kClearElement: {
                // There is no information to send back from this call.
                this.gui_collection.overlay_sets.destroy()
            } break
            case RequestType.kGetElement: {
                // Only one or none of the frames should have an element.
                let elem_wrap = this.gui_collection.overlay_sets.get_elem_wrap(0, 0)
                
                // If the xpath is not empty then we are that one element.
                if (elem_wrap) {
                    let info = elem_wrap.get_info()
                    send_response(info)
                }
            } break
            case RequestType.kSetElement: {
                // Clear out our current element, if any.
                this.gui_collection.overlay_sets.destroy()
                // Now if our frame matches the request, then try to find the element.
                if (req.args.frame_index_path == frame_index_path) {
                    let elem_wraps = this.gui_collection.page_wrap.get_visible_by_xpath(req.args.xpath)
                    if (elem_wraps.length == 1) {
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                        // We send the element info back to bg comm. Only one of these frames should be returning a value.
                        let info = elem_wraps[0].get_info()
                        send_response(info)
                    } else if (elem_wraps.length > 1) {
                        console.error("More than one element matches the xpath of the element we want to make current: " + frame_index_path)
                    } else {
                        console.error("No element matches the xpath of the element we want to make current: " + frame_index_path)
                    }
                }
            } break
            case RequestType.kFindElementByValues: {
                let elem_wraps: ElemWrap[] = this.gui_collection.page_wrap.get_by_all_values(req.args.wrap_type, req.args.target_values)
                if (elem_wraps.length > 0) {
                    let infos : IElementInfo[] = []
                    elem_wraps.forEach((e)=>{
                        let info = e.get_info()
                        infos.push(info)
                    })
                    send_response(infos)
                }
            } break
            case RequestType.kFindElementByType: {
                let elem_wraps = this.gui_collection.page_wrap.get_by_any_value(req.args.wrap_type, [])
                if (elem_wraps.length > 0) {
                    let infos : IElementInfo[] = []
                    elem_wraps.forEach((e)=>{
                        let info = e.get_info()
                        infos.push(info)
                    })
                    send_response(infos)
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
                let elem_wrap = this.gui_collection.overlay_sets.get_elem_wrap(0, 0)
                let info = elem_wrap.get_info()
                send_response(info)
            } break
            case RequestType.kGetCrosshairInfo: {
                let pos = new Point(req.args.global_mouse_position)
                // Get the frame bounds in global client space.
                let box = PageWrap.get_global_client_frame_bounds(window)
                // If this frame doesn't contain the click point, return.
                if (!box.contains_point(pos)) {
                    return
                }
                // If any of our child frames contains the click point, return.
                {
                    var frames = window.document.getElementsByTagName('iframe');
                    for (let i = 0; i < frames.length; i++) {
                        let child_box = PageWrap.get_global_client_frame_bounds(frames[i].contentWindow)
                        if (child_box.contains_point(pos)) {
                            return
                        }
                    }
                }
                // Otherwise we get crosshair info.
                let info = this.gui_collection.get_crosshair_info(new Point(req.args.global_mouse_position))
                if (info != null) {
                    send_response(info)
                }
            } break
            case RequestType.kGetDropDownInfo: {
                if (this.gui_collection.overlay_sets.get_num_sets() == 0) {
                    return
                }
                let info: IDropDownInfo = this.gui_collection.get_drop_down_info(0,0)
                send_response(info)
            } break

        }

    }

}