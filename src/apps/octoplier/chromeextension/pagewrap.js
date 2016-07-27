//The class encapsulate properties of the target web page.
var PageWrap = function() {
    //Mutation timer.
    this.last_mutation_time = null //Time of last dom mutation in this page.
    var mutation_timer = null
    this.page_is_ready = false //Page is ready there have been no dom mutations for (mutation_done_interval) seconds.
    
    //Mutation Observer.
    this.mutation_observer = new MutationObserver(this.on_mutation.bind(this))
    this.mutation_observer_config = {
        childList: true,
        subtree : true,
        attributes: false,
        characterData : false
    }
}

//---------------------------------------------------------------------------------
//Mutation Observation.
//---------------------------------------------------------------------------------

PageWrap.prototype.mutation_check_interval = 100 // time interval to check whether we've waited long enough
PageWrap.prototype.mutation_done_interval = 1000 // minimum time since last mutation, to be considered fully completed and done

//Returns whether the page is currently loading something.
//This is typically called before sending a request from the app.
PageWrap.prototype.is_loading = function() {
    return !this.page_is_ready
}

//Creates and starts the mutation timer.
PageWrap.prototype.start_mutation_timer = function() {
    if (this.mutation_timer == null) {
        g_content_comm.send_to_bg({info: 'page_is_loading'})
        this.page_is_ready = false
        this.last_mutation_time = new Date();
        this.mutation_timer = setInterval(this.update_mutation_timer.bind(this), this.mutation_check_interval)
    }
}

//Stops and destroys the mutation timer.
PageWrap.prototype.stop_mutation_timer = function() {
    clearInterval(this.mutation_timer)
    this.mutation_timer = null
}

//This function is called every interval.
PageWrap.prototype.update_mutation_timer = function() {
    var current_time = new Date()
    var last_mutation_delta = current_time.getTime() - this.last_mutation_time.getTime()
    if (last_mutation_delta > this.mutation_done_interval) {
        this.stop_mutation_timer()
        this.page_is_ready = true
        console.log("PageWrap: page is now ready with delta: " + last_mutation_delta)
        // Only send the page_is_ready from the top frame.
        if (window == window.top) {
            g_content_comm.send_to_bg({info: 'page_is_ready'})
            console.log("PageWrap: sending out page is ready message from the top window.")
            
            // We can now show the context menu.
            g_context_menu.initialize()
            g_wait_popup.close()
        }
    }
}

//This function is called on every mutation of the dom.
PageWrap.prototype.on_mutation = function(mutations) {
    mutations.forEach(function (mutation) {
        // mutation.addedNodes.length
        // mutation.removedNodes.length
    });
    if (mutations.length == 0) {
        return
    }
    
    if (this.mutation_timer == null) {
        console.log('starting mutation timer')
        this.start_mutation_timer()
        g_wait_popup.open()
    } else {
        this.last_mutation_time = new Date();
    }
} 



//---------------------------------------------------------------------------------
//Page Properties.
//---------------------------------------------------------------------------------

//Returns the page height.
PageWrap.prototype.get_height = function() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max( doc_body.scrollHeight, doc_body.offsetHeight, 
            doc_elem.clientHeight, doc_elem.scrollHeight, doc_elem.offsetHeight );
}

//Returns the page width.
PageWrap.prototype.get_width = function() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max(doc_body.scrollWidth, doc_body.offsetWidth, 
            doc_elem.clientWidth, doc_elem.scrollWidth, doc_elem.offsetWidth );
}

//Returns current bounds.
PageWrap.prototype.get_bounds = function() {
    var box = new PageBox()
    box.left = 0
    box.top = 0
    box.right = this.get_width()
    box.bottom = this.get_height()
    return box
}

//Disables hover behavior on the page.
PageWrap.prototype.disable_hover = function() {
    var hover_regex = /:hover/;
    for (var i = 0; i < document.styleSheets.length; i++) {
        var sheet = document.styleSheets[i]
        var rules = sheet.cssRules
        // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
        // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
        if (!rules) {
            continue;
        }
        // Loop over the rules.
        for (var j = rules.length - 1; j >= 0; j--) {
            var rule = rules[j];
            if (rule.type === CSSRule.STYLE_RULE && hover_regex.test(rule.selectorText)) {
                //console.log('deleting rule for ' + rule.selectorText + '+++' + rule.style)
                sheet.deleteRule(j);
            }
        }
    }
}



//---------------------------------------------------------------------------------
//Xpath Helpers.
//---------------------------------------------------------------------------------

//Form an xpath expression which will match any node with a 
//textnode inside of them whose text matches that given.
PageWrap.prototype.form_text_match_xpath = function(text) {
// We need to break out single quotes and concat them to form a proper xpath.
var splits = text.split("'")
if (splits.length>1) {
    // Concat the splits with single quotes.
    text = "concat(";
    for (var s=0; s<splits.length; s++) {
        if (s!=0){
            text += "," + "\"'\"" + ","  // single quote is added here
        }
        text += "'" + splits[s] + "'"
    }
    text += ")"
} else {
    // The text has no single quotes here.
    text = "'" + text + "'"
}
return "//*[text()[string(.) = "+ text +"]]"
}

