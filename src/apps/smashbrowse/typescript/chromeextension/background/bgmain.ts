
// Communication between backgrond and nodejs.
let browser_wrap: BrowserWrap = null
let bg_comm: BgComm = null
let bg_comm_handler: BgCommHandler = null


browser_wrap = new BrowserWrap()
bg_comm = new BgComm()
bg_comm_handler = new BgCommHandler(bg_comm, browser_wrap)
bg_comm.register_nodejs_request_handler(bg_comm_handler)


