
// GUI related.
let gui_collection: GUICollection = null

// Communication between content and background.
let mutation_monitor: MutationMonitor = null
let content_comm: ContentComm = null
let content_comm_handler: ContentCommHandler = null

// Event Blocking.
let event_blocker: EventBlocker = null

// Setup content comms.
content_comm = new ContentComm()

// Popups.
gui_collection = new GUICollection(content_comm)
event_blocker = gui_collection.event_blocker // expose inner event blocker, to keep global variable up to date.

// Content Comm Handler.
content_comm_handler = new ContentCommHandler(content_comm, gui_collection)

mutation_monitor = new MutationMonitor(content_comm, gui_collection)
// Start listening for mutations on page load.
document.addEventListener('DOMContentLoaded', ()=>{mutation_monitor.on_loaded()}, false);

// // Modify the dom to remove the elements to allow the user to install our extension.
// document.addEventListener('DOMContentLoaded', () => {
//     let text = document.getElementById('smash_browse_install_text')
//     let button = document.getElementById('smash_browse_install_button')

//     if (text) {
//         text.parentNode.removeChild(text)
//     }
//     if (button) {
//         button.parentNode.removeChild(button)
//     }
// }, false)

