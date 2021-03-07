
class GUICollection {

    event_blocker: EventBlocker
    content_comm: ContentComm
    page_overlays: PageOverlays
    page_wrap: PageWrap

    wait_popup: WaitPopup

    private distinct_colors: DistinctColors

    constructor(content_comm: ContentComm) {
        this.content_comm = content_comm

        // Our popups.
        this.wait_popup = new WaitPopup()

        // Our overlay sets.
        this.distinct_colors = new DistinctColors()

        // Our page wrap.
        this.page_wrap = new PageWrap(this)

        // Our context menu.
        this.page_overlays = new PageOverlays()

        // Our event blocker.
        this.event_blocker = new EventBlocker(this)
    }

    initialize() {
        if (!this.page_overlays.initialized()) {
            this.page_overlays.initialize()
        }
        if (!this.wait_popup.is_initialized) {
            this.wait_popup.initialize()
        }
    }

    contains_element(element: Node): boolean {
        if (this.wait_popup.contains_element(element)) {
            return true
        }
        return false
    }

    // Get global mouse position which is with respect to the entire page which may contain many embedded frames.
    get_global_mouse_position(e: MouseEvent) {
        // Get the click position in global client space.
        let pos = new Point({ x: e.clientX, y: e.clientY })
        pos = pos.add(PageWrap.get_offset()) // convert local to global client space
        return pos
    }

    // Get the local mouse position which with respect to the provided elem wrap argument.
    get_local_mouse_position(e: MouseEvent, elem_wrap: ElemWrap) {
        // We work in local page space.
        let local_page_click = new Point({ x: e.pageX, y: e.pageY })
        let smallest = PageWrap.get_smallest_element(local_page_click)
        let local_element_relative_click = smallest.get_box().get_relative_point(local_page_click)
        return local_element_relative_click
    }

    global_to_local_client_pos(global_client_click: Point) {
        let local_page_click = new Point(global_client_click)
        local_page_click = local_page_click.subtract(PageWrap.get_offset()) // convert global to local client space
        local_page_click.to_page_space(window)
        return local_page_click
    }

    on_context_menu(e: MouseEvent): boolean {
        if (!this.page_overlays.initialized()) {
            return
        }

        // Update the click box overly with local page space coordinates.
        let local_page_click = new Point({ x: e.pageX, y: e.pageY })
        this.page_overlays.update_crosshair(local_page_click)

        // Find the smallest element that overlaps this point.
        let smallest = PageWrap.get_smallest_element(local_page_click)

        // Get the click position in global client space.
        let global_mouse_position = this.get_global_mouse_position(e)
        let local_mouse_position = this.get_local_mouse_position(e, smallest)

        // Form the IClickInfo object.
        let elem_info = smallest.get_info()
        let click_info = <IClickInfo>(elem_info)
        click_info.global_mouse_position = global_mouse_position
        click_info.local_mouse_position = local_mouse_position

        // Send the response.
        let im = new InfoMessage(0, InfoType.kShowWebActionMenu, click_info)
        this.content_comm.send_to_bg(im)

        // Prevent default behavior of the event.
        e.preventDefault();
        return false
    }

    on_mouse_over(e: MouseEvent): void {
        let point = new Point({ x: e.pageX, y: e.pageY })
        let text_elem_wrap = PageWrap.get_smallest_element_with_text(point)
        let image_elem_wrap = PageWrap.get_smallest_element_with_images(point)
        this.page_overlays.update_overlays(text_elem_wrap, image_elem_wrap)
    }

}