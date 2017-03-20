import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

public class ResponseMessage extends BaseMessage {
    public ResponseMessage(JsonObject obj) {
    	super(obj);
    }
    
    public ResponseMessage(int id, boolean no_errors) {
        super(id, MessageEnums.MessageType.kResponseMessage);
        set_no_errors(no_errors);
        set_value(new JsonObject());
    }
    
    public ResponseMessage(int id, boolean success, JsonObject value) {
        super(id, MessageEnums.MessageType.kResponseMessage);
        set_no_errors(success);
        set_value(value);
    }
    
    void set_no_errors(boolean success) {
    	obj.addProperty("success", success);
    }
    
    boolean get_no_errors() {
    	return obj.get("success").getAsBoolean();
    }
    
    void set_value(JsonElement value) {
    	obj.add("value", value);
    }
    
    JsonElement get_value() {
    	return obj.get("value");
    }
}