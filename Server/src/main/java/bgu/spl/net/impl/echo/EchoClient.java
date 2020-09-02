package bgu.spl.net.impl.echo;

import bgu.spl.net.impl.stomp.StompFrame;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.HashMap;

public class EchoClient {

    public static void main(String[] args) throws IOException {

        if (args.length == 0) {
            args = new String[]{"localhost", "hello"};
        }

        if (args.length < 2) {
            System.out.println("you must supply two arguments: host, message");
            System.exit(1);
        }

        //BufferedReader and BufferedWriter automatically using UTF-8 encoding
        try (Socket sock = new Socket(args[0], 7777);
                BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
                BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()))) {
            System.out.println("sending message to server");
            HashMap<String, String> headers = new HashMap<>();
            headers.put("login","noam");
            headers.put("passcode","thtr");
            headers.put("accept-version","1.2");
            headers.put("host","vngf");

            out.write((new StompFrame(
                    "CONNECT",
                    headers,
                    "")).toString() + '\0');
            out.flush();

            System.out.println("awaiting response");
            String line=in.readLine();
            while (line != null){
                System.out.println(line);
                line=in.readLine();
            }
        }
    }
}
