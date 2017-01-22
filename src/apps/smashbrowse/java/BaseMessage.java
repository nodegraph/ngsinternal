import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

public class BaseMessage {
	JsonObject obj = null;

    BaseMessage(int id, MessageEnums.MessageType msg_type) {
    	obj = new JsonObject();
    	set_id(id);
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
    
    void set_msg_type(MessageEnums.MessageType msg_type) {
    	obj.addProperty("msg_type", msg_type.ordinal());
    }
    
    MessageEnums.MessageType get_msg_type() {
    	return get_enum(obj, "msg_type", MessageEnums.MessageType.class);
    }
    
	static <E extends Enum<E>> E get_enum(JsonObject obj, String path, Class<E> c) {
		String [] splits = path.split("/");
		JsonObject child = obj;
		// All but the last one will be a JsonObject.
		// The last one will be a primitive.
		for (int i=0; i<splits.length-1; i++) {
			child = child.getAsJsonObject(splits[i]);
		}
        int index = child.get(splits[splits.length-1]).getAsInt();
        E[] values = c.getEnumConstants();
        return values[index];
    }

    static BaseMessage create_from_obj(JsonObject obj) {
        MessageEnums.MessageType msg_type = get_enum(obj, "msg_type", MessageEnums.MessageType.class);
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





