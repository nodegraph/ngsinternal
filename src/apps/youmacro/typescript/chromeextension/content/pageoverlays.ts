

class PageOverlays {
    // The user will never be able to click these with the mouse as the line width is zero.
    // They are being drawn using an outline feature.
    text_box_overlay: Overlay = null
    image_box_overlay: Overlay = null
    crosshair_overlay: CrossOverlay = null

    element_overlays: Overlay[] = []

    // The click point at which the crosshairs will be overlayed. This is in local page space.
    cross_hair_pos: Point
    
    constructor() {
    }

    get_page_pos(): Point {
        return this.cross_hair_pos
    }

    initialized(): boolean {
        if (this.crosshair_overlay) {
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
        this.crosshair_overlay = new CrossOverlay('smash_browse_text_box', DistinctColors.text_color)
    }

    // Disables overlays. Used when the browser has been released.
    uninitialize(): void {
        this.crosshair_overlay.destroy()
        this.crosshair_overlay = null

        this.text_box_overlay.destroy()
        this.text_box_overlay = null

        this.image_box_overlay.destroy()
        this.image_box_overlay = null
    }

    // ---------------------------------------------------------------------------------------------
    // Element overlays.
    // ---------------------------------------------------------------------------------------------

    create_element_overlays(infos: IElementInfo[]): void {
        for (let e of infos) {
            // Make sure the element is in our frame.
            if (e.fe_index_path != PageWrap.fe_index_path) {
                continue
            }
            // Get the elem wrap.
            let wraps = PageWrap.get_visible_by_xpath(e.xpath)
            if (wraps.length == 0) {
                continue
            }
            let wrap = wraps[0]
            // Create a new element overlay.
            this.element_overlays.push(new Overlay('smash_browse_selected', "#00FF00", wrap))
        }
    }

    clear_element_overlays(): void {
        for (let o of this.element_overlays) {
            o.destroy()
        }
        this.element_overlays.length = 0
    }

    update_element_overlays(): void {
        for (let o of this.element_overlays) {
            o.update()
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
        this.crosshair_overlay.update_dom_elements(page_pos)
    }

    clear_crosshair(): void {
        const point = new Point({x: -1, y:-1})
        this.crosshair_overlay.update_dom_elements(point)
    }

}





