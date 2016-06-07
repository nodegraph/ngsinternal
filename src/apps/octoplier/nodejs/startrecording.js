var result = true

try {
	window.octoplier.event_proxy.connect()
} catch (e) {
	window.octoplier.logger.log_exception(e)
	result = false
}

return result

