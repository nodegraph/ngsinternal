

// Returns all elements with the specified type and target value.
// If the target value is empty, then any non empty image or text will be matched.
// The target value parameter is only used for image and text types.
function filter_by_type_and_value(elements: IElementInfo[], type: WrapType, target_value: string) {
    let matches: IElementInfo[] = []
    switch (type) {
        case WrapType.image: {
            for (let e of elements) {
                if (!target_value) {
                    if (e.image) {
                        matches.push(e)
                    }
                } else {
                    if (target_value == e.image) {
                        matches.push(e)
                    }
                }
            }
        } break
        case WrapType.text: {
            for (let e of elements) {
                if (!target_value) {
                    if (e.text) {
                        console.log('111 value:*' + target_value + '*  text:*' + e.text + '*')
                        matches.push(e)
                    }
                } else {
                    if (target_value == e.text) {
                        console.log('222 value:*' + target_value + '*  text:*' + e.text + '*')
                        matches.push(e)
                    }
                }
            }
        } break
        case WrapType.input: {
            for (let e of elements) {
                if (e.tag_name == 'input') {
                    matches.push(e)
                }
            }
        } break
        case WrapType.select: {
            for (let e of elements) {
                if (e.tag_name == 'select') {
                    matches.push(e)
                }
            }
        } break
    }
    return matches
}

// Returns all elements whose bounds contains the mouse position.
function filter_by_position(elements: IElementInfo[], global_mouse_position: { x: number, y: number }) {
    console.log('filtering by position: ' + global_mouse_position.x + ',' + global_mouse_position.y)
    let matches: IElementInfo[] = []
    for (let e of elements) {
        if (e.box.left > global_mouse_position.x) {
            continue
        }
        if (e.box.right < global_mouse_position.x) {
            continue
        }
        if (e.box.top > global_mouse_position.y) {
            continue
        }
        if (e.box.bottom < global_mouse_position.y) {
            continue
        }
        matches.push(e)
    }
    console.log('found number matches: ' + matches.length)
    return matches
}

function filter_by_dimensions(elements: IElementInfo[], width: number, height: number, max_width_diff: number, max_height_diff: number) {

    let min_width_pixels = width * (1.0 - (max_width_diff * 0.01))
    let max_width_pixels = width * (1.0 + (max_width_diff * 0.01))
    let min_height_pixels = height * (1.0 - (max_height_diff * 0.01));
    let max_height_pixels = height * (1.0 + (max_height_diff * 0.01));

    let matches: IElementInfo[] = []
    for (let e of elements) {
        let candidate_height = e.box.bottom - e.box.top
        let candidate_width = e.box.right - e.box.left
        if ((candidate_width < min_width_pixels) || (candidate_width > max_width_pixels)) {
            continue
        }
        if ((candidate_height < min_height_pixels) || (candidate_height > max_height_pixels)) {
            continue
        }
        matches.push(e)
    }
    return matches
}


// declare var input: any;
// declare var output: any;
// declare var context: any;