//---------------------------------------------------------------------------------
//Find elem wraps by mouse position.
//---------------------------------------------------------------------------------

//Returns an array of all elem wraps which overlap at the given mouse point.
//Note this will miss out on reporting svg elements.
PageWrap.prototype.get_overlapping_at = function(page_x, page_y) {
    // Use the document.elementsFromPoint class.
    var elements = document.elementsFromPoint(page_x-window.scrollX, page_y-window.scrollY)
    // Convert to elem wraps.
    var elem_wraps = []
    for (var i=0; i<elements.length; i++) {
        // *Important!* We weed out any context menu hits here.
        if (g_event_blocker.gui_contains_element(elements[i])) {
            continue
        }
        elem_wraps.push(new ElemWrap(elements[i]))
    }
    return elem_wraps
}

//Returns the elem wraps in z-order under the given page point, until we become
//fully opaque. Usually we will hit an opaque background plate.
//The document.elementsFromPoint seems to skip svg elem wraps, so we add these in manually.
PageWrap.prototype.get_visible_overlapping_at = function(page_x, page_y) {
  var elem_wraps = this.get_overlapping_at(page_x, page_y)
  
  // Debug settings.
  if (false) {
      console.log('num elem wraps: ' + elem_wraps.length)
      for (var i=0; i<elem_wraps.length; i++) {
          var wrapper = elem_wraps[i]
          console.log('elem wrap['+i+']: opacity' + wrapper.get_opacity() 
                  + ' bg: ' + wrapper.get_background_color() + ' xpath: ' + wrapper.get_xpath() )
      }
  }
  
  // Trim it to point where we reach the back plate of the page.
  for (var i=0; i<elem_wraps.length; i++) {
      var wrapper = elem_wraps[i]
      if (wrapper.is_back_plate()) {
          elem_wraps.length = i+1
          break
      }
  }
  
//  // Splice out any invisible elem wraps.
//  for (var i=0; i<elem_wraps.length; i++) {
//      if (!elem_wraps[i].is_visible()) {
//          elem_wraps.splice(i,1)
//          i -= 1
//      }
//  }
  
  // Build xpath to find all svg elem wraps.
  var xpath = "//*[local-name() = 'svg']"
  var svgs = this.get_by_xpath(xpath)
  
  // Loop over the elem wraps.
  var svg = null
  var rect = null
  for (var i=0; i<svgs.length; i++) {
      svg = svgs[i]
      
      // Make sure the svg is visible.
      if (!svg.is_visible()) {
          continue
      }
      
      // Make sure the svg contains the page point.
      if (!svg.contains_point(page_x, page_y)) {
          continue
      }
      
      // Add the svg to the elem wraps.
      elem_wraps.unshift(svg)
  }
  return elem_wraps
}

//Returns first elem wrap for which getter returns a "true-thy" value.
PageWrap.prototype.get_first_elem_wrap_at = function(getter, page_x, page_y) {
    var elem_wraps = this.get_visible_overlapping_at(page_x, page_y)
    for (var i=0; i<elem_wraps.length; i++) {
        var value = getter.call(elem_wraps[i])
        if (value) {
            return elem_wraps[i]
        }
    }
    return null
}

//Returns the top elem wrap with text, according to z-order.
PageWrap.prototype.get_top_text_elem_wrap_at = function(page_x, page_y) {
    var candidate = this.get_first_elem_wrap_at(ElemWrap.prototype.get_text, page_x, page_y)
    if (!candidate) {
        return null
    }
    return candidate
}

//Returns the top elem wrap with an image, according to z-order.
PageWrap.prototype.get_top_image_elem_wrap_at = function(page_x, page_y) {
    var candidate = this.get_first_elem_wrap_at(ElemWrap.prototype.get_image, page_x, page_y)
    if (!candidate) {
        return null
    }
    return candidate
}

//---------------------------------------------------------------------------------
//Find elem wraps by non mouse values.
//---------------------------------------------------------------------------------


//Returns an array of elem wraps with the given xpath.
PageWrap.prototype.get_by_xpath = function(xpath) {
  var set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
  // Convert to elem wraps.
  var elem_wraps = []
  for (var i=0; i<set.snapshotLength; i++) {
      var element = set.snapshotItem(i)
      // *Important!* We weed out any context menu hits here.
      if (g_event_blocker.gui_contains_element(element)) {
          continue
      }
      elem_wraps.push(new ElemWrap(element))
  }
  return elem_wraps
}

//Returns an array of all the elem wraps.
PageWrap.prototype.get_all = function() {
    var xpath = "//*";
    return this.get_by_xpath(xpath)
}

//Returns all elem wraps which intersect with the given box.
PageWrap.prototype.get_intersecting_with = function(page_box) {
    var intersecting = []
    var elem_wraps = this.get_all()
    for (var i=0; i<elem_wraps.length; i++) {
        if (page_box.intersects(elem_wraps[i].page_box)) {
            intersecting.push(elem_wraps[i])
        }
    }
    return intersecting
}

