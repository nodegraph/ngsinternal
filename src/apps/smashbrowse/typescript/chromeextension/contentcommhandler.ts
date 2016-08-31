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

    handle_bg_request(request: any) {
        console.log('content script received message from bg: ' + JSON.stringify(request))
        switch (request.request) {
            case 'create_set_from_match_values':
                switch (request.wrap_type) {
                    case WrapType.text: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.text, request.match_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                    }
                        break
                    case WrapType.image: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.image, request.match_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                    }
                        break
                    default:
                        console.log("Error: create_set_from_match_values")
                }
                break
            case 'create_set_from_wrap_type':
                let elem_wraps = this.gui_collection.page_wrap.get_by_any_value(request.wrap_type, [])
                this.gui_collection.add_overlay_set(elem_wraps)
                break
            case 'delete_set':
                this.gui_collection.overlay_sets.destroy_set_by_index(request.set_index)
                break
            case 'shift_set':
                this.gui_collection.overlay_sets.shift(request.set_index, request.direction, request.wrap_type, this.gui_collection.page_wrap)
                break
            case 'expand_set':
                let match_criteria = new MatchCriteria(request.match_criteria)
                this.gui_collection.overlay_sets.expand(request.set_index, request.direction, match_criteria, this.gui_collection.page_wrap)
                break
            case 'mark_set':
                this.gui_collection.overlay_sets.mark_set(request.set_index, true)
                break
            case 'unmark_set':
                this.gui_collection.overlay_sets.mark_set(request.set_index, false)
                break
            case 'merge_marked_sets':
                this.gui_collection.overlay_sets.merge_marked_sets()
                break
            case 'shrink_set_to_marked':
                this.gui_collection.overlay_sets.shrink_set_to_marked(request.set_index, request.directions)
                break
            case 'shrink_set':
                this.gui_collection.overlay_sets.shrink_to_extreme(request.set_index, request.direction)
                break
            case 'perform_action':
                // We (content script) can handle only the scrolling actions.
                // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
                // In this case the scroll may not move fully to the requested position.
                if (request.action == 'scroll_down') {
                    this.gui_collection.overlay_sets.scroll_down(request.set_index, request.overlay_index);
                    break
                } else if (request.action == 'scroll_up') {
                    this.gui_collection.overlay_sets.scroll_up(request.set_index, request.overlay_index);
                    break
                } else if (request.action == 'scroll_right') {
                    this.gui_collection.overlay_sets.scroll_right(request.set_index, request.overlay_index);
                    break
                } else if (request.action == 'scroll_left') {
                    this.gui_collection.overlay_sets.scroll_left(request.set_index, request.overlay_index);
                    break
                }

                // The perform action request are usually handled by the nodejs controller.
                // However they can get send to this content script if the xpath in the request
                // has not been resolved. Once we resolve the xpath from the set_index, we reissue
                // the request, instead of sending a response. This means that these perform_action
                // requests will ultimately get sent te app.
            
                // Add the xpath into the request, and re-issue it.
                let xpath = this.gui_collection.overlay_sets.get_xpath(request.set_index, request.overlay_index)
                request.xpath = xpath

                // We also unblock events to allow a webdriver to perform an action on an element at the above xpath.
                this.gui_collection.event_blocker.unblock_events()

                // Send the modified request back to the app.
                this.content_comm.send_to_bg(request)
                return
            case 'block_events':
                this.gui_collection.event_blocker.block_events()
                // No respone message is used for this request, as it procedurally added in to
                // handle unblocking events before performing webdriver actions, and then blocking events back again.
                return
        }
        this.content_comm.send_to_bg({response: true})
    }

}