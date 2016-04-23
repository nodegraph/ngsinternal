// This runs in on the qml/javascript side. (Not in the browser.)

var NodePool;
var g_node_pool;

try {

	// Constructor for the NodePool class.
    NodePool = function() {
        // Our constructores indexed by compute type.
        this.nodeConstructors = {}
        // Our nodes indexed by node path.
        this.nodes = {}
    }

	// Register a constructor for a certain node type.
	NodePool.prototype.register = function(computeType, constructor) {
		if (computeType in this.nodeConstructors) {
			return false;
		}
		this.nodeConstructors[computeType] = constructor;
		return true;
	}

	// Unregister a certain node type.
	NodePool.prototype.unregister = function(computeType) {
		if (computeType in this.nodeConstructors) {
			return false;
		}
		delete this.nodeConstructors[computeType];
		return true;
	}

	// Create a node of a certain node type and at a certain node path.
	NodePool.prototype.create_js_node = function(nodePath, computeType) {
		// The node name should not exist already.
		if (nodePath in this.nodes) {
			return false;
		}

		// If we have a valid node type construct it.
		if (computeType in this.nodeConstructors) {
			this.nodes[nodePath] = new this.nodeConstructors[computeType]();
			return true;
		} else {
			console.log("error: node pool could not find a register constructur for compute type: " + computeType);
		}

		// Otherwise things have not gone well.
		return false;
	}

	// Destroy a node at a certain node path.
	NodePool.prototype.destroy_js_node = function(nodePath) {
		if (nodePath in this.nodes) {
			delete this.nodes[nodePath];
			return true;
		}
		return false;
	}
	
	NodePool.prototype.destroy_all_js_nodes = function() {
	    for (var key in this.nodes) {
	        if (this.nodes.hasOwnProperty(key)) {
	          delete this.nodes[key];
	        }
	    }
	    return true;
	}

	// Get a node at a certain node path.
	NodePool.prototype.get_js_node = function(nodePath) {
		if (nodePath in this.nodes) {
			return this.nodes[nodePath];
		}
		return null;
	}

	// Create our singleton if it doesn't exist.
	if (typeof (g_node_pool) === "undefined") {
		g_node_pool = new NodePool();
	}

} catch (e) {
	console.log("Error in NodePool: " + e);
}


