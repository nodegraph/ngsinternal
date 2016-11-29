
export class DebugUtils {
    static catch_uncaught_exceptions() {
        // Catch any exception at the top level.
        process.on('uncaughtException', function (e: Error) {
            console.error("exception caught at top: " + e.message + " with stack: " + e.stack)
        });
    }

    // Helper to print exceptions.
    static log_exception(e: Error): void {
        console.error("caught exception: " + e.message + " stack: " + e.stack)
    }

}