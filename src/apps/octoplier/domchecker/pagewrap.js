//The class encapsulate properties of the target web page.
var PageWrap = function() {
}

//---------------------------------------------------------------------------------
//Page Properties.
//---------------------------------------------------------------------------------

// Returns the page height.
PageWrap.prototype.get_height = function() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max( doc_body.scrollHeight, doc_body.offsetHeight, 
            doc_elem.clientHeight, doc_elem.scrollHeight, doc_elem.offsetHeight );
}

// Returns the page width.
PageWrap.prototype.get_width = function() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max(doc_body.scrollWidth, doc_body.offsetWidth, 
            doc_elem.clientWidth, doc_elem.scrollWidth, doc_elem.offsetWidth );
}

// Disables hover behavior on the page.
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
//Elem Finders.
//---------------------------------------------------------------------------------

PageWrap.prototype.get_all_elem_wraps_at = function(page_x, page_y) {
    // Use the document.elementsFromPoint class.
    var elements = document.elementsFromPoint(page_x-window.scrollX, page_y-window.scrollY)
    // Convert to elem wraps.
    var elem_wraps = []
    for (var i=0; i<elements.length; i++) {
        elem_wraps.push(new ElemWrap(elements[i]))
    }
    return elem_wraps
}

//Returns the elem wraps in z-order under the given page point, until we become
//fully opaque. Usually we will hit an opaque background plate.
//The document.elementsFromPoint seems to skip svg elem wraps, so we add these in manually.
PageWrap.prototype.get_visible_elem_wraps_at = function(page_x, page_y) {
  var elem_wraps = this.get_all_elem_wraps_at(page_x, page_y)
  
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
  
  // Build xpath to find all svg elem wraps.
  var xpath = "//*[local-name() = 'svg']"
  var svgs = this.get_elem_wraps_by_xpath(xpath)
  
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
      if (!svg.contains_page_point(page_x, page_y)) {
          continue
      }
      
      // Add the svg to the elem wraps.
      elem_wraps.unshift(svg)
  }
  return elem_wraps
}

//Returns an array of element_wrappers with the given xpath.
PageWrap.prototype.get_elem_wraps_by_xpath = function(xpath) {
  var set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
  // Convert to elem wraps.
  var elem_wraps = []
  for (var i=0; i<set.snapshotLength; i++) {
      elem_wraps.push(new ElemWrap(set.snapshotItem(i)))
  }
  return elem_wraps
}

PageWrap.prototype.get_elem_wraps_by_any_value = function(getter, target_values) {
    // Build xpath to find all elem wraps.
    var xpath = "//*"
    var elem_wraps = this.get_elem_wraps_by_xpath(xpath)
    
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
        if (g_context_menu.element_is_part_of_menu(wrapper.element)) {
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
PageWrap.prototype.get_elem_wraps_by_values = function(getter, target_values) {
    // Get our initial candiate elem wraps.
    var candidates = this.get_elem_wraps_by_any_value(getter, target_values)
    
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

//Returns an array of elem wraps which have matching image values.
PageWrap.prototype.get_elem_wraps_by_image_values = function(image_values) {
    return this.get_elem_wraps_by_values(ElemWrap.prototype.get_image, image_values)
}

//Returns an array of elem wraps which have matching text values.
PageWrap.prototype.get_elem_wraps_by_text_values = function(text_values) {
    return this.get_elem_wraps_by_values(ElemWrap.prototype.get_text, text_values)
}

//Returns an array of elem wraps which have the matching tag name.
//Note when finding images or text you should use other methods
//as they appear in other ways besides the use of specific tag names like img or h2 etc.
PageWrap.prototype.get_elem_wraps_by_tag_name = function(tag_name) {
    return this.get_elem_wraps_by_values(ElemWrap.prototype.get_tag_name, [tag_name])
}

//Returns all elem wraps that contain images.
PageWrap.prototype.get_elem_wraps_with_images = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.get_image, [])
}

//Returns all elem wraps that contain text.
PageWrap.prototype.get_elem_wraps_with_text = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.get_text, [])
}

//Returns all elem wraps that contain inputs.
PageWrap.prototype.get_elem_wraps_with_inputs = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.get_tag_name, ['input'])
}

//Returns all elem wraps that contain drop downs.
PageWrap.prototype.get_elem_wraps_with_selects = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.get_tag_name, ['select'])
}

//Returns all elem wraps that have a vertical scroll bars.
PageWrap.prototype.get_elem_wraps_with_vertical_scroll_bars = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.has_vertical_scroll_bar, [])
}

//Returns all elem wraps that have a horizontal scroll bars.
PageWrap.prototype.get_elem_wraps_with_horizontal_scroll_bars = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.has_horizontal_scroll_bar, [])
}

//Returns all elem wraps that have a any scroll bars.
PageWrap.prototype.get_elem_wraps_with_any_scroll_bars = function() {
    return this.get_elem_wraps_by_any_value(ElemWrap.prototype.has_any_scroll_bar, [])
}

//Returns first elem wrap for which getter returns a "true-thy" value.
PageWrap.prototype.get_first_elem_wrap_at = function(getter, page_x, page_y) {
    var elem_wraps = this.get_visible_elem_wraps_at(page_x, page_y)
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
    return this.get_first_elem_wrap_at(ElemWrap.prototype.get_text, page_x, page_y)
}

//Returns the top elem wrap with an image, according to z-order.
PageWrap.prototype.get_top_image_elem_wrap_at = function(page_x, page_y) {
    return this.get_first_elem_wrap_at(ElemWrap.prototype.get_image, page_x, page_y)
}

// Returns the top elem wrap with either and image or text, according to z-order.
PageWrap.prototype.get_top_text_or_image_elem_wrap_at = function(page_x, page_y) {
    return this.get_first_elem_wrap_at(ElemWrap.prototype.get_text_or_image, page_x, page_y)
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
    var elem_wraps = this.get_visible_elem_wraps_at(page_x, page_y)
    var values = this.extract_values(elem_wraps, ElemWrap.prototype.get_image)
    return values
}

//Returns an array of text values from elem wraps under the given page point.
PageWrap.prototype.get_text_values_at = function(page_x, page_y) {
    var elem_wraps = this.get_visible_elem_wraps_at(page_x, page_y)
    var values = this.extract_values(elem_wraps, ElemWrap.prototype.get_text)
    // Values will contain text from bigger overlapping divs with text.
    // Unlike images text won't overlap, so we only want the first text.
    if (values.length > 1) {
        values.length = 1
    }
    return values
}

var g_page_wrap = new PageWrap()