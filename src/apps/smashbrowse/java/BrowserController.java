import java.net.Socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class BrowserController {
    public static void main(String[] args) {
        System.out.println("Hello, World!");
        
        Gson gson = new Gson();
        
        System.out.println(gson.fromJson("{'id':1,'firstName':'Lokesh','lastName':'Gupta','roles':['ADMIN','MANAGER']}", Object.class));
        
        Scanner s = new Scanner(System.in);
        
        while (true) {
//        	int num_bytes = s.nextInt();
//        	String data = "";
//        	while(data.length() < num_bytes) {
//        		data += s.nextLine();
//        	}

        	String data = s.nextLine();
	        
//	        for (s.nextString())
//	        while (s.hasNextInt()) {
//	            A[i] = s.nextInt();
//	            i++;
//	        }
        	
        	System.out.println("got data: " + data);
        }
        
        //System.out.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
        
//        try {
//		} catch (IOException e) {
//			e.printStackTrace();
//		}

    }
}