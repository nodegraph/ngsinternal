
let bg_comm: BgComm = null
let browser_wrap: BrowserWrap = null

let distinct_colors: DistinctColors = null
let overlay_sets: OverlaySets = null

let wait_popup: WaitPopup = null
let text_input_popup: TextInputPopup = null
let select_input_popup: SelectInputPopup = null

// We only create on the top window, not in other iframes.
if (window == window.top) {
    bg_comm = new BgComm()
    browser_wrap = new BrowserWrap(bg_comm)

    // Web Page Overlays.
    distinct_colors = new DistinctColors()
    overlay_sets = new OverlaySets()

    // Popups.
    let wait_popup = new WaitPopup()
    let text_input_popup = new TextInputPopup()
    let select_input_popup = new SelectInputPopup()
} else {

}





let content_comm: ContentComm = null
let page_wrap: PageWrap = null

// We only create on the top window, not in other iframes.
if (window == window.top) {
    content_comm = new ContentComm(overlay_sets, distinct_colors)
    page_wrap = new PageWrap(content_comm)
    content_comm.set_page_wrap(page_wrap)
}

// Start listening for mutations on page load.
document.addEventListener('DOMContentLoaded', page_wrap.on_loaded.bind(page_wrap), false);





let context_menu: ContextMenu = null
// We only create on the top window, not in other iframes.
if (window == window.top) {
    context_menu = new ContextMenu(
        distinct_colors,
        page_wrap,
        content_comm,
        text_input_popup,
        wait_popup,
        select_input_popup,
        overlay_sets
    )
}

let event_blocker = new EventBlocker(context_menu);
event_blocker.block_events()
content_comm.set_event_blocker(event_blocker)