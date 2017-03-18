
// This class wraps the dom element with extra functionality.
class ElemWrap {
    // Our members.
    protected element: Element // The actual dom element.
    protected page_box: Box // Our bounds in page space.
    protected valid: boolean // We become invalid once element disappears.

    static image_getter: (wrap: ElemWrap) => string
    static text_getter: (wrap: ElemWrap) => string
    static input_getter: (wrap: ElemWrap) => string
    static select_getter: (wrap: ElemWrap) => string
    
    // Constructor.
    constructor(element: Element) {
        this.element = element
        this.page_box = new Box({left: 0, right: 0, top: 0, bottom: 0})
        this.valid = true

        // Functors used in filtering.
        ElemWrap.image_getter = (wrap: ElemWrap)=> {return wrap.get_image()}
        ElemWrap.text_getter = (wrap: ElemWrap)=> {return wrap.get_text()}
        ElemWrap.input_getter = (wrap: ElemWrap)=> {if (wrap.get_tag_name() == 'input') {return 'input'} return ''}
        ElemWrap.select_getter = (wrap: ElemWrap)=> {if (wrap.get_tag_name() == 'select') {return 'select'} return ''}

        // Update.
        this.update()
    }

    // Returns an unique opaque pointer for the purposes of identification.
    // When two ElemWraps return the same id, they represent a wrapper around the same dom element.
    get_id() {
        return this.element
    }

    get_element() {
        return this.element
    }

    get_box() {
        return this.page_box
    }

    is_valid() {
        return this.valid
    }

    // Update all internal state.
    update(): void {
        if (document.body.contains(this.element)) {
            this.update_page_box()
        } else {
            this.valid = false
            this.page_box.reset()
        }
    }

    // Returns true if this and the other ElemWrap represent the same dom element.
    equals(other: ElemWrap) {
        if (this.get_id() == other.get_id()) {
            return true
        }
        return false
    }

    // Get our parenting ElemWrap.
    get_parent(): ElemWrap {
        if (!this.element.parentElement) {
            return null
        }
        return new ElemWrap(this.element.parentElement)
    }



    //----------------------------------------------------------------------------------------
    // Geometry Related.
    //----------------------------------------------------------------------------------------

    private update_page_box() {
        let elem_rect = this.element.getBoundingClientRect()
        this.page_box.left = elem_rect.left
        this.page_box.right = elem_rect.right
        this.page_box.top = elem_rect.top
        this.page_box.bottom = elem_rect.bottom
        this.page_box.to_page_space(window)
    }

    //Returns true if the outer element contains the inner element.
    contains(elem_wrap: ElemWrap): boolean {
        return this.page_box.contains(elem_wrap.page_box)
    }

    intersects(elem_wrap: ElemWrap): boolean {
        return this.page_box.intersects(elem_wrap.page_box)
    }

    //Note this containment test uses a sigma of 1.0.
    contains_point (page_pos: Point): boolean {
        return this.page_box.contains_point(page_pos)
    }

    get_horizontal_overlap(elem_wrap: ElemWrap): number {
        let left = Math.max(this.page_box.left, elem_wrap.page_box.left)
        let right = Math.min(this.page_box.right, elem_wrap.page_box.right)
        return Math.max(0, right - left)
    }

    get_vertical_overlap(elem_wrap: ElemWrap): number {
        let top = Math.max(this.page_box.top, elem_wrap.page_box.top)
        let bottom = Math.min(this.page_box.bottom, elem_wrap.page_box.bottom)
        return Math.max(0, bottom - top)
    }

    // Returns false if the element is totally out of screen bounds.
    // Returns true when the element is partially or completely in bounds
    in_screen_bounds(): boolean {
        const e_rect = this.element.getBoundingClientRect();
        let v = this.get_closest_scroll(true)
        let h = this.get_closest_scroll(false)
        const v_rect = v.element.getBoundingClientRect();
        const h_rect = h.element.getBoundingClientRect();

        if (v_rect) {
            if (e_rect.bottom < v_rect.top) {
                return false
            } 
            if (e_rect.top >= v_rect.bottom) {
                return false
            }
            if (e_rect.right < v_rect.left) {
                return false
            }
            if (e_rect.left >= v_rect.right) {
                return false
            }
        }

        if (h_rect) {
            if (e_rect.bottom < h_rect.top) {
                return false
            } 
            if (e_rect.top >= h_rect.bottom) {
                return false
            }
            if (e_rect.right < h_rect.left) {
                return false
            }
            if (e_rect.left >= h_rect.right) {
                return false
            }
        }
        return true
    }

