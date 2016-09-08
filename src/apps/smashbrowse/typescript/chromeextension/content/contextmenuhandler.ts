
//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//1) The smash_browse prefix prevents collisions with js in the target web page.
//2) Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.

class ContextMenuHandler {
    // Our Dependencies.
    content_comm: ContentComm

    // Our Dependencies extracted from GUICollection.
    page_wrap: PageWrap
    context_menu: ContextMenu
    overlay_sets: OverlaySets
    text_input_popup: TextInputPopup
    select_input_popup: SelectInputPopup

    // Our Iframe path as a string.
    iframe: string
        
    constructor(cc: ContentComm, gc: GUICollection) {
        // Dependencies.
        this.content_comm = cc
        this.page_wrap = gc.page_wrap
        this.context_menu = gc.context_menu
        this.overlay_sets = gc.overlay_sets
        this.text_input_popup = gc.text_input_popup
        this.select_input_popup = gc.select_input_popup
        this.iframe = PageWrap.get_iframe_index_path_as_string(window)
    }

    show_app_menu(click_pos: Point, text_values: string[], image_values: string[]): void {
        let req = new RequestMessage(this.iframe, RequestType.kShowAppMenu, {pos: click_pos, text_values: text_values, image_values: image_values})
        this.content_comm.send_to_bg(req)
    }

