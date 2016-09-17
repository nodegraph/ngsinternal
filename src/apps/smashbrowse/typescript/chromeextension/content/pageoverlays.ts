

class PageOverlays {
    // Our Dependencies.
    
    
    // Our Members.

    // Our overlays. 
    // The user will never be able to click these with the mouse as the line width is zero.
    // They are being drawn using an outline feature.
    text_box_overlay: Overlay
    image_box_overlay: Overlay
    click_cross_overlay: CrossOverlay

    // The click point at which the crosshairs will be overlayed.
    page_pos: Point
    
    constructor() {
        // Dependencies.
        
        // Our Members.

        // Our Overlays.
        this.text_box_overlay = null
        this.image_box_overlay = null
        this.click_cross_overlay = null
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
        this.text_box_overlay = new Overlay('smash_browse_text_box', DistinctColors.text_color, -1, false, null)
        this.image_box_overlay = new Overlay('smash_browse_image_box', DistinctColors.image_color, -1, false, null)
        this.click_cross_overlay = new CrossOverlay('smash_browse_text_box', DistinctColors.text_color, -1)
    }

    update_overlays(text_elem_wrap: ElemWrap, image_elem_wrap: ElemWrap): void {
        if (!this.text_box_overlay || !this.image_box_overlay) {
            return
        }
        this.update_text_box_overlay(text_elem_wrap)
        this.update_image_box_overlay(image_elem_wrap)
    }

    private update_text_box_overlay(elem_wrap: ElemWrap): void {
        this.text_box_overlay.elem_wrap = elem_wrap
        this.text_box_overlay.update(DistinctColors.text_color, -1)
    }

    private update_image_box_overlay(elem_wrap: ElemWrap): void {
        this.image_box_overlay.elem_wrap = elem_wrap
        this.image_box_overlay.update(DistinctColors.image_color, -1)
    }

    update_crosshair(pos: Point): void {
        this.click_cross_overlay.update_dom_elements(pos)
    }

}





