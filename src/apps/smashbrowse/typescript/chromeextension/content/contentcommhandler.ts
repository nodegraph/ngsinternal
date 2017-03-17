
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
            case InfoType.kCollectFrameIndexPaths: {
                let fw_index_path = PageWrap.get_fw_index_path(window)
                send_response(fw_index_path)
                break
            }
            case InfoType.kCollectFrameOffsets: {
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
            case InfoType.kDistributeFrameOffsets: {
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
                    let elem_wraps = PageWrap.get_visible_by_xpath(req.args.xpath)
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
                    // Scroll our elememt into view.
                    elem_wrap.scroll_into_view()
                    // Get the info.
                    let info = elem_wrap.get_info()
                    send_response(info)
                }
            } break
            case ChromeRequestType.kScrollElement: {
                // Only one or none of the frames should have an element.
                let elem_wrap = this.gui_collection.page_overlays.get_elem_wrap()
                
                // If the xpath is not empty then we are that one element.
                if (elem_wrap) {
                    
                    switch (req.args.scroll_direction) {
                        case DirectionType.down: {
                            let scroll = elem_wrap.get_closest_scroll(true)
                            if (scroll) {
                                scroll.scroll_down()
                            }
                        } break
                        case DirectionType.up: {
                            let scroll = elem_wrap.get_closest_scroll(true)
                            if (scroll) {
                                scroll.scroll_up()
                            }
                        } break
                        case DirectionType.left: {
                            let scroll = elem_wrap.get_closest_scroll(false)
                            if (scroll) {
                                scroll.scroll_left()
                            }
                        } break
                        case DirectionType.right: {
                            let scroll = elem_wrap.get_closest_scroll(false)
                            if (scroll) {
                                scroll.scroll_right()
                            }
                        } break
                        default: {
                            console.log("Error: unknown direction type: " + req.args.scroll_direction)
                        }
                    }
                    
                    // Get the info.
                    let info = elem_wrap.get_info()
                    send_response(info)
                }
            } break
            case ChromeRequestType.kFindElementByValues: {
                // let elem_wraps: ElemWrap[] = this.gui_collection.page_wrap.get_by_all_values(req.args.wrap_type, req.args.target_values)
                // if (elem_wraps.length > 0) {
                //     let infos : IElementInfo[] = []
                //     elem_wraps.forEach((e)=>{
                //         let info = e.get_info()
                //         infos.push(info)
                //     })
                //     send_response(infos)
                // }
            } break
            case ChromeRequestType.kFindElementByType: {
                // let elem_wraps: ElemWrap[] = this.gui_collection.page_wrap.get_all_visible()
                // elem_wraps = PageWrap.filter_out_our_gui(elem_wraps)
                // switch (req.args.wrap_type) {
                //     case WrapType.image: {
                //         elem_wraps = PageWrap.filter_by_value(elem_wraps, ElemWrap.image_getter, '')
                //     }break
                //     case WrapType.text: {
                //         elem_wraps = PageWrap.filter_by_value(elem_wraps, ElemWrap.text_getter, '')
                //     }break
                //     case WrapType.input: {
                //         elem_wraps = PageWrap.filter_by_value(elem_wraps, ElemWrap.input_getter, '')
                //     }break
                //     case WrapType.select: {
                //         elem_wraps = PageWrap.filter_by_value(elem_wraps, ElemWrap.select_getter, '')
                //     }break
                // }
                 
                // if (elem_wraps.length > 0) {
                //     let infos : IElementInfo[] = []
                //     elem_wraps.forEach((e)=>{
                //         let info = e.get_info()
                //         infos.push(info)
                //     })
                //     send_response(infos)
                // }
            } break
            case ChromeRequestType.kGetAllElements: {
                let elem_wraps: ElemWrap[] = PageWrap.get_all_visible()
                // Convert to IElementInfos.
                let infos : IElementInfo[] = []
                elem_wraps.forEach((e)=>{
                    let info = e.get_info()
                    infos.push(info)
                })
                send_response(infos)
            } break
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