

class SocketMessage {
    text: string
    obj: any
    constructor() {
        this.text = ""
        this.obj = null
    }
    set_from_string(s: string) {
        this.text = s
        this.obj = JSON.parse(this.text)
    }
    set_from_obj(o: any) {
        this.text = JSON.stringify(o)
        this.obj = o
    }

    get_obj() {
        return this.obj
    }

    get_text() {
        return this.text
    }

    is_request() {
        return this.obj.hasOwnProperty('request')
    }

    is_response() {
        return !this.is_request()
    }
}