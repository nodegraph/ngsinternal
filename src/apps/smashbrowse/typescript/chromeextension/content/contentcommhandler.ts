
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

    handle_bg_info(info: InfoMessage, send_response: (response: any) => void) {
        switch (info.info) {
            case InfoType.kCollectIFrameIndexPaths: {
                let fw_index_path = PageWrap.get_fw_index_path(window)
                send_response(fw_index_path)
                break
            }
            case InfoType.kCollectIFrameOffsets: {
                let elements = document.getElementsByTagName("iframe")
                let offsets: { fw_index_path: string, element_index: number, bounds: IBox }[] = []
                for (let i = 0; i < elements.length; i++) {
                    let fw_index_path = PageWrap.get_fw_index_path(elements[i].contentWindow)
                    let rect = elements[i].getBoundingClientRect()
                    let bounds = {left: rect.left, right: rect.right, bottom: rect.bottom, top: rect.top}
                    offsets.push({fw_index_path: fw_index_path, element_index: i, bounds: bounds})
                }
                let fw_index_path = PageWrap.get_fw_index_path(window)
                send_response({fw_index_path: fw_index_path, offsets: offsets})
                break
            }
            case InfoType.kDistributeIFrameOffsets: {
                PageWrap.local_to_global_offset = info.value.offset
                PageWrap.fe_index_path = info.value.fe_index_path
                //console.log('frame index path: ' + PageWrap.get_fw_index_path(window) + ' fe_index_path: ' + PageWrap.fe_index_path)
                send_response(true)
                break
            }
        }        
    }

    handle_bg_request(req: RequestMessage, send_response: (response: any) => void) {
        let success_msg = new ResponseMessage(req.id, true)
        switch (req.request) {
            case ChromeRequestType.kBlockEvents: {
                // There is no information to send back from this call.
                this.gui_collection.event_blocker.block_events()
            } break
            case ChromeRequestType.kUnblockEvents: {
                // There is no information to send back from this call.
                this.gui_collection.event_blocker.unblock_events()
            } break
            case ChromeRequestType.kWaitUntilLoaded: {
                if (mutation_monitor.is_loading()) {
                    send_response(false)
                }
            } break
            case ChromeRequestType.kUpdateElement: {
                // There is no information to send back from this call.
                this.gui_collection.page_overlays.update_element_overlay()
                this.gui_collection.page_overlays.clear_crosshair()
            } break
            case ChromeRequestType.kClearElement: {
                // There is no information to send back from this call.
                this.gui_collection.page_overlays.clear_elem_wrap()
            } break
            case ChromeRequestType.kGetElement: {
                // Only one or none of the frames should have an element.
                let elem_wrap = this.gui_collection.page_overlays.get_elem_wrap()
                
                // If the xpath is not empty then we are that one element.
                if (elem_wrap) {
                    let info = elem_wrap.get_info()
                    send_response(info)
                }
            } break
            case ChromeRequestType.kSetElement: {
                // Clear out our current element, if any.
                this.gui_collection.page_overlays.clear_elem_wrap()
                // Now if our frame matches the request, then try to find the element.
                let fe_index_path = PageWrap.get_fw_index_path(window) 
                if (req.args.fw_index_path == fe_index_path) {
                    let elem_wraps = this.gui_collection.page_wrap.get_visible_by_xpath(req.args.xpath)
                    if (elem_wraps.length == 1) {
                        this.gui_collection.page_overlays.set_elem_wrap(elem_wraps[0])
                        // We send the element info back to bg comm. Only one of these frames should be returning a value.
                        let info = elem_wraps[0].get_info()
                        send_response(info)
                    } else if (elem_wraps.length > 1) {
                        console.error("More than one element matches the xpath of the element we want to make current: " + fe_index_path)
                    } else {
                        console.error("No element matches the xpath of the element we want to make current: " + fe_index_path)
                    }
                }
            } break
            case ChromeRequestType.kScrollElementIntoView: {
                // Only one or none of the frames should have an element.
                let elem_wrap = this.gui_collection.page_overlays.get_elem_wrap()
                
                // If the xpath is not empty then we are that one element.
                if (elem_wrap) {
                    // Scroll our frame into view.
                    PageWrap.scroll_into_view(window)
                    // Scroll our elememt into view in our frame.
                    elem_wrap.scroll_into_view()
                    // Get the info.
                    let info = elem_wrap.get_info()
                    send_response(info)
                }
            } break
            case ChromeRequestType.kFindElementByValues: {
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
            case ChromeRequestType.kFindElementByType: {
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
            // case ChromeRequestType.kPerformElementAction: {
            //     // On the content script side we only handle the scrolling actions.
            //     // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
            //     // In this case the scroll may not move fully to the requested position.
            //     // The user must submit another scroll request in this case to go further.
            //     if (this.gui_collection.overlay_sets.get_num_sets() == 0) {
            //         return
            //     }
            //     if (req.args.element_action == ElementActionType.kScroll) {
            //         switch (req.args.scroll_direction) {
            //             case DirectionType.down: {
            //                 this.gui_collection.overlay_sets.scroll_down(0, 0);
            //             } break
            //             case DirectionType.up: {
            //                 this.gui_collection.overlay_sets.scroll_up(0, 0);
            //             } break
            //             case DirectionType.right: {
            //                 this.gui_collection.overlay_sets.scroll_right(0, 0);
            //             } break
            //             case DirectionType.left: {
            //                 this.gui_collection.overlay_sets.scroll_left(0, 0);
            //             } break
            //         }
            //     }
            //     // Only one of the frames will actually send a response back to bgcomm.
            //     let elem_wrap = this.gui_collection.overlay_sets.get_elem_wrap(0, 0)
            //     let info = elem_wrap.get_info()
            //     send_response(info)
            // } break
            case ChromeRequestType.kGetCrosshairInfo: {
                let pos = new Point(req.args.global_mouse_position)
                // Get the frame bounds in global client space.
                let box = PageWrap.get_global_client_frame_bounds(window)
                // If this frame doesn't contain the click point, return.
                if (!box.contains_point(pos)) {
                    return
                }
                // If any of our child frames contains the click point, they will also
                // return a IClickInfo to the background script. The background script
                // select the best one to use.
                let info = this.gui_collection.get_crosshair_info(pos)
                if (info != null) {
                    send_response(info)
                }
            } break
            case ChromeRequestType.kGetElementValues: {
                // Only one or none of the frames should have an element.
                let elem_wrap = this.gui_collection.page_overlays.get_elem_wrap()
                if (!elem_wrap) {
                    return
                }

                // Get the center of the current element in global client space.
                let elem_box = new Box(elem_wrap.get_box())
                elem_box.to_client_space(window)
                elem_box.add_offset(PageWrap.get_offset()) // convert local to global client space
                let pos = elem_box.get_center()

                // Get the crosshair info using the center point.
                let info = this.gui_collection.get_crosshair_info(pos)
                if (info != null) {
                    send_response(info)
                }
            } break
            case ChromeRequestType.kGetDropDownInfo: {
                if (!this.gui_collection.page_overlays.get_elem_wrap()) {
                    return
                }
                let info: IDropDownInfo = this.gui_collection.get_drop_down_info()
                send_response(info)
            } break

        }

    }

}