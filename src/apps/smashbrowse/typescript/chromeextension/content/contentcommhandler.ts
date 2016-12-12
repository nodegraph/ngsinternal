
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
        console.log('content script ' + PageWrap.get_iframe_index_path_as_string(window) + ' received message from bg: ' + JSON.stringify(req))
        let success_msg = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), true)
        switch (req.request) {
            case RequestType.kUpdateOveralys: {
                // This message applies to all iframes so we don't send a reponse message back.
                // Otherwise there would be too many. Instead bg comm will send a single response back for all iframes.
                this.gui_collection.overlay_sets.update()
            } break
            case RequestType.kGetXPath: {
                // Determine the xpath of the overlay element.
                let xpath = this.gui_collection.overlay_sets.get_xpath(req.args.set_index, req.args.overlay_index)
                let value = { xpath: xpath }
                let resp = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), (xpath != ""), value)
                this.content_comm.send_to_bg(resp)
            } break
            case RequestType.kFindIFrame: {
                
                switch (req.args.wrap_type) {
                    case WrapType.text: {
                    let elem_wraps: ElemWrap[]
                        elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.text, req.args.text_values)
                        if (elem_wraps.length > 0) {
                            console.log('found iframe by matching text')
                            let iframe = PageWrap.get_iframe_index_path_as_string(window)
                            console.log('iframe path is: ' + iframe)
                            let box = PageWrap.get_iframe_global_client_bounds(window)
                            let im = new InfoMessage(0, iframe, InfoType.kFoundIFrame, { iframe: iframe, left: box.left, right: box.right, bottom: box.bottom, top: box.top })
                            this.content_comm.send_to_bg(im)
                        }
                    } break
                    case WrapType.image: {
                        let elem_wraps: ElemWrap[]
                        elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.image, req.args.image_values)
                        if (elem_wraps.length > 0) {
                            console.log('found iframe by matching text')
                            let iframe = PageWrap.get_iframe_index_path_as_string(window)
                            console.log('iframe path is: ' + iframe)
                            let box = PageWrap.get_iframe_global_client_bounds(window)
                            let im = new InfoMessage(0, iframe, InfoType.kFoundIFrame, { iframe: iframe, left: box.left, right: box.right, bottom: box.bottom, top: box.top })
                            this.content_comm.send_to_bg(im)
                        }
                    } break
                    case WrapType.iframe: {
                        // Note that the click_pos argument is always in global client coordinates. 
                        let box = PageWrap.get_iframe_global_client_bounds(window)
                        if (box.contains_point(req.args.click_pos)) {
                            // See if we have any child iframes which contain this point.
                            var iframes = window.document.getElementsByTagName('iframe');
                            for (let i = 0; i < iframes.length; i++) {
                                let child_box = PageWrap.get_iframe_global_client_bounds(iframes[i].contentWindow)
                                if (child_box.contains_point(req.args.click_pos)) {
                                    return
                                }
                            }
                            console.log('found iframe by matching positions')
                            let iframe = PageWrap.get_iframe_index_path_as_string(window)
                            console.log('iframe path is: ' + iframe)
                            let im = new InfoMessage(0, iframe, InfoType.kFoundIFrame, { iframe: iframe, left: box.left, right: box.right, bottom: box.bottom, top: box.top })
                            this.content_comm.send_to_bg(im)
                        }
                    } break
                    default: {
                        console.error("Error: Attempt to create set from unknown wrap type.")
                    }
                }
            } break
            case RequestType.kCreateSetFromMatchValues: {
                let elem_wraps: ElemWrap[]
                switch (req.args.wrap_type) {
                    case WrapType.text: {
                        elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.text, req.args.text_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                    } break
                    case WrapType.image: {
                        elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.image, req.args.image_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                        console.log('overlay set added for image')
                    } break
                    default: {
                        console.error("Error: Attempt to create set from unknown wrap type.")
                    }
                }
                if (elem_wraps.length > 0) {
                    this.content_comm.send_to_bg(success_msg)
                } else {
                    let failed_resp = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), false)
                    this.content_comm.send_to_bg(failed_resp)
                }
            } break
            case RequestType.kCreateSetFromWrapType: {
                let elem_wraps = this.gui_collection.page_wrap.get_by_any_value(req.args.wrap_type, [])
                this.gui_collection.add_overlay_set(elem_wraps)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kDeleteSet: {
                this.gui_collection.overlay_sets.destroy_set_by_index(req.args.set_index)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kShiftSet: {
                this.gui_collection.overlay_sets.shift(req.args.set_index, req.args.direction, req.args.wrap_type, this.gui_collection.page_wrap)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kExpandSet: {
                let match_criteria = new MatchCriteria(req.args.match_criteria)
                this.gui_collection.overlay_sets.expand(req.args.set_index, req.args.direction, match_criteria, this.gui_collection.page_wrap)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kMarkSet: {
                this.gui_collection.overlay_sets.mark_set(req.args.set_index, true)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kUnmarkSet: {
                this.gui_collection.overlay_sets.mark_set(req.args.set_index, false)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kMergeMarkedSets: {
                this.gui_collection.overlay_sets.merge_marked_sets()
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kShrinkSetToMarked: {
                this.gui_collection.overlay_sets.shrink_set_to_marked(req.args.set_index, req.args.directions)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kShrinkSet: {
                this.gui_collection.overlay_sets.shrink_to_extreme(req.args.set_index, req.args.direction)
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kBlockEvents: {
                this.gui_collection.event_blocker.block_events()
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kUnblockEvents: {
                this.gui_collection.event_blocker.unblock_events()
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kWaitUntilLoaded: {
                if (mutation_monitor.is_loading()) {
                    mutation_monitor.add_loaded_callback(
                        () => { this.content_comm.send_to_bg(success_msg) }
                    )
                } else {
                    this.content_comm.send_to_bg(success_msg)
                }
            } break
            case RequestType.kPerformElementAction: {
                // We (content script) can handle only the scrolling actions.
                // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
                // In this case the scroll may not move fully to the requested position.
                if (req.args.action == ElementActionType.kScroll) {
                    switch (req.args.direction) {
                        case DirectionType.down: {
                            this.gui_collection.overlay_sets.scroll_down(req.args.set_index, req.args.overlay_index);
                        } break
                        case DirectionType.up: {
                            this.gui_collection.overlay_sets.scroll_up(req.args.set_index, req.args.overlay_index);
                        } break
                        case DirectionType.right: {
                            this.gui_collection.overlay_sets.scroll_right(req.args.set_index, req.args.overlay_index);
                        } break
                        case DirectionType.left: {
                            this.gui_collection.overlay_sets.scroll_left(req.args.set_index, req.args.overlay_index);
                        } break
                    }
                }
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kGetCrosshairInfo: {
                let info = this.gui_collection.get_crosshair_info(new Point(req.args.click_pos))
                let resp = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), true, info)
                this.content_comm.send_to_bg(resp)
            } break

        }

    }

}