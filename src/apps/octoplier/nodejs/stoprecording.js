var result = true

try {
	window.octoplier.event_proxy.disconnect()
} catch (e) {
	window.octoplier.logger.log_exception(e)
	result = false
}

return result