
// The class encapsulates the properties of a web page.
class MutationMonitor {
    // Static Members.
    static mutation_check_interval = 100 // time interval to check whether we've waited long enough
    static mutation_done_interval = 500 // minimum time since last mutation, to be considered fully completed and done

    // Our Dependencies.
    private content_comm: ContentComm
    private gui_collection: GUICollection
    
    // Our Members.
    private mutation_timer: number
    private last_mutation_time: Date
    private mutation_observer: MutationObserver
    private mutation_observer_config: MutationObserverInit
    private page_is_ready: boolean
    private loaded_callbacks: (()=>void) []
    
    // Constructor.
    constructor(cc: ContentComm, gc: GUICollection) {
        this.content_comm = cc
        this.gui_collection = gc

        // Mutation timer.
        this.last_mutation_time = null // Time of last dom mutation in this page.
        this.mutation_timer = null
        this.page_is_ready = false // Page is ready when there have been no dom mutations for (mutation_done_interval) seconds.

        // Callbacks.
        this.loaded_callbacks = []

        // Mutation Observer.
        this.mutation_observer = new MutationObserver((mutations: MutationRecord[], observer: MutationObserver) => { this.on_mutation(mutations, observer) })
        this.mutation_observer_config = {
            childList: true,
            subtree: true,
            attributes: false,
            characterData: false
        }
    }

    add_loaded_callback(callback: ()=>void): void {
        this.loaded_callbacks.push(callback)
    }

    fire_loaded_callbacks(): void {
        for (let i=0; i< this.loaded_callbacks.length; ++i) {
            this.loaded_callbacks[i]()
        }
        // The callbacks should only be fired once, so clear them.
        this.loaded_callbacks.length = 0
    }

    //---------------------------------------------------------------------------------
    // Mutation Observation.
    //---------------------------------------------------------------------------------

    // Returns whether the page is currently loading something.
    // This is typically called before sending a request from the app.
    is_loading(): boolean {
        return !this.page_is_ready
    }

    // Creates and starts the mutation timer.
    start_mutation_timer(): void {
        if (this.mutation_timer == null) {
            this.page_is_ready = false
            this.last_mutation_time = new Date();
            this.mutation_timer = setInterval(() => { this.update_mutation_timer() }, MutationMonitor.mutation_check_interval)
        }
    }

    // Stops and destroys the mutation timer.
    stop_mutation_timer(): void {
        clearInterval(this.mutation_timer)
        this.mutation_timer = null

        // Now that the mutation's have stopped, let's adjust the page.
        this.adjust_page()

        // Fire loaded callbacks.
        this.fire_loaded_callbacks()
    }

    // This function is called every interval.
    update_mutation_timer(): void {
        let current_time = new Date()
        let last_mutation_delta = current_time.getTime() - this.last_mutation_time.getTime()
        if (last_mutation_delta > MutationMonitor.mutation_done_interval) {
            this.stop_mutation_timer()
            this.page_is_ready = true // Page is now ready.
            // We can now show the context menu.
            this.gui_collection.initialize()
            this.gui_collection.wait_popup.close()
            // Update our overlay sets as some of elements may have disappeared.
            this.gui_collection.page_overlays.update_element_overlays()
        }
    }

