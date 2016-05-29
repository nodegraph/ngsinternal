var result = true

try {
	octoplier.event_proxy.disconnect()
} catch (e) {
	octoplier.logger.log_exception(e)
	result = false
}

result