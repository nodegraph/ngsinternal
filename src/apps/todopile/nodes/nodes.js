// This runs in on the qml/javascript side. (Not in the browser.)

var inc1 = Qt.include("nodepool.js");
var inc2 = Qt.include("node.js");
var inc3 = Qt.include("noopnode.js");

if (inc1.status) {
	console.log("error including nodepool.js: " + inc1.status + " exception: " + inc1.exeption);
}

if (inc2.status) {
	console.log("error including node.js: " + inc2.status + " exception: " + inc2.exception);
}

if (inc3.status) {
	console.log("error including noopnode.js: " + inc3.status + " exception: " + inc3.exception);
}





	
	
	
	
	
	

	
	
	