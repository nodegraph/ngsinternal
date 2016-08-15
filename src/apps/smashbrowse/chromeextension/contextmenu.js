//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//1) The smash_browse prefix prevents collisions with js in the target web page.
//2) Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.

var ContextMenu = function() {
    this.top_menu = null
    this.on_click_bound = null
    this.visible = false
}

ContextMenu.prototype.initialized = function() {
    if (this.top_menu) {
        return true
    }
    return false
}

ContextMenu.prototype.initialize = function() {
    if (this.initialized()) {
        return
    }
    // Create the top level menu element.
    this.top_menu = document.createElement("menu");
    this.top_menu.classList.add('smash_browse_menu')
    document.body.appendChild(this.top_menu)
    
    // Bind the click handler to ourself.
    this.on_click_bound = this.on_click.bind(this)
    
    // Create the menu as dom elements.
    this.create_menu(this.top_menu)
    
    // Create our mouse overlays.
    this.text_box_overlay = new Overlay('smash_browse_text_box', g_distinct_colors.text_color, -1, false, null)
    this.image_box_overlay = new Overlay('smash_browse_image_box', g_distinct_colors.image_color, -1, false, null)
}

ContextMenu.prototype.create_menu = function (top_menu) {
    
    this.test2 = this.add_item(this.top_menu, 'TEST get all cookies')
    this.test3 = this.add_item(this.top_menu, 'TEST clear all cookies')
    this.test4 = this.add_item(this.top_menu, 'TEST set all cookies')
    
    // The find specific menu.
    this.navigate_menu = this.add_sub_menu(top_menu, 'Navigate')
    this.navigate_to_url = this.add_item(this.navigate_menu, 'to url')
//    this.navigate_back = this.add_item(this.navigate_menu, 'back')
//    this.navigate_forward = this.add_item(this.navigate_menu, 'forward')
    this.navigate_refresh = this.add_item(this.navigate_menu, 'refresh')
    
    // Spacer.
    this.add_spacer(top_menu)
    
    // The find specific menu.
    this.create_set_by_matching_menu = this.add_sub_menu(top_menu, 'Create set from values under mouse')
    this.create_set_by_matching_text = this.add_item(this.create_set_by_matching_menu, 'text')
    this.create_set_by_matching_image = this.add_item(this.create_set_by_matching_menu, 'image')
//    this.create_set_by_matching_position = this.add_item(this.create_set_by_matching_menu, 'position')
//    this.create_set_by_matching_xpath = this.add_item(this.create_set_by_matching_menu, 'xpath')
    
    // The find all menu.
    this.create_set_by_type_menu = this.add_sub_menu(top_menu, 'Create set from elements of type')
    this.create_set_from_inputs = this.add_item(this.create_set_by_type_menu, 'has an input (text field)')
    this.create_set_from_selects = this.add_item(this.create_set_by_type_menu, 'has a select (drop down)')
    this.create_set_from_texts = this.add_item(this.create_set_by_type_menu, 'has text')
    this.create_set_from_images = this.add_item(this.create_set_by_type_menu, 'has an image')
    
    // Delete
    this.delete_set = this.add_item(this.top_menu, 'Delete set')
    
    // Spacer.
    this.add_spacer(top_menu)

    // The shift elements menu.
    this.shift_to_text_menu = this.add_sub_menu(top_menu, 'Shift set elements to text')
    this.shift_up_by_text = this.add_item(this.shift_to_text_menu, 'above')
    this.shift_down_by_text = this.add_item(this.shift_to_text_menu, 'below')
    this.shift_left_by_text = this.add_item(this.shift_to_text_menu, 'on left')
    this.shift_right_by_text = this.add_item(this.shift_to_text_menu, 'on right')
    
    this.shift_to_image_menu = this.add_sub_menu(top_menu, 'Shift set elements to images')
    this.shift_up_by_image = this.add_item(this.shift_to_image_menu, 'above')
    this.shift_down_by_image = this.add_item(this.shift_to_image_menu, 'below')
    this.shift_left_by_image = this.add_item(this.shift_to_image_menu, 'on left')
    this.shift_right_by_image = this.add_item(this.shift_to_image_menu, 'on right')
    
    this.shift_to_input_menu = this.add_sub_menu(top_menu, 'Shift set elements to inputs')
    this.shift_up_by_input = this.add_item(this.shift_to_input_menu, 'above')
    this.shift_down_by_input = this.add_item(this.shift_to_input_menu, 'below')
    this.shift_left_by_input = this.add_item(this.shift_to_input_menu, 'on left')
    this.shift_right_by_input = this.add_item(this.shift_to_input_menu, 'on right')

    this.shift_to_selector_menu = this.add_sub_menu(top_menu, 'Shift set elements to selects')
    this.shift_up_by_select = this.add_item(this.shift_to_selector_menu, 'above')
    this.shift_down_by_select = this.add_item(this.shift_to_selector_menu, 'below')
    this.shift_left_by_select = this.add_item(this.shift_to_selector_menu, 'on left')
    this.shift_right_by_select = this.add_item(this.shift_to_selector_menu, 'on right')
    
    // Spacer.
    this.add_spacer(top_menu)
        
    // The expand menu.
    this.expand_menu = this.add_sub_menu(top_menu, 'Expand set to similar elements')
    this.expand_above = this.add_item(this.expand_menu, 'above')
    this.expand_below = this.add_item(this.expand_menu, 'below')
    this.expand_left = this.add_item(this.expand_menu, 'on left')
    this.expand_right = this.add_item(this.expand_menu, 'on right')
    
    // Spacer.
    this.add_spacer(top_menu)
    
    // Mark sets.
    this.mark_set = this.add_item(this.top_menu, 'Mark set')
    this.unmark_set = this.add_item(this.top_menu, 'Unmark set')
    
    // Merge marked sets.
    this.merge_marked_sets = this.add_item(this.top_menu, 'Merge marked sets')
    
    // The shrink to marked menu.
    this.shrink_set_to_marked_menu = this.add_sub_menu(top_menu, 'Shrink set w.r.t. marked sets' )
    this.shrink_set_above_marked = this.add_item(this.shrink_set_to_marked_menu, 'above marked set element')
    this.shrink_set_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'below marked set elements')
    this.shrink_set_above_and_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'above and below marked set elements')
    this.shrink_set_left_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left of marked set elements')
    this.shrink_set_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'right of marked set elements')
    this.shrink_set_left_and_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left and right of marked set elements')
    
    // The shrink down all sets menu.
    this.shrink_set_menu = this.add_sub_menu(top_menu, 'Shrink set to' )
    this.shrink_set_to_topmost = this.add_item(this.shrink_set_menu, 'topmost element')
    this.shrink_set_to_bottommost= this.add_item(this.shrink_set_menu, 'bottommost element')
    this.shrink_set_to_leftmost = this.add_item(this.shrink_set_menu, 'leftmost element')
    this.shrink_set_to_rightmost = this.add_item(this.shrink_set_menu, 'rightmost element')
    
    // Spacer.
    this.spacer = this.add_spacer(this.top_menu)
    
    // Act on element.
    this.perform_menu = this.add_sub_menu(this.top_menu, 'Perform action on set with one element')
    this.perform_click = this.add_item(this.perform_menu, 'click')
    this.perform_type = this.add_item(this.perform_menu, 'type text')
    this.perform_enter = this.add_item(this.perform_menu, 'press enter/submit')
    this.perform_extract = this.add_item(this.perform_menu, 'extract text')
    this.perform_select_option = this.add_item(this.perform_menu, 'select option')
    this.perform_scroll_down = this.add_item(this.perform_menu, 'scroll down')
    this.perform_scroll_up = this.add_item(this.perform_menu, 'scroll up')
    this.perform_scroll_right = this.add_item(this.perform_menu, 'scroll right')
    this.perform_scroll_left = this.add_item(this.perform_menu, 'scroll left')
    
    // Spacer.
    this.spacer = this.add_spacer(this.top_menu)
    
    // Cancel.
    this.cancel = this.add_item(this.top_menu, 'Cancel')
}

