var MatchCriteria = function() {
    if (arguments.length == 0) {
        // Border matching.
        this.match_left = false
        this.match_right = false
        this.match_top = false
        this.match_bottom = false
        
        // Font matching.
        this.match_font = false
        this.match_font_size = false
    } else if (arguments.length == 1) {
        // If there's one argument we expect it to be dictionary ok key-value pairs.
        // Another MatchCriteria object will also have the appropriate key-value pairs.
        var obj = arguments[0]
        
        // Border matching.
        this.match_left = obj.match_left
        this.match_right = obj.match_right
        this.match_top = obj.match_top
        this.match_bottom = obj.match_bottom
        
        // Font matching.
        this.match_font = obj.match_font
        this.match_font_size = obj.match_font_size
    }
}

//Returns true if the two elem wraps match, according to the match criteria.
MatchCriteria.prototype.matches =function(a, b) {

  if (this.match_left && (a.page_box.left != b.page_box.left)) {
      return false
  }
  
  if (this.match_right && (a.page_box.right != b.page_box.right)) {
      return false
  }

  if (this.match_top && (a.page_box.top != b.page_box.top)) {
      return false
  }
  
  if (this.match_bottom && (a.page_box.bottom != b.page_box.bottom)) {
      return false
  }

  // Match font style.
  if (this.match_font || this.match_font_size) {
      var style_a = window.getComputedStyle(a.element, null)
      var style_b = window.getComputedStyle(b.element, null)

      if (this.match_font && (style_a.font != style_b.font)) {
          return false
      }
      if (this.match_font_size && (style_a.fontSize != style_b.fontSize)) {
          return false
      }
  }
  return true
}