import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

public class ResponseMessage extends BaseMessage {
    public ResponseMessage(JsonObject obj) {
    	super(obj);
    }
    
    public ResponseMessage(int id, boolean success) {
        super(id, MessageEnums.MessageType.kResponseMessage);
        set_success(success);
        set_value(new JsonObject());
    }
    
    public ResponseMessage(int id, boolean success, JsonElement value) {
        super(id, MessageEnums.MessageType.kResponseMessage);
        set_success(success);
        set_value(value);
    }
    
    void set_success(boolean success) {
    	obj.addProperty("success", success);
    }
    
    boolean get_success() {
    	return obj.get("success").getAsBoolean();
    }
    
    void set_value(JsonElement value) {
    	obj.add("value", value);
    }
    
    JsonElement get_value() {
    	return obj.get("value");
    }
}