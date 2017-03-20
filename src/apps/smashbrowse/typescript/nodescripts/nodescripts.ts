

// Returns all elements with the specified type and target value.
// If the target value is empty, then any non empty image or text will be matched.
// The target value parameter is only used for image and text types.
function filter_by_type_and_value(elements: IElementInfo[], type: ElementType, target_value: string) {
    console.log('num elements: ' + elements.length)
    console.log('elem type: ' + type)
    console.log('target value: ' + target_value)
    let matches: IElementInfo[] = []
    switch (type) {
        case ElementType.kImage: {
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
        case ElementType.kText: {
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
        case ElementType.kInput: {
            for (let e of elements) {
                if (e.tag_name == 'input') {
                    matches.push(e)
                }
            }
        } break
        case ElementType.kSelect: {
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

function find_closest_to_anchors(elements: IElementInfo[], anchor_elements: IElementInfo[]) {
    // Determine of centers of the target elements.
    let anchor_centers: IPoint[] = []
    for (let anchor of anchor_elements) {
        let x = (anchor.box.right + anchor.box.left) / 2.0
        let y = (anchor.box.bottom + anchor.box.top) / 2.0
        anchor_centers.push({x: x, y: y})
    }

    // Loop through the input elements to find the closest one to our target elements.
    let best: IElementInfo = null
    let best_dist: number = null
    for (let e of elements) {
        let x = (e.box.right + e.box.left) / 2.0
        let y = (e.box.bottom + e.box.top) / 2.0
        let dist = 0;
        for (let c of anchor_centers) {
            dist += Math.sqrt((x-c.x)*(x-c.x) + (y-c.y)*(y-c.y))
        }

        if (best == null) {
            best = e;
            best_dist = dist;
        } else if (dist < best_dist) {
            best = e;
            best_dist = dist;
        }
    }

    if (best == null) {
        return []
    }
    return [best]
}

function find_extremes(elements: IElementInfo[], getter: (info: IElementInfo)=>number, smallest: boolean) {
    let best: IElementInfo[] = []
    let best_value: number = null
    for (let e of elements) {
        let value = getter(e)
        if (best_value == null) {
            best.push(e)
            best_value = value
        } else if (smallest) {
            // This branch is for when we're finding the smallest values.
            if (value < best_value) {
                best.length = 0
                best.push(e);
                best_value = value;
            } else if (value == best_value) {
                best.push(e)
            }
        } else {
            // This branch is for when we're finding the largest values.
            if (value > best_value) {
                best.length = 0
                best.push(e);
                best_value = value;
            } else if (value == best_value) {
                best.push(e)
            }
        }
    }
    return best
}

function find_sidemost(elements: IElementInfo[], dir: DirectionType) {
    switch (dir) {
        case DirectionType.kUp:
            return find_extremes(elements, (e: IElementInfo):number =>{return e.box.top}, true)
        case DirectionType.kDown:
            return find_extremes(elements, (e: IElementInfo):number =>{return e.box.bottom}, false)
        case DirectionType.kLeft:
            return find_extremes(elements, (e: IElementInfo):number =>{return e.box.left}, true)
        case DirectionType.kRight:
            return find_extremes(elements, (e: IElementInfo):number =>{return e.box.right}, false)
        default: {
            console.log('unknown direction encountered')
        }
    }
    return elements
}

function isolate_element(elements: IElementInfo[], element_index: number) {
    if (elements.length > element_index) {
        return [elements[element_index]]
    }
    return elements
}


// declare var input: any;
// declare var output: any;
// declare var context: any;
