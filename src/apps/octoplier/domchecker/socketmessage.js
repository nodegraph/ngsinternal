//These classes represents messages that are passed through sockets.
//They act like a dictionaries with key value pairs.


var RequestMessage = function(request) {
    this.request = request
}

var ResponseMessage = function(response) {
    this.response = response
}