ContextMenu.prototype.add_item = function(menu, item_text) {
    var li = document.createElement('li')
    li.classList.add('smash_browse_menu_item')
    menu.appendChild(li)
    
    var button = document.createElement('button')
    button.classList.add('smash_browse_menu_button')
    button.type = 'button'
    li.appendChild(button)
    
    var span = document.createElement('span')
    span.classList.add('smash_browse_menu_text')
    button.appendChild(span)
    
    var text = document.createTextNode(item_text)
    span.appendChild(text)
    return li
}

ContextMenu.prototype.add_sub_menu = function(menu, item_text) {
    var li = document.createElement('li')
    li.classList.add('smash_browse_menu_item')
    li.classList.add('smash_browse_submenu')
    this.top_menu.appendChild(li)
    
    var button = document.createElement('button')
    button.classList.add('smash_browse_menu_button')
    button.type = 'button'
    li.appendChild(button)
    
    var span = document.createElement('span')
    span.classList.add('smash_browse_menu_text')
    button.appendChild(span)
    
    var text = document.createTextNode(item_text)
    span.appendChild(text)
    
    var menu = document.createElement("menu");
    menu.classList.add('smash_browse_menu')
    li.appendChild(menu)
    return menu
}

ContextMenu.prototype.remove_sub_menu = function(menu, sub_menu) {
    menu.removeChild(sub_menu)
}

ContextMenu.prototype.add_spacer = function(menu) {
    var li = document.createElement('li')
    li.classList.add('smash_browse_menu_separator')
    menu.appendChild(li)
    return li
}

ContextMenu.prototype.disable_item = function(item) {
    item.classList.add('smash_browse_disabled')
}

ContextMenu.prototype.enable_item = function(item) {
    item.classList.remove('smash_browse_disabled')
}

ContextMenu.prototype.show = function(x,y) {
    this.top_menu.style.left = x + 'px'
    this.top_menu.style.top = y + 'px'
    this.top_menu.classList.add('smash_browse_menu_show')
    this.visible = true
}

ContextMenu.prototype.hide = function(x,y) {
    this.top_menu.classList.remove('smash_browse_menu_show')
    this.visible = false
}

ContextMenu.prototype.contains_element = function(element) {
    if (!this.top_menu) {
        return false
    }
    if (this.top_menu.contains(element)) {
        return true
    }
    return false
}

