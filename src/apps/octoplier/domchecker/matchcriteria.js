var MatchCriteria = function() {
    // Border matching.
    this.match_left = false
    this.match_right = false
    this.match_top = false
    this.match_bottom = false
    
    // Font matching.
    this.match_font = false
    this.match_font_size = false
}

//Returns true if the two elem wraps match, according to the match criteria.
MatchCriteria.prototype.matches =function(a, b) {

  if (match_criteria.match_left && (a.page_box.left != b.page_box.left)) {
      return false
  }
  
  if (match_criteria.match_right && (a.page_box.right != b.page_box.right)) {
      return false
  }

  if (match_criteria.match_top && (a.page_box.top != b.page_box.top)) {
      return false
  }
  
  if (match_criteria.match_bottom && (a.page_box.bottom != b.page_box.bottom)) {
      return false
  }

  // Match font style.
  if (match_criteria.match_font || match_criteria.match_font_size) {
      var style_a = window.getComputedStyle(a.element, null)
      var style_b = window.getComputedStyle(b.element, null)

      if (match_criteria.match_font && (style_a.font != style_b.font)) {
          return false
      }
      if (match_criteria.match_font_size && (style_a.fontSize != style_b.fontSize)) {
          return false
      }
  }
  return true
}