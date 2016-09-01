
export const enum RequestType {
    kShutdown,
    check_browser_is_open
}

export class BaseMessage {
    static create_from_string(s: string): BaseMessage {
        let obj = JSON.parse(s)
        if (obj.hasOwnProperty('request')) {
            return <RequestMessage>obj
        } else {
            return <ResponseMessage>obj
        }
    }

    to_string(): string {
        return JSON.stringify(this)
    }
}

export class RequestMessage extends BaseMessage{
    request: RequestType
    args: any[]
}

export class ResponseMessage extends BaseMessage{
    success: boolean
    value: any
}



export class SocketMessage {
    text: string
    obj: any
    constructor() {
        this.text = ""
        this.obj = null
    }
    set_from_string(s: string) {
        console.log("SocketMessage setting from string: " + s)
        this.text = s
        this.obj = JSON.parse(this.text)
        console.log('check stringified: ' + JSON.stringify(this.obj))
    }
    set_from_obj(o: any) {
        this.text = JSON.stringify(o)
        this.obj = o
        console.log('Socketmessage setting from obj stringified: ' + this.text)
        console.log('check stringified: ' + JSON.stringify(this.obj))
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