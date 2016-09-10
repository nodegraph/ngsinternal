
//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//1) The smash_browse prefix prevents collisions with js in the target web page.
//2) Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.

class PageOverlaysHandler {
    // Our Dependencies.
    content_comm: ContentComm
    overlay_sets: OverlaySets

    // Our Iframe path as a string.
    iframe: string

    constructor(cc: ContentComm, gc: GUICollection) {
        // Dependencies.
        this.content_comm = cc
        this.overlay_sets = gc.overlay_sets
        this.iframe = PageWrap.get_iframe_index_path_as_string(window)
    }

    show_app_menu(click_pos: Point, text_values: string[], image_values: string[]): void {
        // Determine the set index at the click point.
        let set_index = this.overlay_sets.find_set_index(click_pos)
        let overlay_index = -1
        if (set_index >= 0) {
            let oset = this.overlay_sets.sets[set_index]
            overlay_index = oset.find_overlay_index(click_pos)
        }

        // If we're a select element, grab the option values and texts.'
        let option_values: string[] = []
        let option_texts: string[] = []
        if (set_index >= 0) {
            let oset = this.overlay_sets.sets[set_index]
            let element = oset.overlays[0].elem_wrap.element
            if (element instanceof HTMLSelectElement) {
                let select: HTMLSelectElement = <HTMLSelectElement>element
                for (let i = 0; i < element.options.length; i++) {
                    let option = <HTMLOptionElement>(element.options[i])
                    option_values.push(option.value)
                    option_texts.push(option.text)
                    console.log('option value,text: ' + option.value + "," + option.text)
                }
            }
        }

        let req = new RequestMessage(this.iframe, RequestType.kShowAppMenu,
            {
                pos: click_pos,
                text_values: text_values,
                image_values: image_values,
                set_index: set_index,
                overlay_index: overlay_index,
                option_values: option_values,
                option_texts: option_texts
            })

        this.content_comm.send_to_bg(req)
    }

}





