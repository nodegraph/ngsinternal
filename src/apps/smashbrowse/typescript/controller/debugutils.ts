/// <reference path="D:\dev\windows\DefinitelyTyped\node\node.d.ts"/>

// Catch any exception at the top level.
process.on('uncaughtException', function(e: Error) {
    console.error("exception caught at top: " + e.message + " with stack: " + e.stack)
});

// Helper to print exceptions.
function log_exception(e: Error): void {
    console.error("caught exception: " + e.message + " stack: " + e.stack)
}