ContextMenu.prototype.on_context_menu = function(page_event) {
    this.page_x = page_event.pageX
    this.page_y = page_event.pageY
    
    var text_values = g_page_wrap.get_text_values_at(this.page_x, this.page_y)
    var image_values = g_page_wrap.get_image_values_at(this.page_x, this.page_y)
    
    if (text_values.length == 0) {
        this.disable_item(this.create_set_by_matching_text)
    } else {
        this.enable_item(this.create_set_by_matching_text)
    }
    if (image_values.length == 0) {
        this.disable_item(this.create_set_by_matching_image)
    } else {
        this.enable_item(this.create_set_by_matching_image)
    }
    
    // Show the menu.
    this.show(page_event.pageX, page_event.pageY)
    
    // Listen to clicks to perform the according action and close the menu.
    document.addEventListener('click', this.on_click_bound, true);
    
    page_event.preventDefault();
    return false
}

ContextMenu.prototype.on_click = function(menu_event) {
    this.hide();
    var menu_target = menu_event.target
    
    if (this.navigate_to_url.contains(menu_target)) { 
        function goto_url(url) {
            g_content_comm.send_to_bg({request: 'navigate_to', url: url})
        }
        g_text_input_popup.set_label_text("Enter URL")
        g_text_input_popup.open(goto_url)
    } 
    
    else if (this.test2.contains(menu_target)) {
        g_content_comm.send_to_bg({request: 'get_all_cookies'})
    }
    
    else if (this.test3.contains(menu_target)) {
        g_content_comm.send_to_bg({request: 'clear_all_cookies'})
    }
    
    else if (this.test4.contains(menu_target)) {
        var request = {
                request: 'set_all_cookies',
                //cookies: [{"domain":".amazon.com","hostOnly":false,"httpOnly":false,"name":"skin","path":"/","sameSite":"no_restriction","secure":false,"session":true,"storeId":"0","value":"noskin"},{"domain":".amazon.com","expirationDate":2082787199.580167,"hostOnly":false,"httpOnly":false,"name":"x-wl-uid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1ArGOUxyFslBzszDsCEFQk1H69mkZ5Gatx2VKrn2bLqxpFui2OmlWPiCuEr90jMjQ9D2sGWJlLWc="},{"domain":".amazon.com","expirationDate":2100094735.549925,"hostOnly":false,"httpOnly":false,"name":"session-token","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"7UwoKYIG1AesDAQeL5+GDkgFzGsxGyagTdgPF0h+c/ag1aWJgfRgkRr8BZeVwXu07nUpXNA9mR5/iurZVh4uMabvUOmhOSIA8BsrN4IrH0jtU/yzNkYds3kESL22jkfnXueMYy7U6L+8tln08mmI2epDBqzBth3ErT5hqSSlc5C5rE0cRdUZbppjsqmJIN1uLpgYkloQGDgtdt46Iote3dPX3dp7kPHqPrRHz4IvbHMQPDbauT/zhsu03/ew/ru/"},{"domain":"media.charter.com","hostOnly":true,"httpOnly":false,"name":"XGIR","path":"/","sameSite":"no_restriction","secure":false,"session":true,"storeId":"0","value":"oVe3sa3+oCMM6XAqesACXt|w63WeXE|oi|NISDI|f5|RIR|RDr|-YlD|"},{"domain":".charter.com","expirationDate":1469633935.828551,"hostOnly":false,"httpOnly":false,"name":"TE_RPN","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"855.818.5773"},{"domain":".charter.com","expirationDate":1477150735.828622,"hostOnly":false,"httpOnly":false,"name":"CSList","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"32331289/32333390,0/0,0/0,0/0,0/0"},{"domain":".charter.com","expirationDate":1532489936.092919,"hostOnly":false,"httpOnly":false,"name":"PrefID","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"123-2376042924"},{"domain":".amazon-adsystem.com","expirationDate":2114380799.66266,"hostOnly":false,"httpOnly":false,"name":"ad-id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"A3pGBBAQuE7ykvE5mbjHc1w"},{"domain":".amazon-adsystem.com","expirationDate":2114380799.66272,"hostOnly":false,"httpOnly":false,"name":"ad-privacy","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0"},{"domain":".contextweb.com","expirationDate":1500478736.912202,"hostOnly":false,"httpOnly":false,"name":"V","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"wcfutv4xnaFw"},{"domain":".contextweb.com","expirationDate":1469375636.912243,"hostOnly":false,"httpOnly":false,"name":"sto-id-20480-bh","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"CHANNCAKJBBP"},{"domain":".adnxs.com","expirationDate":1469461136.9297,"hostOnly":false,"httpOnly":true,"name":"sess","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1"},{"domain":".adnxs.com","expirationDate":1477150736.929742,"hostOnly":false,"httpOnly":true,"name":"uuid2","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"7256487675190112956"},{"domain":".bluekai.com","expirationDate":1484926736.938499,"hostOnly":false,"httpOnly":false,"name":"bkdc","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"snv"},{"domain":".twitter.com","expirationDate":1532446736.959029,"hostOnly":false,"httpOnly":false,"name":"guest_id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"v1%3A146937473896380188"},{"domain":".pubmatic.com","expirationDate":1532446736.959423,"hostOnly":false,"httpOnly":false,"name":"KADUSERCOOKIE","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"B39CE15D-D359-4070-93AF-2B2FCF564904"},{"domain":".btrll.com","expirationDate":1500910736.970503,"hostOnly":false,"httpOnly":false,"name":"BR_APS","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"3V5ThEh_0lvsBIlzBKA"},{"domain":".liverail.com","expirationDate":1477150736.971112,"hostOnly":false,"httpOnly":true,"name":"lr_uid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"3.1469374738979.2645092266224913592"},{"domain":".liverail.com","expirationDate":1471966736.971148,"hostOnly":false,"httpOnly":true,"name":"lr_uds","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"a%3A1%3A%7Bi%3A3013%3Bi%3A1469374738%3B%7D"},{"domain":".openx.net","expirationDate":1500910736.972789,"hostOnly":false,"httpOnly":false,"name":"i","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0d7de61d-3d69-42f9-c660-dac3a1b6d653|1469374738"},{"domain":".adaptv.advertising.com","expirationDate":1532446739,"hostOnly":false,"httpOnly":false,"name":"adaptv_unique_user_cookie","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"\"4688202828396573246__TIME__2016-07-24+08%3A38%3A59\""},{"domain":".advertising.com","expirationDate":1500997136.986348,"hostOnly":false,"httpOnly":false,"name":"APID","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"VBbd1bf340-51b4-11e6-871c-02875268b68b"},{"domain":".yahoo.com","expirationDate":1532533137.041679,"hostOnly":false,"httpOnly":false,"name":"B","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"apuidghbp9o8j&b=3&s=45"},{"domain":".doubleclick.net","expirationDate":1532446737.056996,"hostOnly":false,"httpOnly":false,"name":"id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"22bed4868e080097||t=1469374739|et=730|cs=002213fd48d3b91666662d33a3"},{"domain":".doubleclick.net","expirationDate":1532446737.057073,"hostOnly":false,"httpOnly":true,"name":"IDE","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"AHWqTUkXQJ5daxZur-OgrzXW5VETdHfU3ybxmFQnr6mP13jpdYgz_Wi7Zg"},{"domain":".bluekai.com","expirationDate":1484926737.090603,"hostOnly":false,"httpOnly":false,"name":"bku","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"b/X99OnCkazyiMRV"},{"domain":".bidswitch.net","expirationDate":1500910737.199754,"hostOnly":false,"httpOnly":false,"name":"c","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1469374739"},{"domain":".rubiconproject.com","expirationDate":1500910738.226701,"hostOnly":false,"httpOnly":false,"name":"cd","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"false"},{"domain":".rubiconproject.com","expirationDate":1500910738.226742,"hostOnly":false,"httpOnly":false,"name":"au","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"IR0RS4Y8-HNVT-10.183.95.119"},{"domain":"tap.rubiconproject.com","expirationDate":1500910738.226769,"hostOnly":true,"httpOnly":false,"name":"dq","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1|1|0|0"},{"domain":".bidswitch.net","expirationDate":1500910737.233113,"hostOnly":false,"httpOnly":false,"name":"tuuid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0e7e525b-b584-4e82-a003-af2d7105785f"},{"domain":".bidswitch.net","expirationDate":1500910737.233157,"hostOnly":false,"httpOnly":false,"name":"tuuid_last_update","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1469374739"},{"domain":".adtechus.com","hostOnly":false,"httpOnly":false,"name":"CfP","path":"/","sameSite":"no_restriction","secure":false,"session":true,"storeId":"0","value":"1"},{"domain":".spotxchange.com","expirationDate":1471966739,"hostOnly":false,"httpOnly":false,"name":"partner-1469374739_4cef-0","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"eNplzNEOgiAUANDn%2FuYGVtM3V%2BZggksdcHnTVgvEra02ha%2FvAzofcE45IbuCF4%2FIVzQcpkVFQ1Wyhj9HfXOt4zOS7iXTTG0Pe6EZNEO12QG%2F6GUQDjZMZbRagPXlKi7CNef%2Fy9ZXwJ4dmWMftqjsXoeIpntPJHOtr0Cm6iA9UpFC%2FgMP7zDA"},{"domain":".spotxchange.com","expirationDate":1471966739,"hostOnly":false,"httpOnly":false,"name":"user-0","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"dXNlcl9ndWlkCWRjZGE3OTdlNTFiNDExZTZiM2UyMWMwMmVjNGYwMDAzCWF1ZGllbmNlX2lkCWRjZGE3OTdlLTUxYjQtMTFlNi1iM2UyLTFjMDJlYzRmMDAwMwljcmVhdGVkX2RhdGUJMTQ2OTM3NDczOQltb2RpZmllZF9kYXRlCTE0NjkzNzQ3Mzk="},{"domain":".adtechus.com","expirationDate":1532446738,"hostOnly":false,"httpOnly":false,"name":"JEB2","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"5794D8166E65074490B52083FCCBF6B0"},{"domain":".1rx.io","expirationDate":1532446737.677317,"hostOnly":false,"httpOnly":true,"name":"_rxuuid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"%7B%22rx_uuid%22%3A%22RX-a4930b9e-9bab-4b5e-a7d9-da05a0c9b788%22%7D"},{"domain":".rhythmxchange.com","expirationDate":1532446738.212251,"hostOnly":false,"httpOnly":true,"name":"_rxuuid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"%7B%22rx_uuid%22%3A%22RX-a4930b9e-9bab-4b5e-a7d9-da05a0c9b788%22%7D"},{"domain":"www.amazon.com","expirationDate":1469979534,"hostOnly":true,"httpOnly":false,"name":"csm-hit","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"R7SBQM9DTWXX6RF3K8NR+s-R7SBQM9DTWXX6RF3K8NR|1469374745127"},{"domain":".amazon.com","expirationDate":2082787199.566948,"hostOnly":false,"httpOnly":false,"name":"ubid-main","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"168-0790093-5784324"},{"domain":".amazon.com","expirationDate":2082787199.567006,"hostOnly":false,"httpOnly":false,"name":"session-id-time","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"2082787201l"},{"domain":".amazon.com","expirationDate":2082787199.56705,"hostOnly":false,"httpOnly":false,"name":"session-id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"154-9580678-9787649"}]
                cookies: [{"domain":".media.charter.com","hostOnly":false,"httpOnly":false,"name":"XGIR","path":"/","sameSite":"no_restriction","secure":false,"session":true,"storeId":"0","value":"oVe3sa3+oCMM6XAqesACXt|w63WeXE|oi|NISDI|f5|RIR|RDr|-YlD|"},{"domain":".amazon-adsystem.com","expirationDate":2114380799.66266,"hostOnly":false,"httpOnly":false,"name":"ad-id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"A3pGBBAQuE7ykvE5mbjHc1w"},{"domain":".amazon-adsystem.com","expirationDate":2114380799.66272,"hostOnly":false,"httpOnly":false,"name":"ad-privacy","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0"},{"domain":".contextweb.com","expirationDate":1500478736.912202,"hostOnly":false,"httpOnly":false,"name":"V","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"wcfutv4xnaFw"},{"domain":".adnxs.com","expirationDate":1469461136.9297,"hostOnly":false,"httpOnly":true,"name":"sess","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1"},{"domain":".adnxs.com","expirationDate":1477150736.929742,"hostOnly":false,"httpOnly":true,"name":"uuid2","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"7256487675190112956"},{"domain":".bluekai.com","expirationDate":1484926736.938499,"hostOnly":false,"httpOnly":false,"name":"bkdc","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"snv"},{"domain":".twitter.com","expirationDate":1532446736.959029,"hostOnly":false,"httpOnly":false,"name":"guest_id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"v1%3A146937473896380188"},{"domain":".pubmatic.com","expirationDate":1532446736.959423,"hostOnly":false,"httpOnly":false,"name":"KADUSERCOOKIE","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"B39CE15D-D359-4070-93AF-2B2FCF564904"},{"domain":".btrll.com","expirationDate":1500910736.970503,"hostOnly":false,"httpOnly":false,"name":"BR_APS","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"3V5ThEh_0lvsBIlzBKA"},{"domain":".liverail.com","expirationDate":1477150736.971112,"hostOnly":false,"httpOnly":true,"name":"lr_uid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"3.1469374738979.2645092266224913592"},{"domain":".liverail.com","expirationDate":1471966736.971148,"hostOnly":false,"httpOnly":true,"name":"lr_uds","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"a%3A1%3A%7Bi%3A3013%3Bi%3A1469374738%3B%7D"},{"domain":".openx.net","expirationDate":1500910736.972789,"hostOnly":false,"httpOnly":false,"name":"i","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0d7de61d-3d69-42f9-c660-dac3a1b6d653|1469374738"},{"domain":".adaptv.advertising.com","expirationDate":1532446739,"hostOnly":false,"httpOnly":false,"name":"adaptv_unique_user_cookie","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"\"4688202828396573246__TIME__2016-07-24+08%3A38%3A59\""},{"domain":".advertising.com","expirationDate":1500997136.986348,"hostOnly":false,"httpOnly":false,"name":"APID","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"VBbd1bf340-51b4-11e6-871c-02875268b68b"},{"domain":".yahoo.com","expirationDate":1532533137.041679,"hostOnly":false,"httpOnly":false,"name":"B","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"apuidghbp9o8j&b=3&s=45"},{"domain":".doubleclick.net","expirationDate":1532446737.056996,"hostOnly":false,"httpOnly":false,"name":"id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"22bed4868e080097||t=1469374739|et=730|cs=002213fd48d3b91666662d33a3"},{"domain":".doubleclick.net","expirationDate":1532446737.057073,"hostOnly":false,"httpOnly":true,"name":"IDE","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"AHWqTUkXQJ5daxZur-OgrzXW5VETdHfU3ybxmFQnr6mP13jpdYgz_Wi7Zg"},{"domain":".bluekai.com","expirationDate":1484926737.090603,"hostOnly":false,"httpOnly":false,"name":"bku","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"b/X99OnCkazyiMRV"},{"domain":".bidswitch.net","expirationDate":1500910737.199754,"hostOnly":false,"httpOnly":false,"name":"c","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1469374739"},{"domain":".rubiconproject.com","expirationDate":1500910738.226701,"hostOnly":false,"httpOnly":false,"name":"cd","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"false"},{"domain":".rubiconproject.com","expirationDate":1500910738.226742,"hostOnly":false,"httpOnly":false,"name":"au","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"IR0RS4Y8-HNVT-10.183.95.119"},{"domain":".tap.rubiconproject.com","expirationDate":1500910738.226769,"hostOnly":false,"httpOnly":false,"name":"dq","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1|1|0|0"},{"domain":".bidswitch.net","expirationDate":1500910737.233113,"hostOnly":false,"httpOnly":false,"name":"tuuid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0e7e525b-b584-4e82-a003-af2d7105785f"},{"domain":".bidswitch.net","expirationDate":1500910737.233157,"hostOnly":false,"httpOnly":false,"name":"tuuid_last_update","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1469374739"},{"domain":".adtechus.com","hostOnly":false,"httpOnly":false,"name":"CfP","path":"/","sameSite":"no_restriction","secure":false,"session":true,"storeId":"0","value":"1"},{"domain":".spotxchange.com","expirationDate":1471966739,"hostOnly":false,"httpOnly":false,"name":"partner-1469374739_4cef-0","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"eNplzNEOgiAUANDn%2FuYGVtM3V%2BZggksdcHnTVgvEra02ha%2FvAzofcE45IbuCF4%2FIVzQcpkVFQ1Wyhj9HfXOt4zOS7iXTTG0Pe6EZNEO12QG%2F6GUQDjZMZbRagPXlKi7CNef%2Fy9ZXwJ4dmWMftqjsXoeIpntPJHOtr0Cm6iA9UpFC%2FgMP7zDA"},{"domain":".spotxchange.com","expirationDate":1471966739,"hostOnly":false,"httpOnly":false,"name":"user-0","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"dXNlcl9ndWlkCWRjZGE3OTdlNTFiNDExZTZiM2UyMWMwMmVjNGYwMDAzCWF1ZGllbmNlX2lkCWRjZGE3OTdlLTUxYjQtMTFlNi1iM2UyLTFjMDJlYzRmMDAwMwljcmVhdGVkX2RhdGUJMTQ2OTM3NDczOQltb2RpZmllZF9kYXRlCTE0NjkzNzQ3Mzk="},{"domain":".adtechus.com","expirationDate":1532446738,"hostOnly":false,"httpOnly":false,"name":"JEB2","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"5794D8166E65074490B52083FCCBF6B0"},{"domain":".1rx.io","expirationDate":1532446737.677317,"hostOnly":false,"httpOnly":true,"name":"_rxuuid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"%7B%22rx_uuid%22%3A%22RX-a4930b9e-9bab-4b5e-a7d9-da05a0c9b788%22%7D"},{"domain":".rhythmxchange.com","expirationDate":1532446738.212251,"hostOnly":false,"httpOnly":true,"name":"_rxuuid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"%7B%22rx_uuid%22%3A%22RX-a4930b9e-9bab-4b5e-a7d9-da05a0c9b788%22%7D"},{"domain":".www.amazon.com","expirationDate":1469979534,"hostOnly":false,"httpOnly":false,"name":"csm-hit","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"R7SBQM9DTWXX6RF3K8NR+s-R7SBQM9DTWXX6RF3K8NR|1469374745127"},{"domain":".charter.com","expirationDate":1469644134.636339,"hostOnly":false,"httpOnly":false,"name":"TE_RPN","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"855.818.5773"},{"domain":".charter.com","expirationDate":1477160934.636373,"hostOnly":false,"httpOnly":false,"name":"CSList","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"32331289/32333390,32331289/32333390,0/0,0/0,0/0"},{"domain":".charter.com","expirationDate":1532500134.72454,"hostOnly":false,"httpOnly":false,"name":"PrefID","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"123-2376042924"},{"domain":".amazon.com","expirationDate":1469385928.633002,"hostOnly":false,"httpOnly":false,"name":"a-ogbcbff","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1"},{"domain":".amazon.com","expirationDate":2100105088.633124,"hostOnly":false,"httpOnly":false,"name":"x-main","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"YmFgvX5CRhxhQDVs0mR6qozVcfgr9GU5"},{"domain":".amazon.com","expirationDate":2100105088.633154,"hostOnly":false,"httpOnly":true,"name":"at-main","path":"/","sameSite":"no_restriction","secure":true,"session":false,"storeId":"0","value":"Atza|IwEBIO-i74Sm2j7YCOWVSDboH8vQymSJ_2RT_l_bXKe9Sr70yUyGeyrr6t9ofu3j6XyhYfKWUpUNv-Mit4as8sjg7QnR9fcwdy2EHKSp3au_LGlc0ymU2izTS51SOrdiIBhHa8M3KdDW6ChojocnGBn0coXbDcZl2PeizFpnPxEOq1RzqHEliIouk-JpXhk0eWUgroH8Z0Og0DxshJjPM5WAhmLhkloHYHZobCHVpVE_rcfcees9n2R8gbBX-XqQBvLyRLdJD96wfmTQ_lgmOolZqAe1vQ5ZlQXanw4_DErMEymKW4T0EfIi3iSWiZ_nmz7v6Rutzjq4yR87nPdrVRHar5-F"},{"domain":".amazon.com","hostOnly":false,"httpOnly":true,"name":"sess-at-main","path":"/","sameSite":"no_restriction","secure":true,"session":true,"storeId":"0","value":"\"bXb7S07qNGPz1qgdWAFyfnXUYl8aFx679Bcxf+g43YE=\""},{"domain":".amazon.com","hostOnly":false,"httpOnly":false,"name":"skin","path":"/","sameSite":"no_restriction","secure":false,"session":true,"storeId":"0","value":"noskin"},{"domain":".amazon.com","expirationDate":2082787199.78885,"hostOnly":false,"httpOnly":false,"name":"x-wl-uid","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"1LDohDnMjwaRRzNmJsNmypWDwgQq6gi/BDq1oXLLeyIrLGmuYo6ELqwOTwv+k9+nB/tgtSUFHMfjzrCuoaDY4w3dN7VTP+ZYU3E9VPhfeKbENYWD47GYYkdHqKicRjgqIUq1G/4DI2oA="},{"domain":".amazon.com","expirationDate":2082787199.35956,"hostOnly":false,"httpOnly":false,"name":"ubid-main","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"168-0790093-5784324"},{"domain":".amazon.com","expirationDate":2082787199.672147,"hostOnly":false,"httpOnly":false,"name":"session-id","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"154-9580678-9787649"},{"domain":".amazon.com","expirationDate":2082787199.672208,"hostOnly":false,"httpOnly":false,"name":"session-id-time","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"2082787201l"},{"domain":".amazon.com","expirationDate":2082787199.672231,"hostOnly":false,"httpOnly":false,"name":"session-token","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"\"jGfunjucNuYNNwU2lbAZ/uUTVMOUY0VHYtgen/6amAPGmK05qDMifRsnxZ4OOuMNlm6a9dAxXrukP1cgmifPsf2MuOV9XRez9S/NtLyLTFIyFtRuu9qy+Rrt1PM2zz2F1qWSCM+y5XvLr5Ll4f+shrlqQN8CwJthNJ2Cw3y+ZfXHwAZrnfM56pEknZDy0s5JNX/CYGHkA6FuFMLKvEJIk9BW/iYxFI7BHu1HoDCUnUgHfK+MgKInK3nsVoLwUOmtgTWdVW0NHkl0LJa6upLEFQ==\""},{"domain":"www.amazon.com","expirationDate":1469989888,"hostOnly":true,"httpOnly":false,"name":"csm-hit","path":"/","sameSite":"no_restriction","secure":false,"session":false,"storeId":"0","value":"0HMBSM01WB7J36G9BVFG+s-P4E0JQGCQQ34VNGM6ZJH|1469385101582"}]
        }
        g_content_comm.send_to_bg(request)
    }
    
//    else if (this.navigate_back.contains(menu_target)) { 
//        g_content_comm.send_to_bg({request: 'navigate_back'})
//    } 
//    
//    else if (this.navigate_forward.contains(menu_target)) { 
//        g_content_comm.send_to_bg({request: 'navigate_forward'})
//    } 
    
    else if (this.navigate_refresh.contains(menu_target)) { 
        g_content_comm.send_to_bg({request: 'navigate_refresh'})
    } 
    
    // --------------------------------------------------------------
    // Create Set From Match Values.
    // --------------------------------------------------------------
    
    // Create set from text values.
    else if (this.create_set_by_matching_text.contains(menu_target)) { 
        var text_values = g_page_wrap.get_text_values_at(this.page_x, this.page_y)
        var request = {
            request: 'create_set_from_match_values', 
            wrap_type: ElemWrap.prototype.wrap_type.text,
            match_values: text_values
        }
        g_content_comm.send_to_bg(request)
    } 
    
    // Create set from image values.
    else if (this.create_set_by_matching_image.contains(menu_target)) {
        var image_values = g_page_wrap.get_image_values_at(this.page_x, this.page_y)
        var request = {
            request: 'create_set_from_match_values', 
            wrap_type: ElemWrap.prototype.wrap_type.image,
            match_values: image_values
        }
        g_content_comm.send_to_bg(request)
    } 
    
    // --------------------------------------------------------------
    // Create Set from Type.
    // --------------------------------------------------------------
    
    // Find all inputs.
    else if (this.create_set_from_inputs.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.input
        }
        g_content_comm.send_to_bg(request)
    }
    
    // Find all inputs.
    else if (this.create_set_from_selects.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.select
        }
        g_content_comm.send_to_bg(request)
    }
    
    // Find all images.
    else if (this.create_set_from_images.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.image
        }
        g_content_comm.send_to_bg(request)
    } 
    
    // Find all text.
    else if (this.create_set_from_texts.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.text
        }
        g_content_comm.send_to_bg(request)
    } 
    
    // --------------------------------------------------------------
    // Delete Set.
    // --------------------------------------------------------------
    
    // Delete set.
    else if (this.delete_set.contains(menu_target)) {        
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {request: 'delete_set', set_index: set_index}
        g_content_comm.send_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Shift Set.
    // --------------------------------------------------------------
    
    // Text.
    else if (this.shift_up_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_down_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_left_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_right_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_to_bg(request)
    }
    
    // Image.
    else if (this.shift_up_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_down_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_left_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_right_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_to_bg(request)
    }
    
    // Input.
    else if (this.shift_up_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_down_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_left_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_right_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_to_bg(request)
    }
    
    // Select.
    else if (this.shift_up_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_down_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_left_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shift_right_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_to_bg(request)
    }
        
    // --------------------------------------------------------------
    // Expand to similar elements.
    // --------------------------------------------------------------
    
    // Expand above.
    else if (this.expand_above.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_left = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.up,
            match_criteria: match_criteria
        }
        g_content_comm.send_to_bg(request)
    }
    // Expand below.
    else if (this.expand_below.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_left = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.down,
            match_criteria: match_criteria
        }
        g_content_comm.send_to_bg(request)
    }
    // Expand left.
    else if (this.expand_left.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_top = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.left,
            match_criteria: match_criteria
        }
        g_content_comm.send_to_bg(request)
    }
    // Expand right.
    else if (this.expand_right.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_top = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.right,
            match_criteria: match_criteria
        }
        g_content_comm.send_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Shrink w.r.t. marked sets.
    // --------------------------------------------------------------
    
    // Mark set.
    else if (this.mark_set.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'mark_set',
            set_index: set_index
        }
        g_content_comm.send_to_bg(request)
    } 
    
    // Unmark set.
    else if (this.unmark_set.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'unmark_set',
            set_index: set_index
        }
        g_content_comm.send_to_bg(request)
    } 
    
    // Merge marked sets.
    else if (this.merge_marked_sets.contains(menu_target)) {
        var request = {
                request: 'merge_marked_sets',
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_above_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.up]
        }
        g_content_comm.send_to_bg(request)
        
        //g_overlay_sets.shrink_set_to_marked(this.page_x, this.page_y, [ElemWrap.prototype.direction.up])
    }
    
    else if (this.shrink_set_below_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.down]
        }
        g_content_comm.send_to_bg(request)
        
        //g_overlay_sets.shrink_set_to_marked(this.page_x, this.page_y, [ElemWrap.prototype.direction.down])
    }
    
    else if (this.shrink_set_above_and_below_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.up, ElemWrap.prototype.direction.down]
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_left_of_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.left]
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_right_of_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.right]
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_left_and_right_of_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.left, ElemWrap.prototype.direction.right]
        }
        g_content_comm.send_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Shrink all.
    // --------------------------------------------------------------
    
    else if (this.shrink_set_to_topmost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.up
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_to_bottommost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.down
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_to_leftmost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.left
        }
        g_content_comm.send_to_bg(request)
    }
    
    else if (this.shrink_set_to_rightmost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.right
        }
        g_content_comm.send_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Perform action on set with one element.
    // --------------------------------------------------------------
    
    else if (this.perform_click.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'send_click'
            }
            g_content_comm.send_to_bg(request)
        }
    }

    else if (this.perform_type.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'send_text',
                    text: 'booya'
            }

            function send_message_callback(text) {
                request.text = text
                g_content_comm.send_to_bg(request)
            }
            g_text_input_popup.set_label_text('Enter text to type')
            g_text_input_popup.open(send_message_callback)
        }
    }

    else if (this.perform_enter.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'send_enter',
                    key: 'x'
            }
            g_content_comm.send_to_bg(request)
        }
    }

    else if (this.perform_extract.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'get_text',
                    variable_name: 'test'
            }
            g_content_comm.send_to_bg(request)
        }
    }
    
    else if (this.perform_select_option.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'select_option',
                    option_text: 'hello'
            }
            function send_message_callback(text) {
                request.option_text = text
                g_content_comm.send_to_bg(request)
            }

            var set = g_overlay_sets.sets[set_index]
            var element = set.overlays[0].elem_wrap.element
            var option_values = []
            var option_texts = []
            for (var i=0; i<element.options.length; i++) {
                option_values.push(element.options[i].value)
                option_texts.push(element.options[i].text)
                console.log('option value,text: ' + element.options[i].value + "," + element.options[i].text)
            }
            g_select_input_popup.set_label_text('Enter the text of the option to select')
            g_select_input_popup.set_options(option_values, option_texts)
            g_select_input_popup.open(send_message_callback)
        }
    }
    
    else if (this.perform_scroll_down.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'scroll_down'
            }
            g_content_comm.send_to_bg(request)
        }
    }
    
    else if (this.perform_scroll_up.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'scroll_up'
            }
            g_content_comm.send_to_bg(request)
        }
    }
    
    else if (this.perform_scroll_right.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'scroll_right'
            }
            g_content_comm.send_to_bg(request)
        }
    }
    
    else if (this.perform_scroll_left.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        if (set_index >= 0) {
            var request = {
                    request: 'perform_action',
                    set_index: set_index,
                    overlay_index: 0,
                    action: 'scroll_left'
            }
            g_content_comm.send_to_bg(request)
        }
    }
    
    document.removeEventListener('click', this.on_click_bound, true);
}

ContextMenu.prototype.on_mouse_over = function(page_event) {
    if (!this.text_box_overlay || !this.image_box_overlay) {
        return
    }
    var text_elem_wrap = g_page_wrap.get_top_text_elem_wrap_at(page_event.pageX, page_event.pageY)
    var image_elem_wrap = g_page_wrap.get_top_image_elem_wrap_at(page_event.pageX, page_event.pageY)
    this.update_text_box_overlay(text_elem_wrap)
    this.update_image_box_overlay(image_elem_wrap)
}

ContextMenu.prototype.update_text_box_overlay = function(elem_wrap) {
    this.text_box_overlay.elem_wrap = elem_wrap
    this.text_box_overlay.update(g_distinct_colors.text_color, -1, false)
}

ContextMenu.prototype.update_image_box_overlay = function(elem_wrap) {
    this.image_box_overlay.elem_wrap = elem_wrap
    this.image_box_overlay.update(g_distinct_colors.image_color, -1, false)
}


var g_context_menu = null

//We only create on the top window, not in other iframes.
if (window == window.top) {
    g_context_menu = new ContextMenu()
}