    is_visible(): boolean {
        if (this.element instanceof HTMLElement) {
            let he = <HTMLElement>this.element
            if ((he.offsetWidth == 0) || (he.offsetHeight == 0)) {
                return false
            }
        }
        if (this.page_box.get_width() == 0) {
            return false
        }
        if (this.page_box.get_height() == 0) {
            return false
        }

        // If the :before and :after pseudo element and the element itself is not visible,
        // then we are are not visible.

        // Check the :before pseudo element first.
        let before_is_visible = true
        let before_style = window.getComputedStyle(this.element, ':before');
        if (before_style.visibility == "hidden" || (before_style.display == 'none')) {
            before_is_visible = false
        }

        // Check the :after pseudo element first.
        let after_is_visible = true
        let after_style = window.getComputedStyle(this.element, ':after');
        if (after_style.visibility == "hidden" || (after_style.display == 'none')) {
            after_is_visible = false
        }

        // Now check the actual element.
        let main_is_visible = true
        let style = window.getComputedStyle(this.element, null)
        if (style.visibility != "visible") {
            main_is_visible = false
        }

        // If all 3 are not visible then we're not visible.
        if (!before_is_visible && !after_is_visible && !main_is_visible) {
            return false
        }

        return true
    }

    get_z_index(): number {
        let style = window.getComputedStyle(this.element, null)
        return Number(style.zIndex)
    }


    //----------------------------------------------------------------------------------------
    //Element Identification. (This is usually only valid during one browsing session.)
    //----------------------------------------------------------------------------------------

    //Get the full xpath for an element, anchored at the html document.
    //For simplicity each path element always has a [] index suffix.
    get_xpath(): string {
        let path: string[] = [];
        let element: Element = this.element
        while (element) {
            if (element.parentNode) {
                let node_name: string = element.nodeName;
                let sibling: Element = element
                let count: number = 0;
                while (sibling) {
                    if (sibling.nodeName == node_name) {
                        ++count;
                    }
                    sibling = sibling.previousElementSibling
                }
                if (node_name == 'svg') {
                    node_name = "*[local-name() = 'svg']"
                }
                path.unshift(node_name + '[' + count + ']')
            }
            element = <Element>element.parentNode
        }
        return '/' + path.join('/');
    }

    get_info(): IElementInfo {
        // Bounds.
        let box = new Box(this.get_box()) // This should be in page space.
        box.to_client_space(window)
        box.add_offset(PageWrap.get_offset()) // convert local to global client space

        let options = this.get_options()

        // Form the info.
        return {
            fw_index_path: PageWrap.get_fw_index_path(window),
            fe_index_path: PageWrap.fe_index_path,
            xpath: this.get_xpath(),
            box: box,
            tag_name: this.get_tag_name(),
            href: this.get_anchor_url(),
            image: this.get_image(),
            text: this.get_text(),
            input: this.get_input(),
            option_values: options.option_values,
            option_texts: options.option_texts
        }
    }



    //----------------------------------------------------------------------------------------
    // Scroll Bars.
    //----------------------------------------------------------------------------------------

    scroll_into_view(): void {
        // I asseume this call works with nested frames and scrollbars.
        this.element.scrollIntoView(false)
    }

    //Returns true if the element has horizontal scroll bars.
    has_horizontal_scroll_bar(): boolean {
        if (!(this.element.scrollWidth > this.element.clientWidth)) {
            return false
        }

        // Check the style.
        let style = window.getComputedStyle(this.element, null);
        if (style.overflow == "scroll" || style.overflow == "auto" ||
            style.overflowX == "scroll" || style.overflowX == "auto") {
            return true
        }
        return false
    }

