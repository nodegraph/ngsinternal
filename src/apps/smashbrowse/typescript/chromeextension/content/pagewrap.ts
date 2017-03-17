
interface Window { [key: string]: any } 

// The class encapsulates the properties of a web page.
class PageWrap {
    // Our Dependencies.
    private gui_collection: GUICollection // This is like our owning parent.

    // Our members.
    static local_to_global_offset: IPoint = {x: 0, y: 0}
    static fe_index_path: string
    
    // Constructor.
    constructor(gc: GUICollection) {
        this.gui_collection = gc
    }

    //---------------------------------------------------------------------------------
    // Page Properties.
    //---------------------------------------------------------------------------------

    static get_offset() {
        return PageWrap.local_to_global_offset
    }

    // Returns the page height.
    static get_height() {
        let doc_body = document.body
        let doc_elem = document.documentElement
        return Math.max(doc_body.scrollHeight, doc_body.offsetHeight,
            doc_elem.clientHeight, doc_elem.scrollHeight, doc_elem.offsetHeight);
    }

    // Returns the page width.
    static get_width() {
        let doc_body = document.body
        let doc_elem = document.documentElement
        return Math.max(doc_body.scrollWidth, doc_body.offsetWidth,
            doc_elem.clientWidth, doc_elem.scrollWidth, doc_elem.offsetWidth);
    }

    // Returns current bounds.
    static get_bounds() {
        return new Box({left:0, right: this.get_width(), top:0, bottom: this.get_height()})
    }

    // Get frame index path as string.
    // Note that there is no leading '/'. 
    // This helps when splitting the string, as there won't be empty elements.
    static get_fw_index_path(win: Window) {
        let ipath = this.get_fw_index_path_as_array(win)
        let spath: string = ''
        for (let i = 0; i < ipath.length; i++) {
            if (i != 0) {
                spath += '/'
            }
            spath += ipath[i].toString()
        }
        return spath
    }

    // Get our fw_index_path as zero-based indexes.
    // An empty fw_index_path means we are in the top window.
    static get_fw_index_path_as_array(win: Window) {
        let path: number[] = []
        let totals: number[] = [] // debugging
        while (win.parent != win) {
            var frames = win.parent.frames
            let found = false
            for (let i = 0; i < frames.length; i++) {
                if (frames[i] === win) {
                    path.unshift(i)
                    totals.unshift(frames.length)
                    found = true
                    break;
                }
            }
            if (!found) {
                console.error('Error did not find parenting frame.')
            }
            win = win.parent
        }
        return path
        //path.push(-2)
        //return path.concat(totals)
    }

    static get_local_client_frame_bounds(frame_window: Window) {
        return new Box({ left: 0, right: frame_window.document.documentElement.clientWidth, top: 0, bottom: frame_window.document.documentElement.clientHeight })
    }

    static get_global_client_frame_bounds(frame_window: Window) {
        let box = PageWrap.get_local_client_frame_bounds(frame_window)
        box.add_offset(PageWrap.get_offset())
        return box
    }

    static get_frame_window(fw_index_path: string) {
        // Go to the root window.
        let win = window
        while(win.parent != win) {
            win = win.parent
        }

        // Split the fw_index_path.
        let splits = fw_index_path.split('/')

        // Traverse down the tree of frames starting at the root.
        for (let i=0; i<splits.length; i++) {
            // Note when empty strings are split on '/', you get an array with one element which is an empty string.
            if (splits[i] === '') {
                continue
            }

            // Get the frame index as a number.
            let frame_index = Number(splits[i])
            var frames = win.frames
            if (frames.length <= frame_index) {
                console.error("Error: could not find frame index.")
            }
            win = frames[frame_index]
        }
        return win
    }

    //---------------------------------------------------------------------------------
    // Xpath Helpers.
    //---------------------------------------------------------------------------------

    // Form an xpath expression which will match any node with a 
    // textnode inside of them whose text matches that given.
    static form_text_match_xpath(text: string): string {
        // We need to break out single quotes and concat them to form a proper xpath.
        let splits = text.split("'")
        if (splits.length > 1) {
            // Concat the splits with single quotes.
            text = "concat(";
            for (let s = 0; s < splits.length; s++) {
                if (s != 0) {
                    text += "," + "\"'\"" + ","  // single quote is added here
                }
                text += "'" + splits[s] + "'"
            }
            text += ")"
        } else {
            // The text has no single quotes here.
            text = "'" + text + "'"
        }
        return "//*[text()[string(.) = " + text + "]]"
    }