//Returns all elem wraps which are contained in the given box.
PageWrap.prototype.get_contained_in = function(page_box) {
    var contained = []
    var elem_wraps = this.get_all()
    for (var i=0; i<elem_wraps.length; i++) {
        if (page_box.contains(elem_wraps[i].page_box)) {
            contained.push(elem_wraps[i])
        }
    }
    return contained
}

// Returns elem wraps which match any of the target values.
PageWrap.prototype.get_by_any_value = function(wrap_type, target_values) {
    var getter = ElemWrap.prototype.get_getter_from_wrap_type(wrap_type)
        
    // Get all elem wraps.
    var elem_wraps = this.get_all()
    
    // Loop over the elem wraps.
    var wrapper = null
    var value = null
    var matches = []
    for (var i=0; i<elem_wraps.length; i++) {
        wrapper = elem_wraps[i]
                
        // Make sure the wrapper is visible.
        if (!wrapper.is_visible()) {
            continue
        }
        
        // Skip the elem wrap if it's part of the smash browse menu.
        if (g_event_blocker.gui_contains_element(wrapper.element)) {
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

//Returns an array of elem wraps selected by the matcher.
PageWrap.prototype.get_by_all_values = function(wrap_type, target_values) {
    // Get our initial candiate elem wraps.
    var candidates = this.get_by_any_value(wrap_type, target_values)
    
    // Get our getter.
    var getter = ElemWrap.prototype.get_getter_from_wrap_type(wrap_type)
    
    // Determines arrays of surrounding elem wraps for each elem wrap.
    var results = []
    var overlaps = []
    for (var i=0; i<candidates.length; i++) {
        overlaps.push([])
        for (var j=0; j<candidates.length; j++) {
            if (candidates[j].intersects(candidates[i])) {
                overlaps[i].push(getter.call(candidates[j]))
            }
        }
    }
    
    // Find the elem wraps which have surrounding elem wraps matching the target_values.
    var matching = []
    for (var i=0; i<overlaps.length; i++) {
        var found = true
        for (var j=0; j<target_values.length; j++) {
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
    var eliminated = []
    for (var i=0; i<matching.length; i++) {
        eliminated.push(false)
    }
    
    // Coalesce all contained elem wraps together
    // and find the smallest most internal element
    for (var i=0; i<matching.length; i++) {
        if (eliminated[i]) {
            continue
        }
        for(var j=0; j<matching.length; j++) {
            if (j==i) {
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
    var results = []
    for (var i=0; i<eliminated.length; i++) {
        if (!eliminated[i]) {
            results.push(matching[i])
        }
    }
    
    // Phew we're done!
    return results
}

//---------------------------------------------------------------------------------
//Value extraction.
//---------------------------------------------------------------------------------

//Returns an array of values obtained by looping through the 
//elem wraps and retrieving values using the supplied getter.
PageWrap.prototype.extract_values = function(elem_wraps, getter) {
    var values = []
    for (var i=0; i<elem_wraps.length; i++) {
        var value = getter.call(elem_wraps[i])
        if (!value || is_all_whitespace(value)) {
            continue
        }
        if (values.indexOf(value) >= 0) {
            continue
        }
        values.push(value)
    }
    return values
}

//Returns an array of images values from elem wraps under the given page point.
PageWrap.prototype.get_image_values_at = function(page_x, page_y) {
    var elem_wraps = this.get_visible_overlapping_at(page_x, page_y)
    var values = this.extract_values(elem_wraps, ElemWrap.prototype.get_image)
    return values
}

//Returns an array of text values from elem wraps under the given page point.
PageWrap.prototype.get_text_values_at = function(page_x, page_y) {
    var elem_wraps = this.get_visible_overlapping_at(page_x, page_y)
    var values = this.extract_values(elem_wraps, ElemWrap.prototype.get_text)
    // Values will contain text from bigger overlapping divs with text.
    // Unlike images text won't overlap, so we only want the first text.
    if (values.length > 1) {
        values.length = 1
    }
    return values
}

PageWrap.prototype.on_loaded = function() {
    // Disable hovers.
    g_page_wrap.disable_hover()
    
    // We don't allow iframes to initialize.
    if (window == window.top) {
        // Start mutation to timer, to try and detect when page is fully loaded.
        this.start_mutation_timer()
        
        // Listen to mutations.
        this.mutation_observer.observe(window.document, this.mutation_observer_config);
        
        // Initialize and open the wait popup.
        g_wait_popup.initialize()
        g_wait_popup.open()
        
        // Initialize the other popups.
        g_text_input_popup.initialize()
        g_select_input_popup.initialize()
    }
}


//---------------------------------------------------------------------------------
//Globals.
//---------------------------------------------------------------------------------

var g_page_wrap = new PageWrap()

//Start listening for mutations on page load.
document.addEventListener ('DOMContentLoaded', PageWrap.prototype.on_loaded.bind(g_page_wrap), false);


