
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
        global_client_click = global_client_click.add(PageWrap.get_offset()) // convert local to global client space

        // Update the click box overly with local page space coordinates.
        let local_page_click = new Point({ x: e.pageX, y: e.pageY })
        this.page_overlays.update_crosshair(local_page_click)

        // Send the request to the app.
        let smallest = PageWrap.get_smallest_element(local_page_click)
        let im = new InfoMessage(0, InfoType.kShowWebActionMenu, smallest.get_info())
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
                    //console.log('option value,text: ' + option.value + "," + option.text)
                }
            }
        }
        return { option_values: option_values, option_texts: option_texts }
    }

    get_crosshair_info(global_client_click: Point): IClickInfo {
        let local_page_click = new Point(global_client_click)
        local_page_click = local_page_click.subtract(PageWrap.get_offset()) // convert global to local client space
        local_page_click.to_page_space(window)

        // Get the text and image values.
        let smallest = PageWrap.get_smallest_element(local_page_click)
        if (!smallest) {
            return {
                fw_index_path: '',
                fe_index_path: '',
                xpath: '',
                box: {left: 0, right: 0, top: 0, bottom: 0},
                tag_name: '',
                text: '',
                image: '',
                href: '',
                global_mouse_position: {x: 0, y: 0},
                local_mouse_position: {x: 0, y: 0},
            }
        }

        let xpath = smallest.get_xpath()
        let local_mouse_position = smallest.get_box().get_relative_point(local_page_click)
        let info = smallest.get_info()
        
        let click_info: IClickInfo = {
            fw_index_path: info.fw_index_path,
            fe_index_path: info.fe_index_path,
            xpath: info.xpath,
            box: info.box,
            tag_name: info.tag_name,
            text: info.text,
            image: info.image,
            href: info.href,
            global_mouse_position: global_client_click,
            local_mouse_position: local_mouse_position,
        }
        return click_info
    }

}