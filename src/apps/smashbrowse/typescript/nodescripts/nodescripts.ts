

// Returns all elements with the specified type and target value.
// If the target value is empty, then any non empty image or text will be matched.
// The target value parameter is only used for image and text types.
function isolate_by_type_and_value(elements: IElementInfo[], type: ElementType, target_value: string) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

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
function isolate_by_position(elements: IElementInfo[], global_mouse_position: { x: number, y: number }) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

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

function isolate_by_dimensions(elements: IElementInfo[], width: number, height: number, max_width_diff: number, max_height_diff: number) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

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

// Filters elements which are in the current viewport.
// Note that it simply looks at the in_viewport property,
// so this applies to the time at which the elements were gathered.
function isolate_by_viewport(elements: IElementInfo[]) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

    let matches: IElementInfo[] = []
    for (let e of elements) {
        if (e.in_viewport) {
            matches.push(e)
        }
    }
    return matches
}

function join_elements(elements: IElementInfo[], other: IElementInfo[]) {
    if ((elements == undefined) && (other == undefined)) {
        return []
    } else if (elements == undefined) {
        return other
    } else if (other == undefined) {
        return elements
    }
    return elements.concat(other)
}

function sort_by_distance_to_anchors(elements: IElementInfo[], anchor_elements: IElementInfo[]) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }
    // The anchor_elements may be be undefined if the anchor_elements input to the node is missing the elements property.
    if (anchor_elements == undefined) {
        return []
    }


    // Determine of centers of the target elements.
    let anchor_centers: IPoint[] = []
    for (let anchor of anchor_elements) {
        let x = (anchor.box.right + anchor.box.left) / 2.0
        let y = (anchor.box.bottom + anchor.box.top) / 2.0
        anchor_centers.push({x: x, y: y})
    }

    
    interface IDist {
        dist: number
        elem: IElementInfo
    }

    // Calculates the distances.
    let distances: IDist[] = []
    for (let e of elements) {
        let x = (e.box.right + e.box.left) / 2.0
        let y = (e.box.bottom + e.box.top) / 2.0
        let dist = 0;
        for (let c of anchor_centers) {
            dist += Math.sqrt((x-c.x)*(x-c.x) + (y-c.y)*(y-c.y))
        }
        distances.push({dist: dist, elem: e})
    }

    // Sort the distances.
    distances.sort((a: IDist, b: IDist):number => {
        return a.dist - b.dist
    })

    // Extract the element infos.
    let sorted: IElementInfo[] = []
    for (let d of distances) {
        sorted.push(d.elem)
    }
    return sorted
}

function find_extremes(elements: IElementInfo[], getter: (info: IElementInfo)=>number, smallest: boolean) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

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

function isolate_by_boundary(elements: IElementInfo[], dir: DirectionType) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

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
    return []
}

function isolate_greater_than(elements: IElementInfo[], getter: (info: IElementInfo)=>number, limit: number) {
    let matches: IElementInfo[] = []
    for (let e of elements) {
        if (getter(e) > limit) {
            matches.push(e)
        }
    }
    return matches
}

function isolate_less_than(elements: IElementInfo[], getter: (info: IElementInfo)=>number, limit: number) {
    let matches: IElementInfo[] = []
    for (let e of elements) {
        if (getter(e) < limit) {
            matches.push(e)
        }
    }
    return matches
}

function isolate_to_one_side(elements: IElementInfo[], anchors: IElementInfo[], dir: DirectionType) {
    if (elements == undefined) {
        return []
    }
    if (anchors == undefined) {
        return elements
    }
    
    // Find the anchor bounds.
    let left: number = anchors[0].box.left
    let right: number = anchors[0].box.right
    let bottom: number = anchors[0].box.bottom
    let top: number = anchors[0].box.top
    for (let i = 1; i < anchors.length; i++) {
        if (anchors[i].box.left < left) {
            left = anchors[i].box.left
        }
        if (anchors[i].box.right > right) {
            right = anchors[i].box.right
        }
        if (anchors[i].box.top < top) {
            top = anchors[i].box.top
        }
        if (anchors[i].box.bottom > bottom) {
            bottom = anchors[i].box.bottom
        }
    }

    switch (dir) {
        case DirectionType.kUp:
            return isolate_less_than(elements, (e: IElementInfo):number =>{return e.box.bottom}, top)
        case DirectionType.kDown:
            return isolate_greater_than(elements, (e: IElementInfo):number =>{return e.box.top}, bottom)
        case DirectionType.kLeft:
            return isolate_less_than(elements, (e: IElementInfo):number =>{return e.box.right}, left)
        case DirectionType.kRight:
            return isolate_greater_than(elements, (e: IElementInfo):number =>{return e.box.left}, right)
        default: {
            console.log('unknown direction encountered')
        }
    }
    return []
}

function isolate_by_index(elements: IElementInfo[], start_index: number, num_indices: number) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

    let matches: IElementInfo[] = []
    let end_index = start_index + num_indices
    for (let i = start_index; i<end_index && i<elements.length; i++) {
        matches.push(elements[i])
    }
    return matches
}

function prune_duplicates(elements: IElementInfo[]) {
    // The elements may be be undefined if the main input to the node is missing the elements property.
    if (elements == undefined) {
        return []
    }

    let seen: Set<string> = new Set<string>()
    let matches: IElementInfo[] = []
    for (let e of elements) {
        let key = e.fe_index_path + "--" + e.xpath
        if (seen.has(key)) {
            continue
        }
        seen.add(key)
        matches.push(e)
    }
    return matches
}

function prune_elements(elements: IElementInfo[], prune: IElementInfo[]) {
    // Create a set of the keys to remove.
    let keys_to_prune: Set<string> = new Set<string>()
    for (let e of prune) {
        let key = e.fe_index_path + "--" + e.xpath
        keys_to_prune.add(key)
    }

    // Loop through the elements, removing any that match the keys to discard.
    let matches: IElementInfo[] = []
    for (let e of elements) {
        let key = e.fe_index_path + "--" + e.xpath
        if (keys_to_prune.has(key)) {
            continue
        }
        matches.push(e)
    }
    return matches
}


// declare var input: any;
// declare var output: any;
// declare var context: any;
