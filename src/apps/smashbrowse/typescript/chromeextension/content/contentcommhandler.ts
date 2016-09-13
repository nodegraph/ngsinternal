
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

    handle_bg_request(req: RequestMessage) {
        console.log('content script received message from bg: ' + JSON.stringify(req))
        let success_msg = new ResponseMessage(req.id, PageWrap.iframe, true)
        switch (req.request) {
            case RequestType.kUpdateOveralys: {
                this.gui_collection.overlay_sets.update()
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kCreateSetFromMatchValues: {
                switch (req.args.wrap_type) {
                    case WrapType.text: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.text, req.args.match_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                        console.log('overlay set added for text')
                    }
                        break
                    case WrapType.image: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.image, req.args.match_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                        console.log('overlay set added for image')
                    }
                        break
                    default:
                        console.error("Error: create_set_from_match_values")
                }
                this.content_comm.send_to_bg(success_msg)
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
            case RequestType.kGetOverlayXPath: {
                let xpath = this.gui_collection.overlay_sets.get_xpath(req.args.set_index, req.args.overlay_index)
                let resp = new ResponseMessage(req.id, PageWrap.iframe, true, {xpath: xpath})
                this.content_comm.send_to_bg(resp)
            } break
            case RequestType.kBlockEvents: {
                this.gui_collection.event_blocker.block_events()
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kUnblockEvents: {
                this.gui_collection.event_blocker.unblock_events()
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kPerformAction: {
                // We (content script) can handle only the scrolling actions.
                // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
                // In this case the scroll may not move fully to the requested position.
                if (req.args.action == ActionType.kScrollDown) {
                    this.gui_collection.overlay_sets.scroll_down(req.args.set_index, req.args.overlay_index);
                    break
                } else if (req.args.action == ActionType.kScrollUp) {
                    this.gui_collection.overlay_sets.scroll_up(req.args.set_index, req.args.overlay_index);
                    break
                } else if (req.args.action == ActionType.kScrollRight) {
                    this.gui_collection.overlay_sets.scroll_right(req.args.set_index, req.args.overlay_index);
                    break
                } else if (req.args.action == ActionType.kScrollLeft) {
                    this.gui_collection.overlay_sets.scroll_left(req.args.set_index, req.args.overlay_index);
                    break
                }
                this.content_comm.send_to_bg(success_msg)
                

                // // The perform action request are usually handled by the nodejs commhub.
                // // However they can get send to this content script if the xpath in the request
                // // has not been resolved. Once we resolve the xpath from the set_index, we reissue
                // // the request, instead of sending a response. This means that these perform_action
                // // requests will ultimately get sent te app.
            
                // // Add the xpath into the request, and re-issue it.
                // let xpath = this.gui_collection.overlay_sets.get_xpath(request.args.set_index, request.args.overlay_index)
                // request.xpath = xpath

                // // We also unblock events to allow a webdriver to perform an action on an element at the above xpath.
                // this.gui_collection.event_blocker.unblock_events()

                // // Send the modified request back to the app.
                // this.content_comm.send_to_bg(request)
                // return

            } break
            case RequestType.kGetCrosshairInfo: {
                let info = this.gui_collection.get_crosshair_info(req.args.click_pos)
                let resp = new ResponseMessage(req.id, PageWrap.iframe, true, info)
                this.content_comm.send_to_bg(resp)
            } break

        }
        
    }

}