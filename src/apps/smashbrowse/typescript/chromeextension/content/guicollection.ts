
class GUICollection {

    event_blocker: EventBlocker

    context_menu: ContextMenu

    wait_popup: WaitPopup
    text_input_popup: TextInputPopup
    select_input_popup: SelectInputPopup

    page_wrap: PageWrap

    distinct_colors: DistinctColors
    overlay_sets: OverlaySets

    constructor() {
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
        this.context_menu = new ContextMenu()

        // Our event blocker.
        this.event_blocker = new EventBlocker(this)
    }

    initialize() {
        if (!this.context_menu.initialized()) {
            this.context_menu.initialize()
        }
        if (!this.wait_popup.initialized) {
         this.wait_popup.initialize()
        }
        if (!this.text_input_popup.initialized) {
            this.text_input_popup.initialize()
        }
        if (!this.select_input_popup.initialized) {
            this.select_input_popup.initialize()
        }
    }

    
    contains_element(element: Node): boolean {
        if (this.context_menu.contains_element(element)) {
            return true
        }
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

    hide_context_menu(): void {
        if (this.context_menu.visible) {
            this.context_menu.hide()
        }
    }

    on_context_menu(e: MouseEvent): boolean {
        if (!this.context_menu.initialized()) {
            return
        }
        let p = new Point({x: e.pageX, y: e.pageY})
        let text_values = this.page_wrap.get_text_values_at(p)
        let image_values = this.page_wrap.get_image_values_at(p)
        return this.context_menu.on_context_menu(e, text_values, image_values)
    }

    on_click(e: MouseEvent): void {
        this.context_menu.on_click(e)
    }

    on_mouse_over(e: MouseEvent): void {
        let point = new Point({x: e.pageX, y: e.pageY})
        let text_elem_wrap = this.page_wrap.get_top_text_elem_wrap_at(point)
        let image_elem_wrap = this.page_wrap.get_top_image_elem_wrap_at(point)
        this.context_menu.on_mouse_over(text_elem_wrap, image_elem_wrap)
    }
}