import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

public class RequestMessage extends BaseMessage {
    public RequestMessage(JsonObject obj) {
    	super(obj);
    }
    
    public RequestMessage(int id, MessageEnums.WebDriverRequestType request) {
        super(id, MessageEnums.MessageType.kRequestMessage);
        set_request_type(request);
        set_args(new JsonObject());
    }
    
    public RequestMessage(int id, MessageEnums.WebDriverRequestType request, JsonObject args) {
        super(id, MessageEnums.MessageType.kRequestMessage);
        set_request_type(request);
        set_args(args);
    }
    
    void set_request_type(MessageEnums.WebDriverRequestType request) {
    	obj.addProperty("request", request.ordinal());
    }
    
    MessageEnums.WebDriverRequestType get_request_type() {
    	return get_enum(obj, "request", MessageEnums.WebDriverRequestType.class);
    }
    
    void set_args(JsonElement args) {
    	obj.add("args", args);
    }
    
    JsonElement get_args() {
    	return obj.get("args");
    }

}