    //Returns true if the element has vertical scroll bars.
    has_vertical_scroll_bar(): boolean {
        if (!(this.element.scrollHeight > this.element.clientHeight)) {
            return false
        }

        // Check the style.
        let style = window.getComputedStyle(this.element, null);
        if (style.overflow == "scroll" || style.overflow == "auto" ||
            style.overflowY == "scroll" || style.overflowY == "auto") {
            return true
        }
        return false
    }

    has_any_scroll_bar(): boolean {
        return this.has_vertical_scroll_bar() || this.has_horizontal_scroll_bar()
    }

    //Returns a parent elem wrap with scrollbars, if one exists.
    //Otherwise returns null.
    get_closest_scroll(vertical: boolean): ElemWrap {
        let parent: ElemWrap = this
        while (parent) {
            // The topmost scrollbar will be the document.body.
            // We need to record the scroll on the document.body as well.
            if (parent.element == document.body) {
                console.log('found closest scroll1: ' + parent)
                return parent
            }
            // If we find any scroll bars, add it.
            if ((vertical && parent.has_vertical_scroll_bar()) ||
                (!vertical && parent.has_horizontal_scroll_bar())) {
                console.log('found closest scroll2: ' + parent)
                return parent
            }
            // Check our next parent.
            parent = parent.get_parent()
        }
        console.log('error did no find closest scroll: ' + parent)
        return null
    }

    get_vertical_scroll_amount(): number {
        // We try to scroll by one page.
        let amount = this.element.clientHeight
        if (this.element == document.body) {
            amount = window.innerHeight
        }
        return amount
    }

    get_horizontal_scroll_amount(): number {
        // We try to scroll by one page.
        let amount = this.element.clientWidth
        if (this.element == document.body) {
            amount = window.innerWidth
        }
        return amount
    }

    // Tries to scroll down by one page.
    scroll_down(): void {
        this.element.scrollTop += this.get_vertical_scroll_amount()
    }

    // Tries to scroll up by one page.
    scroll_up(): void {
        this.element.scrollTop -= this.get_vertical_scroll_amount()
    }

    // Tries to scroll right by one page.
    scroll_right(): void {
        this.element.scrollLeft += this.get_horizontal_scroll_amount()
    }

    // Tries to scroll left by one page.
    scroll_left(): void {
        this.element.scrollLeft -= this.get_horizontal_scroll_amount()
    }

    //----------------------------------------------------------------------------------------
    // Important Element Values.
    //----------------------------------------------------------------------------------------

    get_tag_name(): string {
        return this.element.tagName.toLowerCase()
    }

    get_select(): string {
        if (this.get_tag_name() === 'select') {
            let s = <HTMLSelectElement>this.element
            return s.options[s.selectedIndex].textContent
        }
        return ''
    }


    get_anchor_url(): string {
        let e = this.element
        while (!(e instanceof HTMLAnchorElement)) {
            if (!e.parentElement) {
                return ''
            }
            e = e.parentElement
        }

        let a = <HTMLAnchorElement>e
        return a.href
    }

    //Retrieves the image value directly on an element in the dom hierarchy.
    //Note that there may multiple images (ie overlapping image) however they
    //will always be returned as one string from this function.
    get_image(): string {
        // Get the tag name.
        let tag_name = this.element.tagName.toLowerCase()

        // Return the src for images and video elements.
        if (tag_name === 'img') {
            return (<HTMLImageElement>this.element).src
        }

        if (tag_name === 'video') {
            return (<HTMLVideoElement>this.element).src
        }

        if (tag_name === 'source') {
            return (<HTMLSourceElement>this.element).src
        }

        // We serialize the svg data, but note that it is shallow and doesn't expand
        // <use> elements in the svg.
        if (tag_name === 'svg') {
            let serializer = new XMLSerializer();
            return serializer.serializeToString(this.element);
        }

        function get_image_from_style(style: CSSStyleDeclaration) {
            if (style.backgroundImage.indexOf('url(') == 0) {
                let background_image = style.backgroundImage.slice(4, -1);
                if (background_image != "") {
                    // Add on the image scaling and offset, as some images are used like packed textures.
                    let scaling = style.backgroundSize
                    let offset = style.backgroundPosition
                    return background_image + "::" + scaling + "::" + offset
                }
            }
            return ''
        }

        // Otherwise we query the computed style for the background image.
        let after_style: CSSStyleDeclaration = window.getComputedStyle(this.element, ':after');
        let style: CSSStyleDeclaration = window.getComputedStyle(this.element, null)
        let result = ''
        // Check the :after pseudo element first.
        if (after_style) {
            result = get_image_from_style(after_style)
        }
        // If it has no image, then check the actual element.
        if (!result) {
            if (style) {
                result = get_image_from_style(style)
            }
        }
        return result
    }

