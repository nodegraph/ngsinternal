// Communication between backgrond and nodejs.
let browser_wrap: BrowserWrap = null
let bg_comm: BgComm = null
let bg_comm_handler: BgCommHandler = null

if (window == window.top) {
    // Setup bg comms.
    browser_wrap = new BrowserWrap()
    bg_comm = new BgComm()
    bg_comm_handler = new BgCommHandler(bg_comm, browser_wrap)
}