    // This function is called on every mutation of the dom.
    on_mutation(mutations: MutationRecord[], observer: MutationObserver): void {
        // We want to ignore mutations due to streaming of media.
        // On cnet video pages it seems to continually send newly added textnodes as the mutation.
        // So we just ignore those here. 
        let ignore: boolean = true
        for (let m = 0; m < mutations.length; m++) {
            let mutation = mutations[m]

            if (mutation.type == "childList") {
                for (let n = 0; n < mutation.addedNodes.length; n++) {
                    let node_name = mutation.addedNodes[n].nodeName.toLowerCase()
                    //console.log('mutation node name: ' + node_name)
                    if (node_name != '#text' && node_name != 'svg' && node_name != 'span' && node_name != 'div') {
                        ignore = false
                        break
                    }
                }
            }
            if (!ignore) {
                break
            }
        }
        if (ignore) {
            return
        }
            // mutations.forEach(function(mutation: MutationRecord) {
            // console.log("got mutation: " + mutation.type)
            // if (mutation.type == "childList") {
            //     for (var i = 0; i < mutation.addedNodes.length; i++) {
            //         console.log('add node name: ' + mutation.addedNodes[i].nodeName)
            //         if (mutation.addedNodes[i].nodeName.toLowerCase() == '#text') {
            //             console.log('ignoring mutation!')
            //             return
            //         }
            //     }
            // }
            // mutation.addedNodes.length
            // mutation.removedNodes.length
            // });
        if (mutations.length == 0) {
            console.log('ignoring mutation2!')
            return
        }

        if (this.mutation_timer == null) {
            //console.log('starting mutation timer')
            //this.start_mutation_timer()
            //this.gui_collection.wait_popup.open()
        } else {
            this.last_mutation_time = new Date();
        }
    }

    // Disables hover behavior on the page.
    disable_hover() {
        let hover_regex = /:hover/;
        for (let i = 0; i < document.styleSheets.length; i++) {
            let sheet: StyleSheet = document.styleSheets[i]
            if (sheet.type != "text/css") {
                continue
            }
            let css_sheet = <CSSStyleSheet>(sheet)
            let rules = css_sheet.cssRules
            // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
            // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
            if (!rules) {
                continue;
            }
            // Loop over the rules.
            for (let j = rules.length - 1; j >= 0; j--) {
                let rule = rules[j];
                if (rule.type == CSSRule.MEDIA_RULE) {
                    // This rule acts like a grouping rule, so we recurse.
                    this.remove_hover_rules(<CSSMediaRule>rule)
                } else if (rule.type === CSSRule.STYLE_RULE) {
                    let style_rule = <CSSStyleRule>rule
                    if (hover_regex.test(style_rule.selectorText.toLowerCase())) {
                        //console.log('deleting rule for ' + rule.selectorText + '+++' + rule.style)
                        let st = style_rule.selectorText.toLowerCase()
                        st = st.replace(/:hover/g, "")
                        //style_rule.selectorText = st
                        css_sheet.deleteRule(j);
                    }
                }
            }
        }
        //  let style = document.createElement('style');
        //  style.type = 'text/css';
        //  style.innerHTML = '*:hover {all:initial!important;}'
        //  document.getElementsByTagName('head')[0].appendChild(style);
    }

    remove_hover_rules(media_rule: CSSMediaRule): void {
        if (!media_rule) {
            return
        }
        let rules = media_rule.cssRules
        let hover_regex = /:hover/;
        for (let j = rules.length-1; j >= 0; j--) {
            let rule = rules[j];
            if (rule.type == CSSRule.MEDIA_RULE) {
                // This rule acts like a grouping rule, so we recurse.
                this.remove_hover_rules(<CSSMediaRule>rule)
            } else if (rule.type === CSSRule.STYLE_RULE) {
                let style_rule = <CSSStyleRule>rule
                if (hover_regex.test(style_rule.selectorText.toLowerCase())) {
                    //console.log('deleting rule for ' + rule.selectorText + '+++' + rule.style)
                    let st = style_rule.selectorText.toLowerCase()
                    st = st.replace(/:hover/g, "")
                    //style_rule.selectorText = st
                    media_rule.deleteRule(j);
                }
            }
        }
    }

    // Removes all zoom settings from css rules, by setting them all to 1.
    // The css zoom is not supported by all browsers and so should be safe to remove.
    // In Chrome:
    // 1) styles with "zoom: reset;" causes getBoundingClientRect() to return incorrect shifted values.
    // 2) also we see that getBoundingClientRect()'s coordinate space becomes different from the space
    //    used by ElementsFromPoint. There is no obvious way to efficiently figure out the scaling 
    //    from the DOM API. Without using some kind of minimization like probing with ElementsFromPoint
    //    until we hit a know element.
    adjust_zoom_property(rules: CSSRuleList): void {
        if (!rules) {
            return
        }
        for (let j = 0; j < rules.length; j++) {
            let rule = rules[j];
            if (rule.type == CSSRule.MEDIA_RULE) {
                // This rule acts like a grouping rule, so we recurse.
                this.adjust_zoom_property((<CSSMediaRule>rule).cssRules)
            } else if (rule.type == CSSRule.STYLE_RULE) {
                // This is the normal leaf rule type.
                let style_rule = <CSSStyleRule>rule
                if (style_rule.style) {
                    style_rule.style.zoom = "1"
                }
            }
        }
    }

