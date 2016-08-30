
// The class encapsulates the properties of a web page.
class MutationMonitor {
    // Static Members.
    static mutation_check_interval = 100 // time interval to check whether we've waited long enough
    static mutation_done_interval = 1000 // minimum time since last mutation, to be considered fully completed and done

    // Our Dependencies.
    content_comm: ContentComm
    gui_collection: GUICollection
    
    // Our Members.
    mutation_timer: number
    last_mutation_time: Date
    mutation_observer: MutationObserver
    mutation_observer_config: MutationObserverInit
    page_is_ready: boolean
    
    // Constructor.
    constructor(cc: ContentComm, gc: GUICollection) {
        this.content_comm = cc
        this.gui_collection = gc

        // Mutation timer.
        this.last_mutation_time = null // Time of last dom mutation in this page.
        this.mutation_timer = null
        this.page_is_ready = false // Page is ready when there have been no dom mutations for (mutation_done_interval) seconds.

        // Mutation Observer.
        this.mutation_observer = new MutationObserver(this.on_mutation.bind(this))
        this.mutation_observer_config = {
            childList: true,
            subtree: true,
            attributes: false,
            characterData: false
        }
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
            this.content_comm.send_to_bg({ info: 'page_is_loading' })
            this.page_is_ready = false
            this.last_mutation_time = new Date();
            this.mutation_timer = setInterval(this.update_mutation_timer.bind(this), MutationMonitor.mutation_check_interval)
        }
    }

    // Stops and destroys the mutation timer.
    stop_mutation_timer(): void {
        clearInterval(this.mutation_timer)
        this.mutation_timer = null
    }

    // This function is called every interval.
    update_mutation_timer(): void {
        let current_time = new Date()
        let last_mutation_delta = current_time.getTime() - this.last_mutation_time.getTime()
        if (last_mutation_delta > MutationMonitor.mutation_done_interval) {
            this.stop_mutation_timer()
            this.page_is_ready = true
            console.log("MutationMonitor: page is now ready with delta: " + last_mutation_delta)
            // Only send the page_is_ready from the top frame.
            if (window == window.top) {
                this.content_comm.send_to_bg({ info: 'page_is_ready' })
                console.log("MutationMonitor: sending out page is ready message from the top window.")  
                // We can now show the context menu.
                this.gui_collection.initialize()
                this.gui_collection.wait_popup.close()
            }
        }
    }

    // This function is called on every mutation of the dom.
    on_mutation(mutations: MutationRecord[], observer: MutationObserver): void {
        mutations.forEach(function(mutation) {
            // mutation.addedNodes.length
            // mutation.removedNodes.length
        });
        if (mutations.length == 0) {
            return
        }

        if (this.mutation_timer == null) {
            //console.log('starting mutation timer')
            this.start_mutation_timer()
            this.gui_collection.wait_popup.open()
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
                if (rule.type === CSSRule.STYLE_RULE && hover_regex.test((<CSSStyleRule>rule).selectorText)) {
                    //console.log('deleting rule for ' + rule.selectorText + '+++' + rule.style)
                    css_sheet.deleteRule(j);
                }
            }
        }
        //  let style = document.createElement('style');
        //  style.type = 'text/css';
        //  style.innerHTML = '*:hover {all:initial!important;}'
        //  document.getElementsByTagName('head')[0].appendChild(style);
    }

    // Removes all zoom settings from css rules, by setting them all to 1.
    // The css zoom is not supported by all browsers and so should be safe to remove.
    // In Chrome:
    // 1) styles with "zoom: reset;" causes getBoundingClientRect() to return incorrect shifted values.
    // 2) also we see that getBoundingClientRect()'s coordinate space becomes different from the space
    //    used by ElementsFromPoint. There is no obvious way to efficiently figure out the scaling 
    //    from the DOM API. Without using some kind of minimization like probing with ElementsFromPoint
    //    until we hit a know element.
    remove_zoom_property(rules: CSSRuleList): void {
        if (!rules) {
            return
        }
        for (let j = 0; j < rules.length; j++) {
            let rule = rules[j];
            if (rule.type == CSSRule.MEDIA_RULE) {
                // This rule acts like a grouping rule, so we recurse.
                this.remove_zoom_property((<CSSMediaRule>rule).cssRules)
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
        let zoom_regex = /zoom:\s*reset/;
        for (let i = 0; i < document.styleSheets.length; i++) {
            let sheet = document.styleSheets[i]
            // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
            // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
            if (sheet.type != "text/css") {
                continue
            }
            let css_sheet = <CSSStyleSheet>(sheet)
            this.remove_zoom_property(css_sheet.cssRules)
        }
    }

    on_loaded() {
        // Disable hovers.
        this.disable_hover()
        this.disable_zoom()

        // We don't allow iframes to initialize.
        if (window == window.top) {
            // Start mutation to timer, to try and detect when page is fully loaded.
            this.start_mutation_timer()

            // Listen to mutations.
            this.mutation_observer.observe(window.document, this.mutation_observer_config);

            // Initialize and open the wait popup.
            this.gui_collection.wait_popup.open()
        }
    }
}



