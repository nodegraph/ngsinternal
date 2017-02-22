class FrameInfo {
    frame_id: number // From chrome api.
    parent_frame_id: number // From chrome api.
    frame_index_path: string // String composed of indices of iframes.

    bounds: IBox

    children: FrameInfo[]
    parent: FrameInfo

    constructor(frame_id: number, parent_frame_id: number) {
        this.frame_id = frame_id
        this.parent_frame_id = parent_frame_id
        this.frame_index_path = ''

        this.bounds = {left:0, right:0, bottom: 0, top: 0}

        this.children = []
        this.parent = null
    }

    get_offset() {
        let offset: IPoint = {x:0, y: 0}
        let info: FrameInfo = this
        while (info) {
            offset.x += info.bounds.left
            offset.y += info.bounds.top
            info = info.parent
        }
        return offset
    }

    convert_box_to_global_space(box: Box) {
        let offset = this.get_offset()
        box.left += offset.x
        box.right += offset.x
        box.top += offset.y
        box.bottom += offset.y
    }

    convert_box_to_local_space(box: Box) {
        let offset = this.get_offset()
        box.left -= offset.x
        box.right -= offset.x
        box.top -= offset.y
        box.bottom -= offset.y
    }

    convert_point_to_global_space(point: Point) {
        let offset = this.get_offset()
        point.x += offset.x
        point.y += offset.y
    }

    convert_point_to_local_space(point: Point) {
        let offset = this.get_offset()
        point.x -= offset.x
        point.y -= offset.y
    }
}

class FrameInfos {
    infos: FrameInfo[]
    root: FrameInfo

    constructor() {
        this.infos = []
        this.root = null
    }

    clear() {
        this.infos.length = 0
        this.root = null
    }

    initialize(details: chrome.webNavigation.GetAllFrameResultDetails[]) {
        // Clear existing infos.
        this.clear()

        // Create infos.
        for (let d of details) {
            let info = new FrameInfo(d.frameId, d.parentFrameId)
            this.infos.push(info)
        }

        // Form links between infos and find the root.
        for (let child of this.infos) {
            // The parent frame id is -1 for the root.
            if (child.parent_frame_id < 0) {
                this.root = child
                break
            }
            // Otherwise find the parent and form a bi-directional link.
            let found = false
            for (let parent of this.infos) {
                if (child.parent_frame_id == parent.frame_id) {
                    parent.children.push(child)
                    child.parent = parent
                    found = true
                    break
                }
            }
            if (!found) {
                console.log("Error: unable to find parent frame during FrameInfos::initialize")
            }
        }
    }

    get_info_by_id(frame_id: number) {
        for (let info of this.infos) {
            if (info.frame_id == frame_id) {
                return info
            }
        }
        return null
    }

    get_info_by_index_path(frame_index_path: string) {
        for (let info of this.infos) {
            if (info.frame_index_path == frame_index_path) {
                return info
            }
        }
        return null
    }

    set_index_path(frame_id: number, frame_index_path: string) {
        let info = this.get_info_by_id(frame_id)
        if (!info) {
            console.error("could not set index path for frame_id: " + frame_id + " " + frame_index_path)
            return
        }
        info.frame_index_path = frame_index_path
    }

    set_bounds(frame_index_path: string, bounds: IBox) {
        let info = this.get_info_by_index_path(frame_index_path)
        if (!info) {
            console.error("could not set bounds for frame_index_path: " + frame_index_path + " " + JSON.stringify(bounds))
            return
        }
        info.bounds = bounds
    }




}