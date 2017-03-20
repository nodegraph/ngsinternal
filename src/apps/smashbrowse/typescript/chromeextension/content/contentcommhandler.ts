
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

    get_first_element_from_args(infos: IElementInfo[]): ElemWrap {
        if (infos == undefined) {
            return null
        } else if (infos.length <= 0) {
            return null
        } else {
            let elements = PageWrap.get_visible_by_xpath(infos[0].xpath)
            if (elements.length <= 0) {
                return null
            }
            return elements[0]
        }
    }

    // Note that when send_response is not called directly during the function execution, 
    // the Chrome API will invoke it automatically with the argument set to undefined.
    handle_bg_request(req: RequestMessage, send_response: (response: any) => void) {
        switch (req.request) {
            case ChromeRequestType.kBlockEvents: {
                this.gui_collection.event_blocker.block_events()
            } break
            case ChromeRequestType.kUnblockEvents: {
                this.gui_collection.event_blocker.unblock_events()
            } break
            case ChromeRequestType.kWaitUntilLoaded: {
                if (mutation_monitor.is_loading()) {
                    send_response(false)
                }
            } break
            case ChromeRequestType.kHighlightElements: {
                this.gui_collection.page_overlays.clear_element_overlays()
                this.gui_collection.page_overlays.create_element_overlays(req.args.in.elements)
            } break
            case ChromeRequestType.kUpdateElementHighlights: {
                this.gui_collection.page_overlays.update_element_overlays()
                //this.gui_collection.page_overlays.clear_crosshair()
            } break
            case ChromeRequestType.kClearElementHighlights: {
                this.gui_collection.page_overlays.clear_element_overlays()
            } break
            case ChromeRequestType.kScrollElementIntoView: {
                let elem_wrap = this.get_first_element_from_args(req.args.in.elements)
                if (!elem_wrap) {
                    break
                } else {
                    elem_wrap.scroll_into_view()
                }
            } break
            case ChromeRequestType.kScrollElement: {
                let elem_wrap = this.get_first_element_from_args(req.args.in.elements)
                if (!elem_wrap) {
                    break
                } else {
                    switch (req.args.scroll_direction) {
                        case DirectionType.kDown: {
                            let scroll = elem_wrap.get_closest_scroll(true)
                            if (scroll) {
                                scroll.scroll_down()
                            }
                        } break
                        case DirectionType.kUp: {
                            let scroll = elem_wrap.get_closest_scroll(true)
                            if (scroll) {
                                scroll.scroll_up()
                            }
                        } break
                        case DirectionType.kLeft: {
                            let scroll = elem_wrap.get_closest_scroll(false)
                            if (scroll) {
                                scroll.scroll_left()
                            }
                        } break
                        case DirectionType.kRight: {
                            let scroll = elem_wrap.get_closest_scroll(false)
                            if (scroll) {
                                scroll.scroll_right()
                            }
                        } break
                        default: {
                            console.log("Error: unknown direction type: " + req.args.scroll_direction)
                        }
                    }
                }
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
        }
    }

}