    handle_menu_click(menu_event: MouseEvent): void {
        if (!(menu_event.target instanceof Node)) {
            return
        }

        let menu_target: Node = <Node>menu_event.target

        if (this.context_menu.navigate_to_url.contains(menu_target)) {
            function goto_url(url: string) {
                let req = new RequestMessage(this.iframe, RequestType.kNavigateTo)
                req.args = {url: url}
                this.content_comm.send_to_bg(req)
            }
            this.text_input_popup.set_label_text("Enter URL")
            this.text_input_popup.open_with_callback(goto_url.bind(this))
        }

        else if (this.context_menu.test2.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kGetAllCookies)
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.test3.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kClearAllCookies)
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.test4.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kSetAllCookies)
            req.args = {
                cookies: [{ "domain": ".media.charter.com", "hostOnly": false, "httpOnly": false, "name": "XGIR", "path": "/", "sameSite": "no_restriction", "secure": false, "session": true, "storeId": "0", "value": "oVe3sa3+oCMM6XAqesACXt|w63WeXE|oi|NISDI|f5|RIR|RDr|-YlD|" }, { "domain": ".amazon-adsystem.com", "expirationDate": 2114380799.66266, "hostOnly": false, "httpOnly": false, "name": "ad-id", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "A3pGBBAQuE7ykvE5mbjHc1w" }, { "domain": ".amazon-adsystem.com", "expirationDate": 2114380799.66272, "hostOnly": false, "httpOnly": false, "name": "ad-privacy", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "0" }, { "domain": ".contextweb.com", "expirationDate": 1500478736.912202, "hostOnly": false, "httpOnly": false, "name": "V", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "wcfutv4xnaFw" }, { "domain": ".adnxs.com", "expirationDate": 1469461136.9297, "hostOnly": false, "httpOnly": true, "name": "sess", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "1" }, { "domain": ".adnxs.com", "expirationDate": 1477150736.929742, "hostOnly": false, "httpOnly": true, "name": "uuid2", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "7256487675190112956" }, { "domain": ".bluekai.com", "expirationDate": 1484926736.938499, "hostOnly": false, "httpOnly": false, "name": "bkdc", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "snv" }, { "domain": ".twitter.com", "expirationDate": 1532446736.959029, "hostOnly": false, "httpOnly": false, "name": "guest_id", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "v1%3A146937473896380188" }, { "domain": ".pubmatic.com", "expirationDate": 1532446736.959423, "hostOnly": false, "httpOnly": false, "name": "KADUSERCOOKIE", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "B39CE15D-D359-4070-93AF-2B2FCF564904" }, { "domain": ".btrll.com", "expirationDate": 1500910736.970503, "hostOnly": false, "httpOnly": false, "name": "BR_APS", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "3V5ThEh_0lvsBIlzBKA" }, { "domain": ".liverail.com", "expirationDate": 1477150736.971112, "hostOnly": false, "httpOnly": true, "name": "lr_uid", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "3.1469374738979.2645092266224913592" }, { "domain": ".liverail.com", "expirationDate": 1471966736.971148, "hostOnly": false, "httpOnly": true, "name": "lr_uds", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "a%3A1%3A%7Bi%3A3013%3Bi%3A1469374738%3B%7D" }, { "domain": ".openx.net", "expirationDate": 1500910736.972789, "hostOnly": false, "httpOnly": false, "name": "i", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "0d7de61d-3d69-42f9-c660-dac3a1b6d653|1469374738" }, { "domain": ".adaptv.advertising.com", "expirationDate": 1532446739, "hostOnly": false, "httpOnly": false, "name": "adaptv_unique_user_cookie", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "\"4688202828396573246__TIME__2016-07-24+08%3A38%3A59\"" }, { "domain": ".advertising.com", "expirationDate": 1500997136.986348, "hostOnly": false, "httpOnly": false, "name": "APID", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "VBbd1bf340-51b4-11e6-871c-02875268b68b" }, { "domain": ".yahoo.com", "expirationDate": 1532533137.041679, "hostOnly": false, "httpOnly": false, "name": "B", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "apuidghbp9o8j&b=3&s=45" }, { "domain": ".doubleclick.net", "expirationDate": 1532446737.056996, "hostOnly": false, "httpOnly": false, "name": "id", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "22bed4868e080097||t=1469374739|et=730|cs=002213fd48d3b91666662d33a3" }, { "domain": ".doubleclick.net", "expirationDate": 1532446737.057073, "hostOnly": false, "httpOnly": true, "name": "IDE", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "AHWqTUkXQJ5daxZur-OgrzXW5VETdHfU3ybxmFQnr6mP13jpdYgz_Wi7Zg" }, { "domain": ".bluekai.com", "expirationDate": 1484926737.090603, "hostOnly": false, "httpOnly": false, "name": "bku", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "b/X99OnCkazyiMRV" }, { "domain": ".bidswitch.net", "expirationDate": 1500910737.199754, "hostOnly": false, "httpOnly": false, "name": "c", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "1469374739" }, { "domain": ".rubiconproject.com", "expirationDate": 1500910738.226701, "hostOnly": false, "httpOnly": false, "name": "cd", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "false" }, { "domain": ".rubiconproject.com", "expirationDate": 1500910738.226742, "hostOnly": false, "httpOnly": false, "name": "au", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "IR0RS4Y8-HNVT-10.183.95.119" }, { "domain": ".tap.rubiconproject.com", "expirationDate": 1500910738.226769, "hostOnly": false, "httpOnly": false, "name": "dq", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "1|1|0|0" }, { "domain": ".bidswitch.net", "expirationDate": 1500910737.233113, "hostOnly": false, "httpOnly": false, "name": "tuuid", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "0e7e525b-b584-4e82-a003-af2d7105785f" }, { "domain": ".bidswitch.net", "expirationDate": 1500910737.233157, "hostOnly": false, "httpOnly": false, "name": "tuuid_last_update", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "1469374739" }, { "domain": ".adtechus.com", "hostOnly": false, "httpOnly": false, "name": "CfP", "path": "/", "sameSite": "no_restriction", "secure": false, "session": true, "storeId": "0", "value": "1" }, { "domain": ".spotxchange.com", "expirationDate": 1471966739, "hostOnly": false, "httpOnly": false, "name": "partner-1469374739_4cef-0", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "eNplzNEOgiAUANDn%2FuYGVtM3V%2BZggksdcHnTVgvEra02ha%2FvAzofcE45IbuCF4%2FIVzQcpkVFQ1Wyhj9HfXOt4zOS7iXTTG0Pe6EZNEO12QG%2F6GUQDjZMZbRagPXlKi7CNef%2Fy9ZXwJ4dmWMftqjsXoeIpntPJHOtr0Cm6iA9UpFC%2FgMP7zDA" }, { "domain": ".spotxchange.com", "expirationDate": 1471966739, "hostOnly": false, "httpOnly": false, "name": "user-0", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "dXNlcl9ndWlkCWRjZGE3OTdlNTFiNDExZTZiM2UyMWMwMmVjNGYwMDAzCWF1ZGllbmNlX2lkCWRjZGE3OTdlLTUxYjQtMTFlNi1iM2UyLTFjMDJlYzRmMDAwMwljcmVhdGVkX2RhdGUJMTQ2OTM3NDczOQltb2RpZmllZF9kYXRlCTE0NjkzNzQ3Mzk=" }, { "domain": ".adtechus.com", "expirationDate": 1532446738, "hostOnly": false, "httpOnly": false, "name": "JEB2", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "5794D8166E65074490B52083FCCBF6B0" }, { "domain": ".1rx.io", "expirationDate": 1532446737.677317, "hostOnly": false, "httpOnly": true, "name": "_rxuuid", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "%7B%22rx_uuid%22%3A%22RX-a4930b9e-9bab-4b5e-a7d9-da05a0c9b788%22%7D" }, { "domain": ".rhythmxchange.com", "expirationDate": 1532446738.212251, "hostOnly": false, "httpOnly": true, "name": "_rxuuid", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "%7B%22rx_uuid%22%3A%22RX-a4930b9e-9bab-4b5e-a7d9-da05a0c9b788%22%7D" }, { "domain": ".www.amazon.com", "expirationDate": 1469979534, "hostOnly": false, "httpOnly": false, "name": "csm-hit", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "R7SBQM9DTWXX6RF3K8NR+s-R7SBQM9DTWXX6RF3K8NR|1469374745127" }, { "domain": ".charter.com", "expirationDate": 1469644134.636339, "hostOnly": false, "httpOnly": false, "name": "TE_RPN", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "855.818.5773" }, { "domain": ".charter.com", "expirationDate": 1477160934.636373, "hostOnly": false, "httpOnly": false, "name": "CSList", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "32331289/32333390,32331289/32333390,0/0,0/0,0/0" }, { "domain": ".charter.com", "expirationDate": 1532500134.72454, "hostOnly": false, "httpOnly": false, "name": "PrefID", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "123-2376042924" }, { "domain": ".amazon.com", "expirationDate": 1469385928.633002, "hostOnly": false, "httpOnly": false, "name": "a-ogbcbff", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "1" }, { "domain": ".amazon.com", "expirationDate": 2100105088.633124, "hostOnly": false, "httpOnly": false, "name": "x-main", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "YmFgvX5CRhxhQDVs0mR6qozVcfgr9GU5" }, { "domain": ".amazon.com", "expirationDate": 2100105088.633154, "hostOnly": false, "httpOnly": true, "name": "at-main", "path": "/", "sameSite": "no_restriction", "secure": true, "session": false, "storeId": "0", "value": "Atza|IwEBIO-i74Sm2j7YCOWVSDboH8vQymSJ_2RT_l_bXKe9Sr70yUyGeyrr6t9ofu3j6XyhYfKWUpUNv-Mit4as8sjg7QnR9fcwdy2EHKSp3au_LGlc0ymU2izTS51SOrdiIBhHa8M3KdDW6ChojocnGBn0coXbDcZl2PeizFpnPxEOq1RzqHEliIouk-JpXhk0eWUgroH8Z0Og0DxshJjPM5WAhmLhkloHYHZobCHVpVE_rcfcees9n2R8gbBX-XqQBvLyRLdJD96wfmTQ_lgmOolZqAe1vQ5ZlQXanw4_DErMEymKW4T0EfIi3iSWiZ_nmz7v6Rutzjq4yR87nPdrVRHar5-F" }, { "domain": ".amazon.com", "hostOnly": false, "httpOnly": true, "name": "sess-at-main", "path": "/", "sameSite": "no_restriction", "secure": true, "session": true, "storeId": "0", "value": "\"bXb7S07qNGPz1qgdWAFyfnXUYl8aFx679Bcxf+g43YE=\"" }, { "domain": ".amazon.com", "hostOnly": false, "httpOnly": false, "name": "skin", "path": "/", "sameSite": "no_restriction", "secure": false, "session": true, "storeId": "0", "value": "noskin" }, { "domain": ".amazon.com", "expirationDate": 2082787199.78885, "hostOnly": false, "httpOnly": false, "name": "x-wl-uid", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "1LDohDnMjwaRRzNmJsNmypWDwgQq6gi/BDq1oXLLeyIrLGmuYo6ELqwOTwv+k9+nB/tgtSUFHMfjzrCuoaDY4w3dN7VTP+ZYU3E9VPhfeKbENYWD47GYYkdHqKicRjgqIUq1G/4DI2oA=" }, { "domain": ".amazon.com", "expirationDate": 2082787199.35956, "hostOnly": false, "httpOnly": false, "name": "ubid-main", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "168-0790093-5784324" }, { "domain": ".amazon.com", "expirationDate": 2082787199.672147, "hostOnly": false, "httpOnly": false, "name": "session-id", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "154-9580678-9787649" }, { "domain": ".amazon.com", "expirationDate": 2082787199.672208, "hostOnly": false, "httpOnly": false, "name": "session-id-time", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "2082787201l" }, { "domain": ".amazon.com", "expirationDate": 2082787199.672231, "hostOnly": false, "httpOnly": false, "name": "session-token", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "\"jGfunjucNuYNNwU2lbAZ/uUTVMOUY0VHYtgen/6amAPGmK05qDMifRsnxZ4OOuMNlm6a9dAxXrukP1cgmifPsf2MuOV9XRez9S/NtLyLTFIyFtRuu9qy+Rrt1PM2zz2F1qWSCM+y5XvLr5Ll4f+shrlqQN8CwJthNJ2Cw3y+ZfXHwAZrnfM56pEknZDy0s5JNX/CYGHkA6FuFMLKvEJIk9BW/iYxFI7BHu1HoDCUnUgHfK+MgKInK3nsVoLwUOmtgTWdVW0NHkl0LJa6upLEFQ==\"" }, { "domain": "www.amazon.com", "expirationDate": 1469989888, "hostOnly": true, "httpOnly": false, "name": "csm-hit", "path": "/", "sameSite": "no_restriction", "secure": false, "session": false, "storeId": "0", "value": "0HMBSM01WB7J36G9BVFG+s-P4E0JQGCQQ34VNGM6ZJH|1469385101582" }]
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.test5.contains(menu_target)) {
            this.overlay_sets.update()
            let req = new RequestMessage(this.iframe, RequestType.kGetZoom)
            this.content_comm.send_to_bg(req)
        }

        //    else if (this.navigate_back.contains(menu_target)) { 
        //        let req = new RequestMessage(RequestType.kNavigateBack)
        //        this.content_comm.send_to_bg(req)
        //    } 
        //    
        //    else if (this.navigate_forward.contains(menu_target)) { 
        //        let req = new RequestMessage(RequestType.kNavigateForward)
        //        this.content_comm.send_g(req)
        //    } 
    
        else if (this.context_menu.navigate_refresh.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kNavigateRefresh)
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Create Set From Match Values.
        // ----------------------------------------------------------
    
        // Create set from text values.
        else if (this.context_menu.create_set_by_matching_text.contains(menu_target)) {
            let text_values = this.page_wrap.get_text_values_at(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kCreateSetFromMatchValues)
            req.args = {
                wrap_type: WrapType.text,
                match_values: text_values
            }
            this.content_comm.send_to_bg(req)
        }

        // Create set from image values.
        else if (this.context_menu.create_set_by_matching_image.contains(menu_target)) {
            let image_values = this.page_wrap.get_image_values_at(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kCreateSetFromMatchValues)
            req.args = {
                wrap_type: WrapType.image,
                match_values: image_values
            }
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Create Set from Type.
        // ----------------------------------------------------------
    
        // Find all inputs.
        else if (this.context_menu.create_set_from_inputs.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kCreateSetFromWrapType)
            req.args = {
                wrap_type: WrapType.input
            }
            this.content_comm.send_to_bg(req)
        }

        // Find all inputs.
        else if (this.context_menu.create_set_from_selects.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kCreateSetFromWrapType)
            req.args = {
                wrap_type: WrapType.select
            }
            this.content_comm.send_to_bg(req)
        }

        // Find all images.
        else if (this.context_menu.create_set_from_images.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kCreateSetFromWrapType)
            req.args = {
                wrap_type: WrapType.image
            }
            this.content_comm.send_to_bg(req)
        }

        // Find all text.
        else if (this.context_menu.create_set_from_texts.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kCreateSetFromWrapType)
            req.args = {
                wrap_type: WrapType.text
            }
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Delete Set.
        // ----------------------------------------------------------
    
        // Delete set.
        else if (this.context_menu.delete_set.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kDeleteSet)
            req.args = { set_index: set_index }
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Shift Set.
        // ----------------------------------------------------------
    
        // Text.
        else if (this.context_menu.shift_up_by_text.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.up,
                wrap_type: WrapType.text
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_down_by_text.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.down,
                wrap_type: WrapType.text
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_left_by_text.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.left,
                wrap_type: WrapType.text
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_right_by_text.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.right,
                wrap_type: WrapType.text
            };
            this.content_comm.send_to_bg(req)
        }

        // Image.
        else if (this.context_menu.shift_up_by_image.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.up,
                wrap_type: WrapType.image
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_down_by_image.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.down,
                wrap_type: WrapType.image
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_left_by_image.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.left,
                wrap_type: WrapType.image
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_right_by_image.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.right,
                wrap_type: WrapType.image
            };
            this.content_comm.send_to_bg(req)
        }

        // Input.
        else if (this.context_menu.shift_up_by_input.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.up,
                wrap_type: WrapType.input
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_down_by_input.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.down,
                wrap_type: WrapType.input
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_left_by_input.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.left,
                wrap_type: WrapType.input
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_right_by_input.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.right,
                wrap_type: WrapType.input
            };
            this.content_comm.send_to_bg(req)
        }

        // Select.
        else if (this.context_menu.shift_up_by_select.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.up,
                wrap_type: WrapType.select
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_down_by_select.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.down,
                wrap_type: WrapType.select
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_left_by_select.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.left,
                wrap_type: WrapType.select
            };
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shift_right_by_select.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShiftSet)
            req.args = {
                set_index: set_index,
                direction: Direction.right,
                wrap_type: WrapType.select
            };
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Expand to similar elements.
        // ----------------------------------------------------------
    
        // Expand above.
        else if (this.context_menu.expand_above.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let match_criteria = new MatchCriteria()
            match_criteria.match_left = true
            match_criteria.match_font = true
            match_criteria.match_font_size = true
            let req = new RequestMessage(this.iframe, RequestType.kExpandSet)
            req.args = {
                set_index: set_index,
                direction: Direction.up,
                match_criteria: match_criteria
            }
            this.content_comm.send_to_bg(req)
        }
        // Expand below.
        else if (this.context_menu.expand_below.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let match_criteria = new MatchCriteria()
            match_criteria.match_left = true
            match_criteria.match_font = true
            match_criteria.match_font_size = true
            let req = new RequestMessage(this.iframe, RequestType.kExpandSet)
            req.args = {
                set_index: set_index,
                direction: Direction.down,
                match_criteria: match_criteria
            }
            this.content_comm.send_to_bg(req)
        }
        // Expand left.
        else if (this.context_menu.expand_left.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let match_criteria = new MatchCriteria()
            match_criteria.match_top = true
            match_criteria.match_font = true
            match_criteria.match_font_size = true
            let req = new RequestMessage(this.iframe, RequestType.kExpandSet)
            req.args = {
                set_index: set_index,
                direction: Direction.left,
                match_criteria: match_criteria
            }
            this.content_comm.send_to_bg(req)
        }
        // Expand right.
        else if (this.context_menu.expand_right.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let match_criteria = new MatchCriteria()
            match_criteria.match_top = true
            match_criteria.match_font = true
            match_criteria.match_font_size = true
            let req = new RequestMessage(this.iframe, RequestType.kExpandSet)
            req.args = {
                set_index: set_index,
                direction: Direction.right,
                match_criteria: match_criteria
            }
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Shrink w.r.t. marked sets.
        // ----------------------------------------------------------
    
        // Mark set.
        else if (this.context_menu.mark_set.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kMarkSet)
            req.args = {
                set_index: set_index
            }
            this.content_comm.send_to_bg(req)
        }

        // Unmark set.
        else if (this.context_menu.unmark_set.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kUnmarkSet)
            req.args = {
                set_index: set_index
            }
            this.content_comm.send_to_bg(req)
        }

        // Merge marked sets.
        else if (this.context_menu.merge_marked_sets.contains(menu_target)) {
            let req = new RequestMessage(this.iframe, RequestType.kMergeMarkedSets)
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_above_marked.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos(), false)
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSetToMarked)
            req.args = {
                set_index: set_index,
                directions: [Direction.up]
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_below_marked.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos(), false)
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSetToMarked)
            req.args = {
                set_index: set_index,
                directions: [Direction.down]
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_above_and_below_marked.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos(), false)
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSetToMarked)
            req.args = {
                set_index: set_index,
                directions: [Direction.up, Direction.down]
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_left_of_marked.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos(), false)
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSetToMarked)
            req.args = {
                set_index: set_index,
                directions: [Direction.left]
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_right_of_marked.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos(), false)
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSetToMarked)
            req.args = {
                set_index: set_index,
                directions: [Direction.right]
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_left_and_right_of_marked.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos(), false)
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSetToMarked)
            req.args = {
                set_index: set_index,
                directions: [Direction.left, Direction.right]
            }
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Shrink all.
        // ----------------------------------------------------------
    
        else if (this.context_menu.shrink_set_to_topmost.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSet)
            req.args = {
                set_index: set_index,
                direction: Direction.up
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_to_bottommost.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSet)
            req.args = {
                set_index: set_index,
                direction: Direction.down
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_to_leftmost.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSet)
            req.args = {
                set_index: set_index,
                direction: Direction.left
            }
            this.content_comm.send_to_bg(req)
        }

        else if (this.context_menu.shrink_set_to_rightmost.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            let req = new RequestMessage(this.iframe, RequestType.kShrinkSet)
            req.args = {
                set_index: set_index,
                direction: Direction.right
            }
            this.content_comm.send_to_bg(req)
        }

        // --------------------------------------------------------------
        // Perform action on set with one element.
        // ----------------------------------------------------------
    
        else if (this.context_menu.perform_click.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kSendClick
                }
                this.content_comm.send_to_bg(req)
            }
        }

        else if (this.context_menu.perform_type.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kSendText,
                    text: 'booya'
                }

                function on_text_input_callback(text: string) {
                    req.args.text = text
                    this.content_comm.send_to_bg(req)
                }
                this.text_input_popup.set_label_text('Enter text to type')
                this.text_input_popup.open_with_callback(on_text_input_callback.bind(this))
            }
        }

        else if (this.context_menu.perform_enter.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kSendEnter,
                    key: 'x'
                }
                this.content_comm.send_to_bg(req)
            }
        }

        else if (this.context_menu.perform_extract.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kGetText,
                    letiable_name: 'test'
                }
                this.content_comm.send_to_bg(req)
            }
        }

        else if (this.context_menu.perform_select_option.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kSelectOption,
                    option_text: 'hello'
                }
                function on_select_callback(text: string) {
                    req.args.option_text = text
                    this.content_comm.send_to_bg(req)
                }

                let os = this.overlay_sets.sets[set_index]
                let element: HTMLSelectElement = <HTMLSelectElement>os.overlays[0].elem_wrap.element
                let option_values: string[] = []
                let option_texts: string[] = []
                for (let i = 0; i < element.options.length; i++) {
                    let option = <HTMLOptionElement>(element.options[i])
                    option_values.push(option.value)
                    option_texts.push(option.text)
                    console.log('option value,text: ' + option.value + "," + option.text)
                }
                this.select_input_popup.set_label_text('Enter the text of the option to select')
                this.select_input_popup.set_options(option_values, option_texts)
                this.select_input_popup.open_with_callback(on_select_callback.bind(this))
            }
        }

        else if (this.context_menu.perform_scroll_down.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kScrollDown
                }
                this.content_comm.send_to_bg(req)
            }
        }

        else if (this.context_menu.perform_scroll_up.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kScrollUp
                }
                this.content_comm.send_to_bg(req)
            }
        }

        else if (this.context_menu.perform_scroll_right.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kScrollRight
                }
                this.content_comm.send_to_bg(req)
            }
        }

        else if (this.context_menu.perform_scroll_left.contains(menu_target)) {
            let set_index = this.overlay_sets.find_set_index(this.context_menu.get_page_pos())
            if (set_index >= 0) {
                let req = new RequestMessage(this.iframe, RequestType.kPerformAction)
                req.args = {
                    set_index: set_index,
                    overlay_index: 0,
                    action: ActionType.kScrollLeft
                }
                this.content_comm.send_to_bg(req)
            }
        }
    }
}





