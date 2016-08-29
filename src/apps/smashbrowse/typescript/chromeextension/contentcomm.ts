/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>

//Class which handles communication between:
//1) this content script and the background script.
class ContentComm {
    // Our dependencies.
    private page_wrap: PageWrap
    private overlay_sets: OverlaySets
    private distinct_colors: DistinctColors
    private event_blocker: EventBlocker

    // Our members.
    private bg_receivers: {[request_type: string]: (message: any)=>void}
    
    // Constructor.
    constructor (overlay_sets: OverlaySets, distinct_colors: DistinctColors) {
        this.overlay_sets = overlay_sets
        this.distinct_colors = distinct_colors
        this.connect_to_bg()
    }

    // The page wrap needs to be set outside the constructor because it creates a constructor loop with PageWrap.
    set_page_wrap(page_wrap: PageWrap): void {
        this.page_wrap = page_wrap
    }
    set_event_blocker(event_blocker: EventBlocker) : void {
        this.event_blocker = event_blocker
    }
    
    //Setup communication channel with chrome runtime.
    connect_to_bg(): void {
        chrome.runtime.onMessage.addListener(this.receive_from_bg.bind(this))
    }

    //Send a message to the bg script.
    send_to_bg(socket_message: any): void {
        chrome.runtime.sendMessage(socket_message)
    }

    register_bg_message_receiver(request_code: string, callback: (message: any)=>void) {
        this.bg_receivers[request_code] = callback
    }

    //Receive a message from the bg script.
    receive_from_bg(request: any, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) {
        console.log('content script received message from bg: ' + JSON.stringify(request))
        switch (request.request) {
            case 'create_set_from_match_values':
                switch (request.wrap_type) {
                    case WrapType.text: {
                        let elem_wraps = this.page_wrap.get_by_all_values(WrapType.text, request.match_values)
                        let os = new OverlaySet(elem_wraps, this.distinct_colors)
                        this.overlay_sets.add_set(os)
                    }
                        break
                    case WrapType.image: {
                        let elem_wraps = this.page_wrap.get_by_all_values(WrapType.image, request.match_values)
                        let os = new OverlaySet(elem_wraps, this.distinct_colors)
                        this.overlay_sets.add_set(os)
                    }
                        break
                    default:
                        console.log("Error: create_set_from_match_values")
                }
                break
            case 'create_set_from_wrap_type':
                let elem_wraps = this.page_wrap.get_by_any_value(request.wrap_type, [])
                this.overlay_sets.add_set(new OverlaySet(elem_wraps, this.distinct_colors))
                break
            case 'delete_set':
                this.overlay_sets.destroy_set_by_index(request.set_index)
                break
            case 'shift_set':
                this.overlay_sets.shift(request.set_index, request.direction, request.wrap_type, this.page_wrap)
                break
            case 'expand_set':
                let match_criteria = new MatchCriteria(request.match_criteria)
                this.overlay_sets.expand(request.set_index, request.direction, match_criteria, this.page_wrap)
                break
            case 'mark_set':
                this.overlay_sets.mark_set(request.set_index, true)
                break
            case 'unmark_set':
                this.overlay_sets.mark_set(request.set_index, false)
                break
            case 'merge_marked_sets':
                this.overlay_sets.merge_marked_sets()
                break
            case 'shrink_set_to_marked':
                this.overlay_sets.shrink_set_to_marked(request.set_index, request.directions)
                break
            case 'shrink_set':
                this.overlay_sets.shrink_to_extreme(request.set_index, request.direction)
                break
            case 'perform_action':
                // We (content script) can handle only the scrolling actions.
                // When scrolling there may be AJAX requests dynamically loading elements into the scrolled page.
                // In this case the scroll may not move fully to the requested position.
                if (request.action == 'scroll_down') {
                    this.overlay_sets.scroll_down(request.set_index, request.overlay_index);
                    break
                } else if (request.action == 'scroll_up') {
                    this.overlay_sets.scroll_up(request.set_index, request.overlay_index);
                    break
                } else if (request.action == 'scroll_right') {
                    this.overlay_sets.scroll_right(request.set_index, request.overlay_index);
                    break
                } else if (request.action == 'scroll_left') {
                    this.overlay_sets.scroll_left(request.set_index, request.overlay_index);
                    break
                }

                // The perform action request are usually handled by the nodejs controller.
                // However they can get send to this content script if the xpath in the request
                // has not been resolved. Once we resolve the xpath from the set_index, we reissue
                // the request, instead of sending a response. This means that these perform_action
                // requests will ultimately get sent te app.
            
                // Add the xpath into the request, and re-issue it.
                let xpath = this.overlay_sets.get_xpath(request.set_index, request.overlay_index)
                request.xpath = xpath

                // We also unblock events to allow a webdriver to perform an action on an element at the above xpath.
                this.event_blocker.unblock_events()

                // Send the modified request back to the app.
                this.send_to_bg(request)
                return
            case 'block_events':
                this.event_blocker.block_events()
                // No respone message is used for this request, as it procedurally added in to
                // handle unblocking events before performing webdriver actions, and then blocking events back again.
                return
        }
        this.send_to_bg({response: true})
    }
}















