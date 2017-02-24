class FrameInfo {
    readonly frame_id: number // From chrome api. This uniquely identifies the frame among all nested frames in the page.
    readonly parent_frame_id: number // From chrome api. This identifies the parent frame.

    // From testing it seems the window.frames ordering is different from document.getElementsByTagName("iframe") ordering.

    // String composed of indices to window.frames down to our frame.  
    // This also uniquely identifies the frame among all nested frames in the page.
    fw_index_path: string 

    // Index into parent frame's document.getElementsByTagName("iframe") array. 
    // This uniquely identifies the frame for a given parent frame.
    element_index: number 

    bounds: IBox

    children: FrameInfo[]
    parent: FrameInfo

    constructor(frame_id: number, parent_frame_id: number) {
        this.frame_id = frame_id
        this.parent_frame_id = parent_frame_id
        this.fw_index_path = ''
        this.element_index = -1

        this.bounds = {left:0, right:0, bottom: 0, top: 0}

        this.children = []
        this.parent = null
    }

    is_root() {
        if (this.frame_id == 0) {
            return true
        }
        return false
    }

    calculate_offset() {
        let offset: IPoint = {x:0, y: 0}
        let info: FrameInfo = this
        while (info) {
            offset.x += info.bounds.left
            offset.y += info.bounds.top
            info = info.parent
        }
        return offset
    }

    calculate_fe_index_path() {
        let path = ''
        let info: FrameInfo = this
        while (info && (!info.is_root())) {
            if (path == '') {
                path = info.element_index.toString()
            } else {
                path = info.element_index.toString() + '/' + path
            }
            info = info.parent
        }
        return path
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

    get_info_by_id(frame_id: number) {
        for (let info of this.infos) {
            if (info.frame_id == frame_id) {
                return info
            }
        }
        return null
    }

    get_info_by_fw_index_path(fw_index_path: string) {
        for (let info of this.infos) {
            if (info.fw_index_path == fw_index_path) {
                return info
            }
        }
        return null
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
        let root_count = 0
        for (let child of this.infos) {
            // The parent frame id is -1 for the root.
            if (child.is_root()) {
                this.root = child
                root_count += 1
                continue
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
        if (root_count > 1) {
            console.log("Error: found more than one root frame: " + root_count)
        }

        //this.dump_tree()
    }

    dump_parenting() {
        for (let child of this.infos) {
            let parent = child
            console.log('----------------------')
            while(parent) {
                console.log(parent.fw_index_path)
                parent = parent.parent
            }
        }
    }

    dump(info: FrameInfo) {
        console.log('tree element: ' + info.fw_index_path)
        for (let child of info.children) {
            this.dump(child)
        }
    }

    dump_tree() {
        this.dump(this.root)
    }

    dump_array() {
        for (let info of this.infos) {
            console.log('frame index path: ' + info.fw_index_path + ' info.parent_frame_id: ' + info.parent_frame_id + ' info.frame_id: ' + info.frame_id)
            console.log('parent: ' + info.parent + ' num children: ' + info.children.length)
        }
    }
}