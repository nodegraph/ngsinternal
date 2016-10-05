
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
        let success_msg = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), true)
        switch (req.request) {
            case RequestType.kUpdateOveralys: {
                // This message applies to all iframes so we don't send a reponse message back.
                // Otherwise there would be too many. Instead bg comm will send a single response back for all iframes.
                this.gui_collection.overlay_sets.update()
            } break
            case RequestType.kCacheFrame: {
                let content = document.documentElement.innerHTML //textContent;
                localStorage.setItem('smash_browse_page_cache_' + PageWrap.get_iframe_index_path_as_string(window), content);
                // let serializer = new XMLSerializer()
                // let content = serializer.serializeToString(document.body)
                // localStorage.setItem('smash_browse_page_cache_' + PageWrap.get_iframe_index_path_as_string(window), content);
            } break
            case RequestType.kLoadCachedFrame: {
                let content = localStorage.getItem('smash_browse_page_cache_' + PageWrap.get_iframe_index_path_as_string(window));
                //document.documentElement.innerHTML = content //textContent = content
                document.write(content)
            } break
            case RequestType.kGetXPath: {
                // Determine the xpath of the overlay element.
                let xpath = this.gui_collection.overlay_sets.get_xpath(req.args.set_index, req.args.overlay_index)
                let value = { xpath: xpath }
                let resp = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), true, value)
                this.content_comm.send_to_bg(resp)
            } break
            case RequestType.kCreateSetFromMatchValues: {
                switch (req.args.wrap_type) {
                    case WrapType.text: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.text, req.args.text_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                    } break
                    case WrapType.image: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.image, req.args.image_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                        this.gui_collection.overlay_sets.update()
                        console.log('overlay set added for image')
                    } break
                    default: {
                        console.error("Error: Attempt to create set from unknown wrap type.")
                    }
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
                        case Direction.down: {
                            this.gui_collection.overlay_sets.scroll_down(req.args.set_index, req.args.overlay_index);
                        } break
                        case Direction.up: {
                            this.gui_collection.overlay_sets.scroll_up(req.args.set_index, req.args.overlay_index);
                        } break
                        case Direction.right: {
                            this.gui_collection.overlay_sets.scroll_right(req.args.set_index, req.args.overlay_index);
                        } break
                        case Direction.left: {
                            this.gui_collection.overlay_sets.scroll_left(req.args.set_index, req.args.overlay_index);
                        } break
                    }
                }
                this.content_comm.send_to_bg(success_msg)
            } break
            case RequestType.kGetCrosshairInfo: {
                let info = this.gui_collection.get_crosshair_info(req.args.click_pos)
                let resp = new ResponseMessage(req.id, PageWrap.get_iframe_index_path_as_string(window), true, info)
                this.content_comm.send_to_bg(resp)
            } break

        }

    }

}