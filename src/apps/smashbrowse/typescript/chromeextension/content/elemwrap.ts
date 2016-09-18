
//Wrap type.
const enum WrapType {
    text,
    image,
    input,
    select,
    iframe
}

// This class wraps the dom element with extra functionality.
class ElemWrap {
    // Our dependencies.
    
    // Our members.
    private element: Element // The actual dom element.
    private page_box: Box // Our bounds in page space.
    private wrap_type: WrapType // Our wrapping type.
    private getter: () => string // Cached values getter for our specific wrapping type.
    
    // Constructor.
    constructor(element: Element) {
        // Our Dependencies.
        
        // Our Members.
        this.element = element

        // Cached values, which need to be updated when the element changes.
        this.page_box = new Box()

        // Our wrap type and getter should stay fixed throughout our lifetime.
        this.wrap_type = this.calculate_wrap_type()
        this.getter = this.get_getter_from_wrap_type(this.wrap_type)

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

    get_wrap_type() {
        return this.wrap_type
    }

    get_box() {
        return this.page_box
    }

    get_getter(): () => string {
        return this.getter
    }

    get_frame_index_path(): number[] {
        let local_window: Window = this.element.ownerDocument.defaultView
        return PageWrap.get_iframe_index_path(local_window)
    }

    // Update all internal state.
    update(): void {
        this.update_page_box()
    }

    // Returns true if this and the other ElemWrap represent the same dom element.
    equals(other: ElemWrap) {
        if (this.get_id() == other.get_id()) {
            return true
        }
        return false
    }

    // Get our wrap type.
    calculate_wrap_type(): WrapType {
        if (this.is_iframe()) {
            return WrapType.iframe
        }
        if (this.get_text()) {
            return WrapType.text
        }
        if (this.get_image()) {
            return WrapType.image
        }
        if (this.is_input()) {
            return WrapType.input
        }
        if (this.is_select()) {
            return WrapType.select
        }
        return -1
    }

    get_getter_from_wrap_type(wrap_type: WrapType): () => string {
        // Note this returns functions that are not bound to this.
        // This means that these functions can be called by many instances.
        switch (wrap_type) {
            case WrapType.text:
                return this.get_text
            case WrapType.image:
                return this.get_image
            case WrapType.input:
                return this.is_input
            case WrapType.select:
                return this.is_select
            case WrapType.iframe:
                return this.is_iframe
        }
        return null
    }

    //----------------------------------------------------------------------------------------
    // Geometry Related.
    //----------------------------------------------------------------------------------------

    update_page_box() {
        let elem_rect = this.element.getBoundingClientRect()
        this.page_box.left = elem_rect.left
        this.page_box.right = elem_rect.right
        this.page_box.top = elem_rect.top
        this.page_box.bottom = elem_rect.bottom
        this.page_box.to_page_space()
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

        // Check the :before pseudo element first.
        let before_style = window.getComputedStyle(this.element, ':before');
        if (before_style.visibility == "hidden" || (before_style.display == 'none')) {
            return false
        }

        // Check the :after pseudo element first.
        let after_style = window.getComputedStyle(this.element, ':after');
        if (after_style.visibility == "hidden" || (after_style.display == 'none')) {
            return false
        }
        // Now check the actual element.
        let style = window.getComputedStyle(this.element, null)
        if (style.visibility != "visible") {
            return false
        }
        return true
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

    //----------------------------------------------------------------------------------------
    // Element Shifting.
    //----------------------------------------------------------------------------------------

    // Returns the next topmost element on one side of us.
    get_neighboring_elem_wrap(getter: () => string, side: Direction, page_wrap: PageWrap): ElemWrap {
        let beam = this.page_box.get_beam(side, PageWrap.get_bounds())

        // Get all elem wraps intersecting the beam.
        let elem_wraps: ElemWrap[] = page_wrap.get_intersecting_with(beam)

        // Select out those elem wraps returning any value with getter.
        let candidates: ElemWrap[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            if (elem_wraps[i].get_id() == this.get_id()) {
                continue
            }
            let value = getter.call(elem_wraps[i])
            if (value) {
                console.log('found element with value:-->' + value + "<--")
                candidates.push(elem_wraps[i])
            }
        }

        // Find the closest candidate to us.
        let min_dist: number = 99999999
        let min_candidate: ElemWrap = null
        for (let i = 0; i < candidates.length; i++) {
            let dist = candidates[i].page_box.is_oriented_on(side, this.page_box)
            // A dist greater than zero means it's properly oriented.
            if (dist > 0) {
                // Is this closer than any other candidate.
                if (!min_candidate || dist < min_dist) {
                    min_candidate = candidates[i]
                    min_dist = dist
                }
            }
        }

        // Return our result.
        console.log('min candidate: ' + getter.call(min_candidate))
        return min_candidate
    }

    shift(dir: Direction, wrap_type: WrapType, page_wrap: PageWrap): void {
        // Find the proper getter for the type we're shifting to.
        let getter: () => string = this.get_getter_from_wrap_type(wrap_type)
        let elem_wrap: ElemWrap = this.get_neighboring_elem_wrap(getter, dir, page_wrap)
        if (elem_wrap) {
            this.element = elem_wrap.element
        }
    }


    //Returns the next topmost element on one side of us.
    get_similar_neighbors(side: Direction, match_criteria: MatchCriteria, page_wrap: PageWrap): ElemWrap[] {
        let beam = this.page_box.get_beam(side, PageWrap.get_bounds())

        // Get all elem wraps intersecting the beam.
        let elem_wraps = page_wrap.get_intersecting_with(beam)
        //console.log('num elem wraps: ' + elem_wraps.length)

        // Select out those elem wraps returning any value with getter.
        let getter: () => string = this.get_getter()
        let candidates: ElemWrap[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = getter.call(elem_wraps[i])
            if (value) {
                candidates.push(elem_wraps[i])
            }
        }
        //console.log('num candidates: ' + candidates.length)

        // Find the candidates which match us.
        let matches: ElemWrap[] = []
        for (let i = 0; i < candidates.length; i++) {
            if (match_criteria.matches(this, candidates[i])) {
                matches.push(candidates[i])
            }
        }
        //console.log('num matches: ' + matches.length)

        // Return our result.
        return matches
    }


    //----------------------------------------------------------------------------------------
    // Scroll Bars.
    //----------------------------------------------------------------------------------------

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
    // Element Behavioral Properties.
    //----------------------------------------------------------------------------------------

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

    get_parent(): ElemWrap {
        if (!this.element.parentElement) {
            return null
        }
        return new ElemWrap(this.element.parentElement)
    }



    //----------------------------------------------------------------------------------------
    // Element Properties/Values.
    //----------------------------------------------------------------------------------------

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
        }

        // Otherwise we query the computed style for the background image.
        let after_style: CSSStyleDeclaration = window.getComputedStyle(this.element, ':after');
        let style: CSSStyleDeclaration = window.getComputedStyle(this.element, null)
        let result = ""
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

    //Retrieves the text value directly under an element in the dom hierarchy.
    //Note that there may be multiple texts (ie muliple paragraphs) however they
    //will always be returned as one string from this function.
    get_text(): string {
        let text = ""

        // Loop through children accumulating text node values.
        for (let c = 0; c < this.element.childNodes.length; c++) {
            let child = this.element.childNodes[c]
            if (child.nodeType == Node.TEXT_NODE) {
                if (child instanceof Attr) {
                    let attr: Attr = <Attr>(child)
                    console.log('text nodes attr name is: ' + attr.name)
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

        if (this.element.tagName.toLowerCase() == 'input') {
            let value = this.element.getAttribute('value')
            if (!Utils.is_all_whitespace(value)) {
                text += value
            }
        }

        let before_style = window.getComputedStyle(this.element, ':before');
        let value: string = before_style.content
        value = Utils.strip_quotes(value)
        if (!Utils.is_all_whitespace(value)) {
            text += value
        }

        let after_style = window.getComputedStyle(this.element, ':after');
        value = after_style.content
        value = Utils.strip_quotes(value)
        if (!Utils.is_all_whitespace(value)) {
            text += value
        }
        return text
    }

    get_text_or_image(): string {
        let text = this.get_text()
        if (text) {
            return text
        }
        let image = this.get_image()
        if (image) {
            return image
        }
    }

    get_tag_name(): string {
        return this.element.tagName.toLowerCase()
    }

    is_input(): string {
        if (this.element.tagName.toLowerCase() == 'input') {
            return 'input'
        }
        return ''
    }

    is_select(): string {
        if (this.element.tagName.toLowerCase() == 'select') {
            return 'select'
        }
        return ''
    }

    is_iframe(): string {
        if (this.element.tagName.toLowerCase() == 'iframe') {
            return 'iframe'
        }
        return ''
    }


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


}




