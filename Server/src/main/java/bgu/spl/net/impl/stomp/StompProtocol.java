package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

import java.util.HashMap;

public class StompProtocol implements StompMessagingProtocol<StompFrame>{
    private boolean shouldTerminate=false;
    private ConnectionsImpl connections;
    private int connectionId;

    public void start(int connectionId, Connections<StompFrame> connections) {
        this.connections=((ConnectionsImpl)connections).getInstance();
        this.connectionId=connectionId;
    }

    public void process(StompFrame message) {
        switch (message.getCommand()){
            case "CONNECT":
                this.connect(message);
                break;
            case "SEND":
                this.send(message);
                break;
            case "SUBSCRIBE":
                this.subscribe(message);
                break;
            case "UNSUBSCRIBE":
                this.unsubscribe(message);
                break;
            case "DISCONNECT":
                this.disconnect(message);
                break;
            case "RECEIPT":
                this.receipt(message);
                break;
        }
    }

    private void connect (StompFrame message) {
        boolean connect=connections.getInstance().connect(connectionId,message.getHeaders().get("login"),message.getHeaders().get("passcode"));
        if (!connect) {// if can't connect with user - send error
            error(message);
            return;
        }
        // send connected frame
        HashMap<String, String> headers = new HashMap<>();
        headers.put("version", message.getHeaders().get("accept-version"));
        connections.getInstance().send(connectionId,new StompFrame("CONNECTED",headers,""));
        receipt(message);// if there's a receipt header - send receipt
    }

    private void send (StompFrame message) {// send a broadcast message to the topic
        HashMap<String, String> headers = new HashMap<>();
        headers.put("Message-id",connections.getInstance().MessageIDtoString());
        connections.getInstance().incrementMessageID();
        headers.put("destination",message.getHeaders().get("destination"));
        connections.getInstance().send(message.getHeaders().get("destination"),new StompFrame("MESSAGE",headers,message.getBody()));
        receipt(message);// if there's a receipt header - send receipt
    }
    private void subscribe (StompFrame message) {// call subscribe function in ConnectionImpl
        connections.getInstance().Subscribe(connectionId, message.getHeaders().get("id"), message.getHeaders().get("destination"));
        receipt(message);// if there's a receipt header - send receipt
    }

    private void unsubscribe (StompFrame message) {// call unsubscribe function in ConnectionImpl
        connections.getInstance().Unsubscribe(message.getHeaders().get("id"),connectionId);
        receipt(message);// if there's a receipt header - send receipt
    }

    private void disconnect (StompFrame message) {
        receipt(message);// if there's a receipt header - send receipt
        connections.getInstance().disconnect(connectionId);// call disconnect function in ConnectionImpl
        shouldTerminate = true;
    }

    private void receipt (StompFrame message) {// if there's a receipt header - send receipt
        if(message.getHeaders().containsKey("receipt")) {
            HashMap<String, String> headers = new HashMap<>();
            headers.put("receipt-id", message.getHeaders().get("receipt"));
            connections.getInstance().send(connectionId, new StompFrame("RECEIPT", headers, ""));
        }
    }

    private void error (StompFrame message) {// if there's an error with user's connection
        // send error message
        HashMap<String, String> headers = message.getHeaders();
        headers.put("message",connections.getInstance().getErrorMessage());
        connections.getInstance().send(connectionId,new StompFrame("ERROR",headers,""));
        shouldTerminate=true;
    }

    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
