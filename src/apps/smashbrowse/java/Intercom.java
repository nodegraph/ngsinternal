import java.net.ServerSocket;
import java.net.Socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Intercom {
    public static void main(String[] args) {
        System.out.println("Hello, World!");
        
		try {
			ServerSocket server = new ServerSocket(80);

			System.out.println("Server has started on 127.0.0.1:80.\r\nWaiting for a connection...");

			Socket client = server.accept();

			System.out.println("A client connected.");

			InputStream in = client.getInputStream();

			OutputStream out = client.getOutputStream();

			new Scanner(in, "UTF-8").useDelimiter("\\r\\n\\r\\n").next();
			
			//translate bytes of request to string
			String data = new Scanner(in,"UTF-8").useDelimiter("\\r\\n\\r\\n").next();

			Matcher get = Pattern.compile("^GET").matcher(data);

//			if (get.find()) {
//			    Matcher match = Pattern.compile("Sec-WebSocket-Key: (.*)").matcher(data);
//			    match.find();
//			    byte[] response = ("HTTP/1.1 101 Switching Protocols\r\n"
//			            + "Connection: Upgrade\r\n"
//			            + "Upgrade: websocket\r\n"
//			            + "Sec-WebSocket-Accept: "
//			            + DatatypeConverter
//			            .printBase64Binary(
//			                    MessageDigest
//			                    .getInstance("SHA-1")
//			                    .digest((match.group(1) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
//			                            .getBytes("UTF-8")))
//			            + "\r\n\r\n")
//			            .getBytes("UTF-8");
//
//			    out.write(response, 0, response.length);
//			    System.out.println("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
//			} else {
//				System.out.println("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
//			}
			
		} catch (IOException e) {
			e.printStackTrace();
		}

    }
}