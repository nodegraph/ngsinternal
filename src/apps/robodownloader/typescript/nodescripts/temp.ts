

// ------------------------------------------------------------------------------------------------------------------
    // Static element methods.
    // ------------------------------------------------------------------------------------------------------------------

    static find_top_leftmost(elems: IElementInfo[]): IElementInfo {
        let best: IElementInfo = null
        elems.forEach((elem) => {
            if (best === null) {
                best = elem
            } else {
                if (elem.box.top < best.box.top) {
                    best = elem
                } else if ((elem.box.top == best.box.top) && (elem.box.left < best.box.left)) {
                    best = elem
                }
            }
        })
        return best
    }

    // This method is supposed to find the topmost frame, in a set of overlapping frames.
    // This will take some time to implement so for now it simply looks at the frame index path_length
    // and choose the shortest path, or if there are multiple paths with the same length then it
    // chooses the first one it encounters.
    // The other issue that needs to be investigated is that we seem to be getting frames that aren't
    // really visible and often they would be clipped by their parent frames but our logic currently
    // doesn't take this into account.
    static find_top_frame(elems: IClickInfo[]): IClickInfo {
        let best: IClickInfo = null
        let best_path_length: number = null
        elems.forEach((elem) => {
            let path_arr = get_array_from_index_path(elem.fw_index_path)
            let path_length = path_arr.length
            if (best === null) {
                best = elem
                best_path_length = path_length
            } else {
                if (path_length < best_path_length) {
                    best = elem
                    best_path_length = path_length
                }
            }
        })
        return best
    }

    static find_next_along_rows(src: IElementInfo, candidates: IElementInfo[], max_width_diff: number, max_height_diff: number, max_angle_diff: number) {
        let next: IElementInfo = BgCommHandler.find_closest_neighbor(src, candidates, 0, max_width_diff, max_height_diff, max_angle_diff);
        if (next) {
            return next
        }

        // Otherwise we need to find the first element of rows below the current row.
        let best: IElementInfo = null

        let src_box = new Box(src.box)
        const src_bottom = src_box.bottom

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((dest) => {
            let dest_box = new Box(dest.box)
            
            // Determine if the candidate is of the right size.
            let dest_width = dest_box.get_width()
            let dest_height = dest_box.get_height()
            let min_width_pixels = src_box.get_width() * (1.0 - (max_width_diff * 0.01))
            let max_width_pixels = src_box.get_width() * (1.0 + (max_width_diff * 0.01))
            let min_height_pixels = src_box.get_height() * (1.0 - (max_height_diff * 0.01));
            let max_height_pixels = src_box.get_height() * (1.0 + (max_height_diff * 0.01));
            if ((dest_width < min_width_pixels) || (dest_width > max_width_pixels)) {
                return
            }
            if ((dest_height < min_height_pixels) || (dest_height > max_height_pixels)) {
                return
            }

            // Make sure the dest element is below our current row.
            if (dest.box.top < src_bottom) {
                return
            }

            // If best hasn't been set yet, we take the first dest element.
            if (!best) {
                best = dest
                return
            }

            // If the dest element is more top-left than our current best, it becomes the next best element.
            if (dest.box.top <= best.box.top) {
                if (dest.box.left <= best.box.left) {
                    best = dest
                    return
                }
            }
        })
        return best
    }

    static find_next_along_column(src: IElementInfo, candidates: IElementInfo[], max_width_diff: number, max_height_diff: number) {
        let best: IElementInfo = null

        let src_box = new Box(src.box)
        const src_bottom = src_box.bottom

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((candidate) => {
            let candidate_box = new Box(candidate.box)
            
            // The dest box must be align with the src box.
            if (candidate_box.left != src_box.left) {
                return
            }

            // Determine if the candidate is of the right size.
            let dest_width = candidate_box.get_width()
            let dest_height = candidate_box.get_height()
            let min_width_pixels = src_box.get_width() * (1.0 - (max_width_diff * 0.01))
            let max_width_pixels = src_box.get_width() * (1.0 + (max_width_diff * 0.01))
            let min_height_pixels = src_box.get_height() * (1.0 - (max_height_diff * 0.01));
            let max_height_pixels = src_box.get_height() * (1.0 + (max_height_diff * 0.01));
            if ((dest_width < min_width_pixels) || (dest_width > max_width_pixels)) {
                return
            }
            if ((dest_height < min_height_pixels) || (dest_height > max_height_pixels)) {
                return
            }

            // Make sure the dest element is below our current row.
            if (candidate.box.top < src_bottom) {
                return
            }

            // If best hasn't been set yet, we take the first dest element.
            if (!best) {
                best = candidate
                return
            }

            // If the dest element is above our current best, it becomes the next best element.
            if (candidate.box.top <= best.box.top) {
                best = candidate
                return
            }
        })
        return best
    }

    static find_first_column_elements(candidates: IElementInfo[], min_elements_per_column: number = 5, max_diff_between_elements: number = 500) {

        let columns: Map<number, IElementInfo[]> = new Map<number,IElementInfo[]>();

        // Group the elements into columns.
        candidates.forEach((candidate) => {
            let candidate_box = new Box(candidate.box)
            if (!columns.has(candidate_box.left)) {
                columns.set(candidate_box.left, [])
            }
            let elements = columns.get(candidate_box.left)
            elements.push(candidate)
        })
        
        // Remove columns with less than min_elements_per_column
        let keys_to_remove: number[] = []
        columns.forEach((elements: IElementInfo[], key: number) => {
            if (elements.length < min_elements_per_column) {
                keys_to_remove.push(key)
            }
        })
        for (let key of keys_to_remove) {
            columns.delete(key)
        }
        
        // Sort the elements in the columns vertically.
        columns.forEach((elements: IElementInfo[], key: number) => {
            elements.sort((e1, e2) => {return e1.box.top - e2.box.top});
        })

        // Collect the starting elements of each column.
        let first_elements: IElementInfo[]
        columns.forEach((elements: IElementInfo[], key: number) => {
            first_elements.push(elements[0])
        })

        // A given column may be split into multiple groups.
        // Find the starting points of internal splits and append them.
        columns.forEach((elements: IElementInfo[], key: number) => {
            if (elements.length < 2) {
                return
            }
            for (let i = 1; i < elements.length; i++) {
                if ((elements[i].box.top - elements[i-1].box.bottom) > max_diff_between_elements) {
                    first_elements.push(elements[i])
                }
            }
        })

        return first_elements
    }

    static find_closest_neighbor(src: IElementInfo, candidates: IElementInfo[], angle: number, max_width_diff: number, max_height_diff: number, max_angle_diff: number) {

        // Loop through each one trying to find the best one.
        let best: IElementInfo = null
        let best_distance = 0

        let src_box = new Box(src.box)
        let src_center = src_box.get_center()

        let theta = angle / 180.0 * Math.PI
        let dir = new Point({x: Math.cos(theta), y: -1 * Math.sin(theta)}) // -1 is because y increases from top to bottom.
        let perp_dir = new Point({x: -dir.y, y: dir.x})

        // Note the return in the forEach loop acts like a continue statement.
        candidates.forEach((dest) => {
            // Skip the dest element if its equal to the src element.
            if ((dest.fw_index_path == src.fw_index_path) && (dest.xpath == src.xpath)) {
                return
            }

            // Determine the candidates center point information.
            let dest_box = new Box(dest.box)
            let dest_center:Point = dest_box.get_center()
            let diff = dest_center.subtract(src_center)
            
            // Determine if the candidate is of the right size.
            let dest_width = dest_box.get_width()
            let dest_height = dest_box.get_height()
            let min_width_pixels = src_box.get_width() * (1.0 - (max_width_diff * 0.01))
            let max_width_pixels = src_box.get_width() * (1.0 + (max_width_diff * 0.01))
            let min_height_pixels = src_box.get_height() * (1.0 - (max_height_diff * 0.01));
            let max_height_pixels = src_box.get_height() * (1.0 + (max_height_diff * 0.01));
            if ((dest_width < min_width_pixels) || (dest_width > max_width_pixels)) {
                return
            }
            if ((dest_height < min_height_pixels) || (dest_height > max_height_pixels)) {
                return
            }

            // Determine the angle difference from the desired angle.
            // First determine the x and y in the tilted frame of the desired angle.
            let parallel_dist = (diff.x * dir.x) + (diff.y * dir.y)
            let perp_dist = (diff.x * perp_dir.x) + (diff.y * perp_dir.y)
            // Next determine the angle in radians.
            let theta = Math.atan2(perp_dist, parallel_dist);
            let angle = -1 * theta / 3.141592653 * 180.0; // -1 is because y increases from top to bottom, and we want users to think that 0 degress is to the right, and 90 degress is up.
            if (Math.abs(angle) > Math.abs(max_angle_diff)) {
                return
            }

            // Determine the distance.
            let distance = (diff.x*diff.x) + (diff.y * diff.y)

            // We want to skip elements that are in the opposite direction and that are overlaid directly on top of us.
            // For example an image when hovered over may load in a video on top.
            if (parallel_dist < 5) {
                return
            }

            // Update best if the dest is closer to the src element.
            if (!best) {
                best = dest
                best_distance = distance
            } else if (Math.abs(distance - best_distance) < 0.0001) {
                // If the distance and best_distance are very close check their z_index values or their sizes.
                // We choose the one with larger z_index or smaller size.
                {
                    let dest_area = (dest.box.right - dest.box.left) * (dest.box.bottom - dest.box.top)
                    let best_area = (best.box.right - best.box.left) * (best.box.bottom - best.box.top)
                    if (dest_area < best_area) {
                        best = dest
                        best_distance = distance
                    }
                }
            } else if (distance < best_distance) {
                best = dest
                best_distance = distance
            }
        })
        return best
    }
