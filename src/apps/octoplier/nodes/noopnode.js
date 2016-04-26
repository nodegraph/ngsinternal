// This runs in on the qml/javascript side. (Not in the browser.)

var NoOpNode;

try {
	NoOpNode = function() {
		// Call our parent constructor.
		Node.call(this);
		// Any parameters for our compute operation should
		// be placed in our params map, this.params.
	}

	// Setup our prototype.
	NoOpNode.prototype = Object.create(Node.prototype);
	NoOpNode.prototype.constructor = NoOpNode;

	// Our input plug names.
	NoOpNode.prototype.get_js_input_plug_names = function() {
		return [ "input", "hello" ];
	}
	
	// Our output plug names.
	NoOpNode.prototype.get_js_output_plug_names = function() {
		return [ "output", "there" ];
	}
	
	// Our compute.
	NoOpNode.prototype.perform_js_compute = function() {
		console.log("calling NoOpNode::compute");
		data = this.getInputData("input");
		this.setOutputData("output", data);
		return true;
	}

	// Register this constructor.
	g_node_pool.register("NoOp", NoOpNode);
} catch (e) {
	console.log("Error in NoOpNode definition: " + e);
}


	
	
	
	
	
	
	