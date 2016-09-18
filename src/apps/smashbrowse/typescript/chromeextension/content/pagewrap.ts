
// The class encapsulates the properties of a web page.
class PageWrap {
    // Our Dependencies.
    gui_collection: GUICollection // This is like our owning parent.
    
    // Constructor.
    constructor(gc: GUICollection) {
        this.gui_collection = gc
    }

    //---------------------------------------------------------------------------------
    // Page Properties.
    //---------------------------------------------------------------------------------

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
        let box = new Box()
        box.left = 0
        box.top = 0
        box.right = this.get_width()
        box.bottom = this.get_height()
        return box
    }

    // Get frame index path as string.
    // Note that there is no leading '/'. 
    // This helps when splitting the string, as there won't be empty elements.
    static get_iframe_index_path_as_string(win: Window) {
        let ipath = this.get_iframe_index_path(win)
        let spath: string = ''
        for (let i = 0; i < ipath.length; i++) {
            if (i != 0) {
                spath += '/'
            }
            spath += ipath[i].toString()
        }
        return spath
    }

    // Get our iframe path as zero-based indexes.
    // An empty iframe path means we are in the top window.
    static get_iframe_index_path(win: Window) {
        let path: number[] = []
        let totals: number[] = [] // debugging
        while (win.parent != win) {
            var iframes = win.parent.document.getElementsByTagName('iframe');
            let found = false
            for (let i = iframes.length-1; i>=0; i--) {
                if (iframes[i].contentWindow === win) {
                    path.unshift(i)
                    totals.unshift(iframes.length)
                    found = true
                    break;
                }
            }
            if (!found) {
                console.error('Error did not find parenting iframe')
            }
            win = win.parent
        }
        return path
        //path.push(-2)
        //return path.concat(totals)
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
    // Find elem wraps by mouse position.
    //---------------------------------------------------------------------------------

    // Returns an array of all elem wraps which overlap at the given mouse point.
    // Note this will miss out on reporting svg elements.
    get_overlapping_at(page_pos: Point): ElemWrap[] {
        let client_pos = new Point(page_pos)
        client_pos.to_client_space()
        // declare Document.msElementsFromPoint(x: number, y: number){} // The DefinitelyTyped libraries seem to be missing this call.
        let hack: any = document
        let nodes: NodeList = hack.elementsFromPoint(client_pos.x, client_pos.y)

        // Convert to elem wraps.
        let elem_wraps: ElemWrap[] = []
        for (let i = 0; i < nodes.length; i++) {
            if (!(nodes[i] instanceof Element)) {
                continue
            }
            let element = <Element>(nodes[i])
            // We skip any elements that are a part of our own smash browse gui elements.
            if (this.gui_collection.contains_element(element)) {
                continue
            }
            elem_wraps.push(new ElemWrap(element))
        }
        return elem_wraps
    }

    // Returns the elem wraps in z-order under the given page point, until we become
    // fully opaque. Usually we will hit an opaque background plate.
    // The document.elementsFromPoint seems to skip svg elem wraps, so we add these in manually.
    get_visible_overlapping_at(page_pos: Point): ElemWrap[] {
        let elem_wraps = this.get_overlapping_at(page_pos)

        // Debug settings.
        //        if (false) {
        //            console.log('num elem wraps: ' + elem_wraps.length)
        //            for (let i = 0; i < elem_wraps.length; i++) {
        //                let wrapper = elem_wraps[i]
        //                console.log('elem wrap[' + i + ']: opacity' + wrapper.get_opacity()
        //                    + ' bg: ' + wrapper.get_background_color() + ' xpath: ' + wrapper.get_xpath())
        //            }
        //        }

        // Trim it to point where we reach the back plate of the page.
        for (let i = 0; i < elem_wraps.length; i++) {
            let wrapper = elem_wraps[i]
            if (wrapper.is_back_plate()) {
                elem_wraps.length = i + 1
                break
            }
        }

        // Splice out any invisible elem wraps.
        for (let i = 0; i < elem_wraps.length; i++) {
            if (!elem_wraps[i].is_visible()) {
                elem_wraps.splice(i, 1)
                i -= 1
            }
        }

        // Build xpath to find all svg elem wraps.
        let svgs: ElemWrap[] = this.get_svgs() //this.get_visible_by_xpath(xpath)

        // Loop over the elem wraps.
        let svg: ElemWrap = null
        for (let i = 0; i < svgs.length; i++) {
            svg = svgs[i]

            // Make sure the svg is visible.
            if (!svg.is_visible()) {
                continue
            }

            // Make sure the svg contains the page point.
            if (!svg.contains_point(page_pos)) {
                continue
            }

            // Add the svg to the elem wraps.
            elem_wraps.unshift(svg)
        }
        return elem_wraps
    }

    // Returns first elem wrap for which getter returns a "true-thy" value.
    get_first_elem_wrap_at(getter: () => string, page_pos: Point) {
        let elem_wraps = this.get_visible_overlapping_at(page_pos)
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = getter.call(elem_wraps[i])
            if (value) {
                return elem_wraps[i]
            }
        }
        return null
    }

    // Returns the top elem wrap with text, according to z-order.
    get_top_text_elem_wrap_at(page_pos: Point): ElemWrap {
        let candidate = this.get_first_elem_wrap_at(ElemWrap.prototype.get_text, page_pos)
        if (!candidate) {
            return null
        }
        return candidate
    }

    //Returns the top elem wrap with an image, according to z-order.
    get_top_image_elem_wrap_at(page_pos: Point): ElemWrap {
        let candidate = this.get_first_elem_wrap_at(ElemWrap.prototype.get_image, page_pos)
        if (!candidate) {
            return null
        }
        return candidate
    }

    //---------------------------------------------------------------------------------
    //Find elem wraps by non mouse values.
    //---------------------------------------------------------------------------------

    get_svgs(): ElemWrap[] {
        let xpath = "//*[local-name() = 'svg']"
        let set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
        // Convert to elem wraps.
        let elem_wraps: ElemWrap[] = []
        for (let i = 0; i < set.snapshotLength; i++) {
            let item = set.snapshotItem(i)
            // We skip any elements that are a part of our own smash browse gui elements.
            if (this.gui_collection.contains_element(item)) {
                continue
            }
            if (!(item instanceof SVGElement)) {
                continue
            }
            // Create the elem wrap.
            let ew = new ElemWrap(<SVGElement>item)
            // Drop it if it's not visible.
            if (!ew.is_visible()) {
                continue
            }
            // Otherwise we collect it.
            elem_wraps.push(ew)
        }
        return elem_wraps
    }

    get_videos(): ElemWrap[] {
        let xpath = "//*[local-name() = 'video']"
        let set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
        // Convert to elem wraps.
        let elem_wraps: ElemWrap[] = []
        for (let i = 0; i < set.snapshotLength; i++) {
            let item = set.snapshotItem(i)
            // We skip any elements that are a part of our own smash browse gui elements.
            if (this.gui_collection.contains_element(item)) {
                continue
            }
            if (!(item instanceof HTMLVideoElement)) {
                continue
            }
            // Create the elem wrap.
            let ew = new ElemWrap(<HTMLVideoElement>item)
            // Drop it if it's not visible.
            if (!ew.is_visible()) {
                continue
            }
            // Otherwise we collect it.
            elem_wraps.push(ew)
        }
        return elem_wraps
    }

    // Returns an array of elem wraps with the given xpath.
    // Note that even in visible elements will be returned.
    get_visible_by_xpath(xpath: string): ElemWrap[] {
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
            if (this.gui_collection.contains_element(element)) {
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

    // Returns an array of all the elem wraps.
    get_all_visible(): ElemWrap[] {
        let xpath = "//*";
        return this.get_visible_by_xpath(xpath)
    }

    // Returns all elem wraps which intersect with the given box.
    get_intersecting_with(page_box: Box): ElemWrap[] {
        let intersecting: ElemWrap[] = []
        let elem_wraps = this.get_all_visible()
        for (let i = 0; i < elem_wraps.length; i++) {
            if (page_box.intersects(elem_wraps[i].get_box())) {
                intersecting.push(elem_wraps[i])
            }
        }
        return intersecting
    }

    // Returns all elem wraps which are contained in the given box.
    get_contained_in(page_box: Box): ElemWrap[] {
        let contained: ElemWrap[] = []
        let elem_wraps = this.get_all_visible()
        for (let i = 0; i < elem_wraps.length; i++) {
            if (page_box.contains(elem_wraps[i].get_box())) {
                contained.push(elem_wraps[i])
            }
        }
        return contained
    }

    // Returns elem wraps which match any of the target values.
    get_by_any_value(wrap_type: WrapType, target_values: string[]): ElemWrap[] {
        let getter = ElemWrap.prototype.get_getter_from_wrap_type(wrap_type)

        // Get all elem wraps.
        let elem_wraps = this.get_all_visible()

        // Loop over the elem wraps.
        let wrapper: ElemWrap = null
        let value: string = null
        let matches: ElemWrap[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            wrapper = elem_wraps[i]

            // Make sure the wrapper is visible.
            if (!wrapper.is_visible()) {
                continue
            }

            // Skip the elem wrap if it's part of the smash browse menu.
            if (this.gui_collection.contains_element(wrapper.get_element())) {
                continue
            }

            // See if this elem wrap value matches something in target_values
            value = getter.call(wrapper)

            // If taget_values is an empty array we match any truthful value.
            if ((target_values.length == 0) && value) {
                matches.push(wrapper)
            }
            // Otherwise we match any value in the target_values.
            else if (target_values.indexOf(value) >= 0) {
                matches.push(wrapper)
            }
        }
        return matches
    }

    // Returns an array of elem wraps selected by the matcher.
    get_by_all_values(wrap_type: WrapType, target_values: string[]): ElemWrap[] {
        // Get our initial candiate elem wraps.
        let candidates: ElemWrap[] = this.get_by_any_value(wrap_type, target_values)

        // Get our getter.
        let getter: () => string = ElemWrap.prototype.get_getter_from_wrap_type(wrap_type)

        // Determines arrays of surrounding elem wraps for each elem wrap.
        let overlaps: string[][] = []
        for (let i = 0; i < candidates.length; i++) {
            overlaps.push([])
            for (let j = 0; j < candidates.length; j++) {
                if (candidates[j].intersects(candidates[i])) {
                    overlaps[i].push(getter.call(candidates[j]))
                }
            }
        }

        // Find the elem wraps which have surrounding elem wraps matching the target_values.
        let matching: ElemWrap[] = []
        for (let i = 0; i < overlaps.length; i++) {
            let found = true
            for (let j = 0; j < target_values.length; j++) {
                if (overlaps[i].indexOf(target_values[j]) < 0) {
                    found = false
                    break
                }
            }
            if (found) {
                matching.push(candidates[i])
            }
        }

        // Initialize eliminated to all false.
        let eliminated: boolean[] = []
        for (let i = 0; i < matching.length; i++) {
            eliminated.push(false)
        }

        // Coalesce all contained elem wraps together
        // and find the smallest most internal element
        for (let i = 0; i < matching.length; i++) {
            if (eliminated[i]) {
                continue
            }
            for (let j = 0; j < matching.length; j++) {
                if (j == i) {
                    continue
                }
                if (eliminated[j]) {
                    continue
                }
                if (matching[j].contains(matching[i])) {
                    eliminated[j] = true
                }
            }
        }

        // Extract the non eliminated elem wraps out.
        let results: ElemWrap[] = []
        for (let i = 0; i < eliminated.length; i++) {
            if (!eliminated[i]) {
                results.push(matching[i])
            }
        }

        // Phew we're done!
        return results
    }

    //---------------------------------------------------------------------------------
    // Value extraction.
    //---------------------------------------------------------------------------------

    // Returns an array of values obtained by looping through the 
    // elem wraps and retrieving values using the supplied getter.
    static extract_values(elem_wraps: ElemWrap[], getter: () => string): string[] {
        let values: string[] = []
        for (let i = 0; i < elem_wraps.length; i++) {
            let value = getter.call(elem_wraps[i])
            if (!value || Utils.is_all_whitespace(value)) {
                continue
            }
            if (values.indexOf(value) >= 0) {
                continue
            }
            values.push(value)
        }
        return values
    }

    // Returns an array of images values from elem wraps under the given page point.
    static get_image_values_at(elem_wraps: ElemWrap[], page_pos: Point): string[] {
        let values = PageWrap.extract_values(elem_wraps, ElemWrap.prototype.get_image)
        return values
    }

    // Returns an array of text values from elem wraps under the given page point.
    static get_text_values_at(elem_wraps: ElemWrap[], page_pos: Point): string[] {
        let values = PageWrap.extract_values(elem_wraps, ElemWrap.prototype.get_text)
        // Values will contain text from bigger overlapping divs with text.
        // Unlike images text won't overlap, so we only want the first text.
        if (values.length > 1) {
            values.length = 1
        }
        return values
    }

}