    //---------------------------------------------------------------------------------
    // Find single element under the mouse position.
    //---------------------------------------------------------------------------------
    
    // Returns first elem wrap for which getter returns a "true-thy" value.
    static get_smallest_element_with_text(page_pos: Point) {
        let elem_wraps = PageWrap.get_visible_elements(page_pos)
        // This currently assumes that elements are sorted.
        // The ordering comes from ElementsFromPoint however SVG are added the end due to a bug in Chrome.
        // See the use of ElementsFromPoint.
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = elem_wraps[i].get_text()
            if (value) {
                return elem_wraps[i]
            }
        }
        return null
    }

    static get_smallest_element_with_images(page_pos: Point) {
        let elem_wraps = PageWrap.get_visible_elements(page_pos)
        // This currently assumes that elements are sorted.
        // The ordering comes from ElementsFromPoint however SVG are added the end due to a bug in Chrome.
        // See the use of ElementsFromPoint.
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = elem_wraps[i].get_image()
            if (value) {
                return elem_wraps[i]
            }
        }
        return null
    }

    static get_smallest_input_element(page_pos: Point) {
        let elem_wraps = PageWrap.get_visible_elements(page_pos)
        // This currently assumes that elements are sorted.
        // The ordering comes from ElementsFromPoint however SVG are added the end due to a bug in Chrome.
        // See the use of ElementsFromPoint.
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = elem_wraps[i].get_tag_name()
            if (value == 'input') {
                return elem_wraps[i]
            }
        }
        return null
    }

    static get_smallest_select_element(page_pos: Point) {
        let elem_wraps = PageWrap.get_visible_elements(page_pos)
        // This currently assumes that elements are sorted.
        // The ordering comes from ElementsFromPoint however SVG are added the end due to a bug in Chrome.
        // See the use of ElementsFromPoint.
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = elem_wraps[i].get_tag_name()
            if (value == 'select') {
                return elem_wraps[i]
            }
        }
        return null
    }

    static get_smallest_element(page_pos: Point) {
        let elem_wraps = PageWrap.get_visible_elements(page_pos)
        // This currently assumes that elements are sorted.
        // The ordering comes from ElementsFromPoint however SVG are added the end due to a bug in Chrome.
        // See the use of ElementsFromPoint.
        for (let i = 0; i < elem_wraps.length; i++) {
            if (elem_wraps[i].get_image()) {
                return elem_wraps[i]
            } else if (elem_wraps[i].get_text()) {
                return elem_wraps[i]
            } else if (elem_wraps[i].get_tag_name() == 'input') {
                return elem_wraps[i]
            } else if (elem_wraps[i].get_tag_name() == 'select') {
                return elem_wraps[i]
            }
        }
        return null
    }

    //---------------------------------------------------------------------------------
    // Find multiple elements under the mouse position.
    //---------------------------------------------------------------------------------

    // Returns an array of all elem wraps which overlap at the given mouse point.
    // You should not assume that the array is sorted with respect to z-index.
    // SVG elements may be duplicated.
    static get_elements(page_pos: Point): ElemWrap[] {
        let client_pos = new Point(page_pos)
        client_pos.to_client_space(window)
        
        // Find elements underneath the point. 
        // Note that from our testing elementsFromPoint is not able to find svg elements that use <use xlink:href=..> internally.
        // However it does seem to be able to find other svg elements which do not use references internally.
        let elements = document.elementsFromPoint(client_pos.x, client_pos.y)

        // Prune out any elements that are a part of our own smash browse gui elements.
        for (var i = elements.length - 1; i >= 0; i--) {
            if (gui_collection.contains_element(elements[i])) {
                elements.splice(i, 1)
            }
        }

        // Find all svg elements.
        let svg_elements = PageWrap.get_svgs()

        // Filter down to those that intersect our point.
        for (let i = svg_elements.length - 1; i >= 0; i--) {
            let wrap = new ElemWrap(svg_elements[i])
            if (!wrap.contains_point(page_pos)) {
                svg_elements.splice(i, 1)
            }
        }

        // Join the elements together. Note that there can be duplicated SVG elements in here,
        // if the SVG is found elementsFromPoint because we later search for SVGs again.
        elements = elements.concat(svg_elements)

        // Convert the elements to elem wraps.
        let elem_wraps: ElemWrap[] = []
        for (let i = 0; i < elements.length; i++) {
            elem_wraps.push(new ElemWrap(elements[i]))
        }
        return elem_wraps
    }

    // Returns an array of visible elem wraps which overlap the given mouse point.
    // You should not assume that the array is sorted with respect to z-index.
    // SVG elements may be duplicated.
    static get_visible_elements(page_pos: Point): ElemWrap[] {
        let elem_wraps = PageWrap.get_elements(page_pos)
        for (let i = elem_wraps.length - 1; i >= 0; i--) {
            if (!elem_wraps[i].is_valid()) {
                elem_wraps.splice(i, 1)
            }
        }
        return elem_wraps
    }

    //---------------------------------------------------------------------------------
    // Find multiple elements from xpath..
    //---------------------------------------------------------------------------------

    static get_svgs(): SVGElement[] {
        let xpath = "//*[local-name() = 'svg']"
        let set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
        // Convert to elem wraps.
        let elem_wraps: SVGElement[] = []
        for (let i = 0; i < set.snapshotLength; i++) {
            let item = set.snapshotItem(i)
            // We skip any elements that are a part of our own smash browse gui elements.
            if (gui_collection.contains_element(item)) {
                continue
            }
            if (!(item instanceof SVGElement)) {
                continue
            }
            // Otherwise we collect it.
            elem_wraps.push(<SVGElement>item)
        }
        return elem_wraps
    }

    // Returns an array of elem wraps with the given xpath.
    static get_visible_by_xpath(xpath: string): ElemWrap[] {
        let set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
        // Convert to elem wraps.
        let elem_wraps: ElemWrap[] = []
        for (let i = 0; i < set.snapshotLength; i++) {
            let item = set.snapshotItem(i)
            if (!(item instanceof Element)) {
                continue
            }
            // Cast to an Element.
            let element = <Element>(item)
            // We skip any elements that are a part of our own smash browse gui elements.
            if (gui_collection.contains_element(element)) {
                continue
            }
            // Create the elem wrap.
            let ew = new ElemWrap(element)
            // Drop it if it's not visible.
            if (!ew.is_visible()) {
                continue
            }
            // Otherwise we collect it.
            elem_wraps.push(ew)
        }
        return elem_wraps
    }

    //---------------------------------------------------------------------------------
    // Find all elements.
    //---------------------------------------------------------------------------------

    // Returns an array of all the elem wraps.
    static get_all_visible(): ElemWrap[] {
        let xpath = "//*";
        return PageWrap.get_visible_by_xpath(xpath)
    }

    //---------------------------------------------------------------------------------
    // Filter Elements.
    //---------------------------------------------------------------------------------

    // Returns all elem wraps which intersect with the given box.
    static filter_by_box_intersection(elem_wraps: ElemWrap[], page_box: Box): ElemWrap[] {
        let matches: ElemWrap[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            if (page_box.intersects(elem_wraps[i].get_box())) {
                matches.push(elem_wraps[i])
            }
        }
        return matches
    }

    // Returns all elem wraps which are contained in the given box.
    static filter_by_box_containment(elem_wraps: ElemWrap[], page_box: Box): ElemWrap[] {
        let matches: ElemWrap[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            if (page_box.contains(elem_wraps[i].get_box())) {
                matches.push(elem_wraps[i])
            }
        }
        return matches
    }

    // Returns elem wraps which match the target value.
    static filter_by_value(elem_wraps: ElemWrap[], value_getter: (wrap: ElemWrap)=>string, target_value: string): ElemWrap[] {
        // Loop over the elem wraps.
        let wrap: ElemWrap[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            let wrapper = elem_wraps[i]

            // See if this elem wrap's value matches our value.
            let value = value_getter(wrapper)

            // If the target_value is an empty string we match any non empty string value.
            if ((!target_value) && value) {
                wrap.push(wrapper)
            }
            // Otherwise the value must match the target value.
            else if (target_value == value) {
                wrap.push(wrapper)
            }
        }
        return wrap
    }

}



