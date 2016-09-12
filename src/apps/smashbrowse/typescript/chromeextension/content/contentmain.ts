
// GUI related.
let gui_collection: GUICollection = null

// Communication between content and background.
let mutation_monitor: MutationMonitor = null
let content_comm: ContentComm = null
let content_comm_handler: ContentCommHandler = null

// Context menu handling.
let context_menu_handler: PageOverlaysHandler = null

// If we're not the top window (ie: an IFrame), then we just block all events.
let event_blocker: EventBlocker = null

// Popups.
gui_collection = new GUICollection()
event_blocker = gui_collection.event_blocker // expose inner event blocker, to keep global variable up to date.

// Setup content comms.
content_comm = new ContentComm()
content_comm_handler = new ContentCommHandler(content_comm, gui_collection)

mutation_monitor = new MutationMonitor(content_comm, gui_collection)
// Start listening for mutations on page load.
document.addEventListener('DOMContentLoaded', mutation_monitor.on_loaded.bind(mutation_monitor), false);

// Context menu handling.
context_menu_handler = new PageOverlaysHandler(content_comm, gui_collection)
gui_collection.page_overlays.set_handler(context_menu_handler)

console.log("create a new instance of content script!")

