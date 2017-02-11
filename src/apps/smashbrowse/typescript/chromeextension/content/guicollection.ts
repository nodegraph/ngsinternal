
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

    on_context_menu(e: MouseEvent): boolean {
        if (!this.page_overlays.initialized()) {
            return
        }

        // Get the click position in global client space.
        let global_client_click = new Point({ x: e.clientX, y: e.clientY })
        global_client_click.to_global_client_space(window)

        // Update the click box overly with local page space coordinates.
        let local_page_click = new Point({ x: e.pageX, y: e.pageY })
        this.page_overlays.update_crosshair(local_page_click)

        // Send the request to the app.
        let im = new InfoMessage(0, InfoType.kShowWebActionMenu, { global_mouse_position: global_client_click })
        this.content_comm.send_to_bg(im)

        // Prevent default behavior of the event.
        e.preventDefault();
        return false
    }

    on_mouse_over(e: MouseEvent): void {
        let point = new Point({ x: e.pageX, y: e.pageY })
        let text_elem_wrap = this.page_wrap.get_top_elem_wrap_at(point, WrapType.text)
        let image_elem_wrap = this.page_wrap.get_top_elem_wrap_at(point, WrapType.image)
        this.page_overlays.update_overlays(text_elem_wrap, image_elem_wrap)
    }

    get_drop_down_info(): IDropDownInfo {
        // If we're a select element, grab the option values and texts.
        let option_values: string[] = []
        let option_texts: string[] = []
        let elem_wrap = this.page_overlays.get_elem_wrap()
        if (elem_wrap) {
            let element = elem_wrap.get_element()
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
        return { option_values: option_values, option_texts: option_texts }
    }

    get_crosshair_info(global_client_click: Point): IClickInfo {
        let local_page_click = new Point(global_client_click)
        local_page_click.to_local_client_space(window)
        local_page_click.to_page_space(window)

        // Get the text and image values.
        let elem_wraps = this.page_wrap.get_visible_overlapping_at(local_page_click)
        let text_values = PageWrap.get_text_values_at(elem_wraps, local_page_click)
        let image_values = PageWrap.get_image_values_at(elem_wraps, local_page_click)

        // PageWrap.get_visible_overlapping_at(..) should be returning the elements in paint order.
        // So we take the first one.
        if (elem_wraps.length == 0) {
            return null
        }

        let elem = elem_wraps[0]
        let xpath = elem.get_xpath()
        let local_mouse_position = elem.get_box().get_relative_point(local_page_click)

        let args: IClickInfo = {
            frame_index_path: PageWrap.get_frame_index_path(window),
            xpath: xpath,
            // Click pos.
            global_mouse_position: global_client_click,
            local_mouse_position: local_mouse_position,
            // Text and image values under click.
            text_values: text_values,
            image_values: image_values,
        }
        return args
    }

}