    // Disables "zoom: reset;" in all styles. 
    // In Chrome, styles with "zoom: reset;" causes getBoundingClientRect() to return incorrect shifted values.
    disable_zoom() {
        //let zoom_regex = /zoom:\s*reset/;
        for (let i = 0; i < document.styleSheets.length; i++) {
            let sheet = document.styleSheets[i]
            // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
            // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
            if (sheet.type != "text/css") {
                continue
            }
            let css_sheet = <CSSStyleSheet>(sheet)
            this.adjust_zoom_property(css_sheet.cssRules)
        }
    }

    adjust_scroll_property(rules: CSSRuleList): void {
        if (!rules) {
            return
        }
        for (let j = 0; j < rules.length; j++) {
            let rule = rules[j];
            if (rule.type == CSSRule.MEDIA_RULE) {
                // This rule acts like a grouping rule, so we recurse.
                this.adjust_scroll_property((<CSSMediaRule>rule).cssRules)
            } else if (rule.type == CSSRule.STYLE_RULE) {
                // This is the normal leaf rule type.
                let style_rule = <CSSStyleRule>rule
                if (style_rule.style) {
                    if (style_rule.style.overflow) {
                        style_rule.style.setProperty('position','static','')
                    }
                    if (style_rule.style.overflowY) {
                        style_rule.style.setProperty('position','static','')
                    }
                    if (style_rule.style.overflowX) {
                        style_rule.style.setProperty('position','static','')
                    }
                }
            }
        }
    }

    disable_scrollbars() {
        for (let i = 0; i < document.styleSheets.length; i++) {
            let sheet = document.styleSheets[i]
            // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
            // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
            if (sheet.type != "text/css") {
                continue
            }
            let css_sheet = <CSSStyleSheet>(sheet)
            this.adjust_scroll_property(css_sheet.cssRules)
        }
    }

    // Not really tested.
    adjust_display_property(rules: CSSRuleList): void {
        if (!rules) {
            return
        }
        for (let j = 0; j < rules.length; j++) {
            let rule = rules[j];
            if (rule.type == CSSRule.MEDIA_RULE) {
                // This rule acts like a grouping rule, so we recurse.
                this.adjust_display_property((<CSSMediaRule>rule).cssRules)
            } else if (rule.type == CSSRule.STYLE_RULE) {
                // This is the normal leaf rule type.
                let style_rule = <CSSStyleRule>rule
                if (style_rule.style) {
                    style_rule.style.display = "initial"
                }
            }
        }
    }

    // Not really tested.
    show_hidden() {
        for (let i = 0; i < document.styleSheets.length; i++) {
            let sheet = document.styleSheets[i]
            // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
            // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
            if (sheet.type != "text/css") {
                continue
            }
            let css_sheet = <CSSStyleSheet>(sheet)
            this.adjust_zoom_property(css_sheet.cssRules)
        }
    }

    adjust_page() {
        //this.disable_hover()
        //this.disable_zoom()

        //this.disable_scrollbars()
        //this.add_video_controls()
        //this.show_hidden()
    }

    on_loaded() {
        this.adjust_page()
        
        // Hide scroll bars.
        window.document.documentElement.style.overflow = "hidden"

        // Start mutation to timer, to try and detect when page is fully loaded.
        this.start_mutation_timer()

        // Listen to mutations.
        this.mutation_observer.observe(window.document, this.mutation_observer_config);

        // Initialize and open the wait popup.
        this.gui_collection.wait_popup.open()
    }
}



