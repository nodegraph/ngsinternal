var result = true

try {
	octoplier.event_proxy.connect()
} catch (e) {
	octoplier.logger.log_exception(e)
	result = false
}

result


