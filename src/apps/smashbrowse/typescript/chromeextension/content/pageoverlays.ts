

class PageOverlays {
    // Our Dependencies.
    
    
    // Our Members.

    // Our overlays. 
    // The user will never be able to click these with the mouse as the line width is zero.
    // They are being drawn using an outline feature.
    text_box_overlay: Overlay = null
    image_box_overlay: Overlay = null
    element_overlay: Overlay = null
    click_cross_overlay: CrossOverlay = null
    element_cross_overlay: CrossOverlay = null

    // The click point at which the crosshairs will be overlayed. This is in local page space.
    page_pos: Point
    
    constructor() {
    }

    get_page_pos(): Point {
        return this.page_pos
    }

    initialized(): boolean {
        if (this.click_cross_overlay) {
            return true
        }
        return false
    }

    initialize(): void {
        if (this.initialized()) {
            return
        }

        // Create our mouse overlays.
        this.text_box_overlay = new Overlay('smash_browse_text_box', DistinctColors.text_color, null)
        this.image_box_overlay = new Overlay('smash_browse_image_box', DistinctColors.image_color, null)
        this.click_cross_overlay = new CrossOverlay('smash_browse_text_box', DistinctColors.text_color)
        this.element_overlay = new Overlay('smash_browse_selected', "#00FF00", null)
        this.element_cross_overlay = new CrossOverlay('smash_browse_text_box', "#00FF00")
    }

    // Current element overlay.

    get_elem_wrap(): ElemWrap {
        return this.element_overlay.get_elem_wrap()
    }

    set_elem_wrap(elem_wrap: ElemWrap): void {
        this.element_overlay.set_elem_wrap(elem_wrap)
        this.update_element_overlay()
    }

    clear_elem_wrap(): void {
        this.element_overlay.clear_elem_wrap()
    }

    // ---------------------------------------------------------------------------------------------
    // Element overlay update.
    // ---------------------------------------------------------------------------------------------

    update_element_overlay(): void {
        this.element_overlay.update()
        let elem_wrap = this.element_overlay.get_elem_wrap()
        if (elem_wrap) {
            const point = elem_wrap.get_box().get_center()
            this.element_cross_overlay.update_dom_elements(point)
        }
    }

    // ---------------------------------------------------------------------------------------------
    // Text and Image overlay update.
    // ---------------------------------------------------------------------------------------------

    update_overlays(text_elem_wrap: ElemWrap, image_elem_wrap: ElemWrap): void {
        if (!this.text_box_overlay || !this.image_box_overlay) {
            return
        }
        this.update_text_box_overlay(text_elem_wrap)
        this.update_image_box_overlay(image_elem_wrap)
    }

    private update_text_box_overlay(elem_wrap: ElemWrap): void {
        this.text_box_overlay.set_elem_wrap(elem_wrap)
        this.text_box_overlay.update()
    }

    private update_image_box_overlay(elem_wrap: ElemWrap): void {
        this.image_box_overlay.set_elem_wrap(elem_wrap)
        this.image_box_overlay.update()
    }

    // ---------------------------------------------------------------------------------------------
    // Crosshair overlay update.
    // ---------------------------------------------------------------------------------------------

    update_crosshair(page_pos: Point): void {
        this.click_cross_overlay.update_dom_elements(page_pos)
    }

}





