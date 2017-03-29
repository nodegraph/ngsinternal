import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

public class BaseMessage {
	JsonObject obj = null;

    BaseMessage(int id, MessageEnums.MessageType msg_type) {
    	obj = new JsonObject();
    	set_id(id);
    	set_receiver_type(MessageEnums.ReceiverType.kWebDriver);
    	set_msg_type(msg_type);
    }
    
    BaseMessage(JsonObject obj) {
    	this.obj = obj;
    }
    
    String to_string() {
        return obj.toString();
    }
    
    void set_id(int id) {
    	obj.addProperty("id", id);
    }
    
    int get_id() {
    	return obj.getAsJsonPrimitive("id").getAsInt();
    }
    
    void set_receiver_type(MessageEnums.ReceiverType receiver_type) {
    	obj.addProperty("receiver_type", receiver_type.get_value());
    }
    
    MessageEnums.ReceiverType get_receiver_type() {
    	return MessageEnums.ReceiverType.get_enum(obj.get("receiver_type").getAsInt());
    }
    
    void set_msg_type(MessageEnums.MessageType msg_type) {
    	obj.addProperty("msg_type", msg_type.get_value());
    }
    
    MessageEnums.MessageType get_msg_type() {
    	return MessageEnums.MessageType.get_enum(obj.get("msg_type").getAsInt());
    }
    
    static BaseMessage create_from_obj(JsonObject obj) {
        MessageEnums.MessageType msg_type = MessageEnums.MessageType.get_enum(obj.get("msg_type").getAsInt());
        switch (msg_type) {
            case kRequestMessage: {
            	return new RequestMessage(obj);
            }
            case kResponseMessage: {
            	return new ResponseMessage(obj);
            }
            case kInfoMessage: {
            	return new InfoMessage(obj);
            }
            default: {
                System.err.println("Error: Attempt to create message from obj without a message type: " + obj.toString());
                return null;
            }
        }
    }

}





