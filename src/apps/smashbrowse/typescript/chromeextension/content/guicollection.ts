
class GUICollection {

    event_blocker: EventBlocker
    content_comm: ContentComm
    page_overlays: PageOverlays


    wait_popup: WaitPopup
    text_input_popup: TextInputPopup
    select_input_popup: SelectInputPopup

    page_wrap: PageWrap

    private distinct_colors: DistinctColors
    overlay_sets: OverlaySets

    constructor(content_comm: ContentComm) {
        this.content_comm = content_comm

        // Our popups.
        this.wait_popup = new WaitPopup()
        this.text_input_popup = new TextInputPopup()
        this.select_input_popup = new SelectInputPopup()

        // Our overlay sets.
        this.distinct_colors = new DistinctColors()
        this.overlay_sets = new OverlaySets()

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
        if (!this.text_input_popup.is_initialized) {
            this.text_input_popup.initialize()
        }
        if (!this.select_input_popup.is_initialized) {
            this.select_input_popup.initialize()
        }
    }


    contains_element(element: Node): boolean {
        if (this.wait_popup.contains_element(element)) {
            return true
        }
        if (this.text_input_popup.contains_element(element)) {
            return true
        }
        if (this.select_input_popup.contains_element(element)) {
            return true
        }
        return false
    }

    add_overlay_set(elem_wraps: ElemWrap[]): void {
        let os: OverlaySet = new OverlaySet(elem_wraps, this.distinct_colors)
        this.overlay_sets.add_set(os)
    }

    on_context_menu(e: MouseEvent): boolean {
        if (!this.page_overlays.initialized()) {
            return
        }

        // Get the click position in global client space.
        let global_client_click = new Point({ x: e.clientX, y: e.clientY })
        console.log('contextmenu local client click: ' + global_client_click.to_string())
        global_client_click.to_global_client_space(window)
        console.log('contextmenu global client click: ' + global_client_click.to_string())

        // Update the click box overly with local page space coordinates.
        let local_page_click = new Point({ x: e.pageX, y: e.pageY })
        this.page_overlays.update_crosshair(local_page_click)
        console.log('contextmenu local page click: ' + local_page_click.to_string())

        // Send the request to the app.
        let im = new InfoMessage(0, InfoType.kShowWebActionMenu, { click_pos: global_client_click })
        this.content_comm.send_to_bg(im)

        // Prevent default behavior of the event.
        e.preventDefault();
        return false
    }

    on_mouse_over(e: MouseEvent): void {
        let point = new Point({ x: e.pageX, y: e.pageY })
        let text_elem_wrap = this.page_wrap.get_top_text_elem_wrap_at(point)
        let image_elem_wrap = this.page_wrap.get_top_image_elem_wrap_at(point)
        this.page_overlays.update_overlays(text_elem_wrap, image_elem_wrap)
    }

    get_crosshair_info(global_client_click: Point): any {
        let local_page_click = new Point(global_client_click)
        local_page_click.to_local_client_space(window)
        local_page_click.to_page_space(window)

        // Get the text and image values.
        let elem_wraps = this.page_wrap.get_visible_overlapping_at(local_page_click)
        let text_values = PageWrap.get_text_values_at(elem_wraps, local_page_click)
        let image_values = PageWrap.get_image_values_at(elem_wraps, local_page_click)

        // This is currently not getting used. But is useful for debugging.
        // Get the click pos relative to the topmost/innermost/nearest element in the stack of elements
        // under the click point. Note that this element may not be in an overlay set yet. 
        let nearest_rel_click_pos = new Point({ x: 0, y: 0 })
        if (elem_wraps.length > 0) {
            nearest_rel_click_pos = elem_wraps[0].get_box().get_relative_point(local_page_click)
        }

        // Determine the set index and overlay index at the click point.
        // Also determine the click pos relative to this overlay.
        let set_overlay_index = this.overlay_sets.find_set_overlay_index(local_page_click)
        let set_index = set_overlay_index.set_index
        let overlay_index = set_overlay_index.overlay_index
        let overlay_rel_click_pos: Point = new Point({ x: 1, y: 1 })
        if (set_index >= 0) {
            let oset = this.overlay_sets.get_set(set_index)
            overlay_rel_click_pos = oset.get_overlay(overlay_index).get_elem_wrap().get_box().get_relative_point(local_page_click)
        }

        // If we're a select element, grab the option values and texts.
        let option_values: string[] = []
        let option_texts: string[] = []
        if (set_index >= 0) {
            let oset = this.overlay_sets.get_set(set_index)
            let element = oset.get_overlay(overlay_index).get_elem_wrap().get_element()
            if (element instanceof HTMLSelectElement) {
                let select: HTMLSelectElement = <HTMLSelectElement>element
                for (let i = 0; i < element.options.length; i++) {
                    let option = <HTMLOptionElement>(element.options[i])
                    option_values.push(option.value)
                    option_texts.push(option.text)
                    console.log('option value,text: ' + option.value + "," + option.text)
                }
            }
        }

        let args = {
            // Click pos.
            click_pos: global_client_click,
            nearest_rel_click_pos: nearest_rel_click_pos,
            overlay_rel_click_pos: overlay_rel_click_pos,
            // Text and image values under click.
            text_values: text_values,
            image_values: image_values,
            // Existing set element under click.
            set_index: set_index,
            overlay_index: overlay_index,
            // Select/Dropdown option texts.
            option_texts: option_texts,
        }
        return args
    }

}