    // Retrieves the text value from children directly under this element in the dom hierarchy.
    // The text from all immediate children will be concatenated.
    get_text() {
        let text = ''

        // Loop through children accumulating text node values.
        for (let c = 0; c < this.element.childNodes.length; c++) {
            let child = this.element.childNodes[c]
            if (child.nodeType == Node.TEXT_NODE) {
                if (child instanceof Attr) {
                    let attr: Attr = <Attr>(child)
                    if (attr.name.toLowerCase() == 'href') {
                        continue
                    }
                }
                let value = child.nodeValue
                value = value.trim()
                // Add text if it's not all whitespace.
                if (!Utils.is_all_whitespace(value)) {
                    text += value
                }
            }
        }

        let before_style = window.getComputedStyle(this.element, ':before');
        let value: string = before_style.content
        value = Utils.strip_quotes(value)
        if (!Utils.is_all_whitespace(value)) {
            text = value + text
        }

        let after_style = window.getComputedStyle(this.element, ':after');
        value = after_style.content
        value = Utils.strip_quotes(value)
        if (!Utils.is_all_whitespace(value)) {
            text += value
        }
        return text
    }

    get_input() {
        let text = ''
        if (this.element.tagName.toLowerCase() == 'input') {
           let value = this.element.getAttribute('value')
           if (!Utils.is_all_whitespace(value)) {
               text += value
           }
        }
        return text
    }

    get_options(): IDropDownInfo {
        // The option values and texts of the select dropdown.
        let option_values: string[] = []
        let option_texts: string[] = []

        // Extract the option values and texts from the select element.
        let element = this.get_element()
        if (element instanceof HTMLSelectElement) {
            let select: HTMLSelectElement = <HTMLSelectElement>element
            for (let i = 0; i < element.options.length; i++) {
                let option = <HTMLOptionElement>(element.options[i])
                option_values.push(option.value)
                option_texts.push(option.text)
                //console.log('option value,text: ' + option.value + "," + option.text)
            }
        }
        return { option_values: option_values, option_texts: option_texts }
    }

    //----------------------------------------------------------------------------------------
    // Experimental Property Getters.
    //----------------------------------------------------------------------------------------

    //Retrieves the opacity value directly on an element in the dom hierarchy.
    get_opacity(): string {
        //  // Check the :after pseudo element first.
        //  let after_style = window.getComputedStyle(element, ':after')
        //  if (after_style.opacity) {
        //      return after_style.opacity
        //  }
        // Check the actual element.
        let style = window.getComputedStyle(this.element, null)
        if (style.opacity) {
            return style.opacity
        }
        return ""
    }

    get_background_color(): string {
        //    // Check the :after pseudo element first.
        //    let after_style = window.getComputedStyle(element, ':after')
        //    if (after_style.backgroundColor) {
        //        return after_style.backgroundColor
        //    }
        // Check the actual element.
        let style = window.getComputedStyle(this.element, null)
        if (style.backgroundColor) {
            return style.backgroundColor
        }
        return ""
    }

    //This is detects the back ground element of web pages and also popup divs.
    //It's heuristic is to detect fully opaque elements.
    //Note this is just a heuristic and may need changes in the future.
    is_back_plate(): boolean {
        let bg: string = this.get_background_color()
        if (bg == 'rgba(0, 0, 0, 0)') {
            return false
        }
        let opacity: number = Number(this.get_opacity())
        if (opacity != 1) {
            return false
        }
        if (bg.startsWith('rgba(')) {
            let sub = bg.slice(5, -1)
            let colors = sub.split(',')
            if (!colors[3].startsWith('1')) {
                return false
            }
        }
        return true
    }
}





