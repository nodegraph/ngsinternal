
// This class represents the rectangular overlays that we see overlayed on
// the target web page. These boxes ares used to represent what elements 
// are currently under the mouse as well as what elements have already been
// selected by the user.

class CrossOverlay {
    // Color.
    color: string
    color_index: number

    // Visual properties of the cross.
    bar_length: number
    thickness: number

    // Position.
    page_pos: Point // The position in page space. (Not client space)
    
    // Our dom elements.
    horizontal: HTMLDivElement
    vertical: HTMLDivElement
    
    constructor(class_name: string, color: string, color_index: number) {
        // Our dom elements.
        this.horizontal = null
        this.vertical = null

        // Our color.
        this.color = color
        this.color_index = color_index

        // Physical properties.
        this.bar_length = 50
        this.thickness = 0

        // Setup.
        this.create_dom_elements(class_name)
        this.update_dom_elements(new Point({x: 0,y: 0}))
    }
    
    //Destroy our dom elements.
    destroy(): void {
        // Destroy the dom elments.
        document.body.removeChild(this.horizontal)
        document.body.removeChild(this.vertical)
        // Set our dom references to null.
        this.horizontal = null
        this.vertical = null
    }

    // ----------------------------------------------------------------------------------
    // Private methods.
    //----------------------------------------------------------------------------------

    //Creates dom elements representing the four edges of the box overlay.
    create_dom_elements(class_name: string): void {
        this.horizontal = document.createElement("div")
        this.horizontal.classList.add(class_name)
        this.horizontal.style.position = "absolute"
        document.body.appendChild(this.horizontal)

        this.vertical = document.createElement("div")
        this.vertical.classList.add(class_name)
        this.vertical.style.position = "absolute"
        document.body.appendChild(this.vertical)

        this.horizontal.style.left = '0px'
        this.horizontal.style.top = '0px'
        this.horizontal.style.width = '0px'
        this.horizontal.style.height = '0px'

        this.vertical.style.left = '0px'
        this.vertical.style.top = '0px'
        this.vertical.style.width = '0px'
        this.vertical.style.height = '0px'
    }

    //Updates the dom elements to reflect new position and size.
    update_dom_elements(page_pos: Point): void {
        this.page_pos = page_pos

        // Set the color of the dom elements.
        this.horizontal.style.outlineColor = this.color
        this.vertical.style.outlineColor = this.color

        const half_size = this.bar_length / 2.0
        const half_thickness = this.thickness / 2.0

        // Left.
        this.horizontal.style.left = (this.page_pos.x - half_size) + 'px'
        this.horizontal.style.top = (this.page_pos.y - half_thickness) + 'px'
        this.horizontal.style.width = this.bar_length + 'px'
        this.horizontal.style.height = this.thickness + 'px'

        console.log('wwwwwwwwwwwwwwwww: ' + this.bar_length + 'px')

        // Right.
        this.vertical.style.left = (this.page_pos.x + half_thickness) + 'px'
        this.vertical.style.top = (this.page_pos.y - half_size) + 'px'
        this.vertical.style.width = this.thickness + 'px'
        this.vertical.style.height = this.bar_length + 'px'
    }

}








