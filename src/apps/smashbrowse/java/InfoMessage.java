import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

public class InfoMessage extends BaseMessage {
    MessageEnums.InfoType info;
    JsonObject value;
    
    public InfoMessage(JsonObject obj) {
    	super(obj);
    }
    
    public InfoMessage(int id, MessageEnums.InfoType info) {
        super(id, MessageEnums.MessageType.kInfoMessage);
        this.info = info;
        this.value = new JsonObject();
    }
    
    public InfoMessage(int id, MessageEnums.InfoType info, JsonObject value) {
        super(id, MessageEnums.MessageType.kInfoMessage);
        this.info = info;
        this.value = value;
    }
    
    void set_info_type(MessageEnums.InfoType info) {
    	obj.addProperty("info", info.ordinal());
    }
    
    MessageEnums.InfoType get_info_type() {
    	return get_enum(obj, "info", MessageEnums.InfoType.class);
    }
    
    void set_value(JsonElement value) {
    	obj.add("value", value);
    }
    
    JsonElement get_value() {
    	return obj.get("value");
    }
}