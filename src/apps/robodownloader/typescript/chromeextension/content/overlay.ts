
// This class represents the rectangular overlays that we see overlayed on
// the target web page. These boxes ares used to represent what elements 
// are currently under the mouse as well as what elements have already been
// selected by the user.

class Overlay {
    // The thickness of the edges of the box overlay.
    static thickness = 0
    // Amount to oversize the box around the dom element.
    static enlarge = 0
    
    // Our dom elements.
    private left: HTMLDivElement
    private right: HTMLDivElement
    private top: HTMLDivElement
    private bottom: HTMLDivElement
    
    // Our ElemWrap.
    private elem_wrap: ElemWrap
    private color: string
    
    constructor(class_name: string, color: string,  elem_wrap: ElemWrap=null) {
        // Primary roperty.
        this.color = color
        this.elem_wrap = elem_wrap // Note the elem wrap is allowed to be null as well.

        // Our dom elements.
        this.left = null
        this.right = null
        this.top = null
        this.bottom = null

        // Setup.
        this.create_dom_elements(class_name)
        this.update_dom_elements(color)
    }

    // ElemWrap
    get_elem_wrap() {
        return this.elem_wrap
    }
    set_elem_wrap(elem_wrap: ElemWrap) {
        this.elem_wrap = elem_wrap
    }
    clear_elem_wrap() {
        this.elem_wrap = null
    }
    is_valid() {
        return this.elem_wrap.is_valid()
    }
    
    // Update all internal state.
    update(): void {
        if (this.elem_wrap) {
            this.elem_wrap.update()
        }
        this.update_dom_elements(this.color)
    }

    // Destroy our dom elements.
    destroy(): void {
        // Release the elem_wrap
        this.elem_wrap = null
        // Destroy the dom elments.
        document.body.removeChild(this.left)
        document.body.removeChild(this.right)
        document.body.removeChild(this.top)
        document.body.removeChild(this.bottom)
        // Set our dom references to null.
        this.left = null
        this.right = null
        this.top = null
        this.bottom = null
    }

    contains_point(page_pos: Point): boolean {
        return this.elem_wrap.contains_point(page_pos)
    }

    //----------------------------------------------------------------------------------
    // Private methods.
    //----------------------------------------------------------------------------------

    // Creates dom elements representing the four edges of the box overlay.
    create_dom_elements(class_name: string): void {
        this.left = document.createElement("div")
        this.left.classList.add(class_name)
        this.left.style.position = "absolute"
        document.body.appendChild(this.left)

        this.right = document.createElement("div")
        this.right.classList.add(class_name)
        this.right.style.position = "absolute"
        document.body.appendChild(this.right)

        this.top = document.createElement("div")
        this.top.classList.add(class_name)
        this.top.style.position = "absolute"
        document.body.appendChild(this.top)

        this.bottom = document.createElement("div")
        this.bottom.classList.add(class_name)
        this.bottom.style.position = "absolute"
        document.body.appendChild(this.bottom)
    }

    // Updates the dom elements to reflect new position and size.
    update_dom_elements(color: string): void {
        if (!this.elem_wrap) {
            this.left.style.left = '-10px'
            this.left.style.top = '-10px'
            this.left.style.width = '0px'
            this.left.style.height = '0px'

            this.right.style.left = '-10px'
            this.right.style.top = '-10px'
            this.right.style.width = '0px'
            this.right.style.height = '0px'

            this.top.style.left = '-10px'
            this.top.style.top = '-10px'
            this.top.style.width = '0px'
            this.top.style.height = '0px'

            this.bottom.style.left = '-10px'
            this.bottom.style.top = '-10px'
            this.bottom.style.width = '0px'
            this.bottom.style.height = '0px'
            return
        }

        // Otherwise if we have an elem wra    
        // Set the color of the dom elements.
        this.left.style.outlineColor = color
        this.right.style.outlineColor = color
        this.top.style.outlineColor = color
        this.bottom.style.outlineColor = color

        // Set the geometry of the dom elements.
        let page_box = this.elem_wrap.get_box()
        let width = page_box.right - page_box.left
        let height = page_box.bottom - page_box.top
        let t = Overlay.thickness

        // Left.
        this.left.style.left = (page_box.left - Overlay.enlarge - t) + 'px'
        this.left.style.top = (page_box.top - Overlay.enlarge - t) + 'px'
        this.left.style.width = t + 'px'
        this.left.style.height = (height + 2 * Overlay.enlarge + 2 * t) + 'px'

        // Right.
        this.right.style.left = (page_box.right + Overlay.enlarge) + 'px'
        this.right.style.top = (page_box.top - Overlay.enlarge - t) + 'px'
        this.right.style.width = t + 'px'
        this.right.style.height = (height + 2 * Overlay.enlarge + 2 * t) + 'px'

        // Top.
        this.top.style.left = (page_box.left - Overlay.enlarge - t) + 'px'
        this.top.style.top = (page_box.top - Overlay.enlarge - t) + 'px'
        this.top.style.width = (width + 2 * Overlay.enlarge + 2 * t) + 'px'
        this.top.style.height = t + 'px'

        // Bottom.
        this.bottom.style.left = (page_box.left - Overlay.enlarge - t) + 'px'
        this.bottom.style.top = page_box.bottom + Overlay.enlarge + 'px'
        this.bottom.style.width = (width + 2 * Overlay.enlarge + 2 * t) + 'px'
        this.bottom.style.height = t + 'px'
    }

}








