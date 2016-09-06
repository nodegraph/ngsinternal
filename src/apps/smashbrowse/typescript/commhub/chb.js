// Bootstrap file.

process.on('uncaughtException', function (e) {
    console.error("exception caught at top: " + e.message + " with stack: " + e.stack)
});

var requirejs = require('requirejs');

requirejs.config({
    baseUrl: ".",
    nodeRequire: require
});

requirejs(['ch', 'commhub'],
    function (commhub) {
    });

