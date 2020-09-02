package bgu.spl.net.impl.newsfeed;

import bgu.spl.net.impl.rci.RCIClient;
import bgu.spl.net.impl.stomp.StompFrame;

import java.util.HashMap;

public class NewsFeedClientMain {

    public static void main(String[] args) throws Exception {
        if (args.length == 0) {

            args = new String[]{"127.0.0.1"};
        }

//        System.out.println("running clients");
        runFirstClient("localhost");
        runSecondClient("localhost");
        runThirdClient("localhost");
    }

    private static void runFirstClient(String host) throws Exception {
        try (RCIClient c = new RCIClient("localhost", 7777)) {
            HashMap<String, String> headers = new HashMap<>();
            headers.put("login","noam");
            headers.put("passcode","thtr");
            headers.put("accept-version","1.2");
            headers.put("host","vngf");
            c.send(new StompFrame(
                    "CONNECT",
                    headers,
            ""));


            System.out.println(c.receive()); //ok
            HashMap<String, String> headers2 = new HashMap<>();
            headers2.put("receipt","1");
            c.send(new StompFrame(
                    "DISCONNECT",
                    headers2,
                    ""));
            System.out.println("second client received: " + c.receive());
        }

    }

    private static void runSecondClient(String host) throws Exception {
        try (RCIClient c = new RCIClient("localhost", 7777)) {
            HashMap<String, String> headers = new HashMap<>();
            headers.put("login","tal");
            headers.put("passcode","thtr");
            headers.put("accept-version","1.2");
            headers.put("host","vngf");
            c.send(new StompFrame(
                    "CONNECT",
                    headers,
                    ""));
            System.out.println("second client received: " + c.receive());
            HashMap<String, String> headers2 = new HashMap<>();
            headers2.put("receipt","1");
            c.send(new StompFrame(
                    "DISCONNECT",
                    headers2,
                    ""));
            System.out.println("second client received: " + c.receive());
        }
    }

    private static void runThirdClient(String host) throws Exception {
        try (RCIClient c = new RCIClient("localhost", 7777)) {
            HashMap<String, String> headers = new HashMap<>();
            headers.put("login","dan");
            headers.put("passcode","sf");
            headers.put("accept-version","1.2");
            headers.put("host","vngf");
            c.send(new StompFrame(
                    "CONNECT",
                    headers,
                    ""));
            System.out.println("third client received: " + c.receive());
            HashMap<String, String> headers2 = new HashMap<>();
            headers2.put("receipt","1");
            c.send(new StompFrame(
                    "DISCONNECT",
                    headers2,
                    ""));
            System.out.println("second client received: " + c.receive());
        }
    }
}
