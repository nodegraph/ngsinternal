import {ContentComm} from "./contentcomm"
import {BaseMessage, RequestMessage, ResponseMessage, InfoMessage, RequestType, ActionType} from "../../controller/socketmessage"
import {GUICollection} from "./guicollection"
import {ElemWrap, WrapType} from "./elemwrap"
import {MatchCriteria} from "./matchcriteria"

export class ContentCommHandler {
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

    handle_bg_request(request: RequestMessage) {
        console.log('content script received message from bg: ' + JSON.stringify(request))
        switch (request.request) {
            case RequestType.kCreateSetFromMatchValues:
                switch (request.args.wrap_type) {
                    case WrapType.text: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.text, request.args.match_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                    }
                        break
                    case WrapType.image: {
                        let elem_wraps = this.gui_collection.page_wrap.get_by_all_values(WrapType.image, request.args.match_values)
                        this.gui_collection.add_overlay_set(elem_wraps)
                    }
                        break
                    default:
                        console.log("Error: create_set_from_match_values")
                }
                break
            case RequestType.kCreateSetFromWrapType:
                let elem_wraps = this.gui_collection.page_wrap.get_by_any_value(request.args.wrap_type, [])
                this.gui_collection.add_overlay_set(elem_wraps)
                break
            case RequestType.kDeleteSet:
                this.gui_collection.overlay_sets.destroy_set_by_index(request.args.set_index)
                break
            case RequestType.kShiftSet:
                this.gui_collection.overlay_sets.shift(request.args.set_index, request.args.direction, request.args.wrap_type, this.gui_collection.page_wrap)
                break
            case RequestType.kExpandSet:
                let match_criteria = new MatchCriteria(request.args.match_criteria)
                this.gui_collection.overlay_sets.expand(request.args.set_index, request.args.direction, match_criteria, this.gui_collection.page_wrap)
                break
            case RequestType.kMarkSet:
                this.gui_collection.overlay_sets.mark_set(request.args.set_index, true)
                break
            case RequestType.kUnmarkSet:
                this.gui_collection.overlay_sets.mark_set(request.args.set_index, false)
                break
            case RequestType.kMergeMarkedSets:
                this.gui_collection.overlay_sets.merge_marked_sets()
                break
            case RequestType.kShrinkSetToMarked:
                this.gui_collection.overlay_sets.shrink_set_to_marked(request.args.set_index, request.args.directions)
                break
            case RequestType.kShrinkSet:
                this.gui_collection.overlay_sets.shrink_to_extreme(request.args.set_index, request.args.direction)
                break
            case RequestType.kPerformAction:
                // We (content script) can handle only the scrolling actions.
                // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
                // In this case the scroll may not move fully to the requested position.
                if (request.args.action == ActionType.kScrollDown) {
                    this.gui_collection.overlay_sets.scroll_down(request.args.set_index, request.args.overlay_index);
                    break
                } else if (request.args.action == ActionType.kScrollUp) {
                    this.gui_collection.overlay_sets.scroll_up(request.args.set_index, request.args.overlay_index);
                    break
                } else if (request.args.action == ActionType.kScrollRight) {
                    this.gui_collection.overlay_sets.scroll_right(request.args.set_index, request.args.overlay_index);
                    break
                } else if (request.args.action == ActionType.kScrollLeft) {
                    this.gui_collection.overlay_sets.scroll_left(request.args.set_index, request.args.overlay_index);
                    break
                }

                // The perform action request are usually handled by the nodejs controller.
                // However they can get send to this content script if the xpath in the request
                // has not been resolved. Once we resolve the xpath from the set_index, we reissue
                // the request, instead of sending a response. This means that these perform_action
                // requests will ultimately get sent te app.
            
                // Add the xpath into the request, and re-issue it.
                let xpath = this.gui_collection.overlay_sets.get_xpath(request.args.set_index, request.args.overlay_index)
                request.xpath = xpath

                // We also unblock events to allow a webdriver to perform an action on an element at the above xpath.
                this.gui_collection.event_blocker.unblock_events()

                // Send the modified request back to the app.
                this.content_comm.send_to_bg(request)
                return
            case RequestType.kBlockEvents:
                this.gui_collection.event_blocker.block_events()
                // No respone message is used for this request, as it procedurally added in to
                // handle unblocking events before performing webdriver actions, and then blocking events back again.
                return
        }
        this.content_comm.send_to_bg(new ResponseMessage(true))
    }

}