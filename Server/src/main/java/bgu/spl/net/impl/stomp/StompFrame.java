package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;

public class StompFrame {
    private String command;
    private HashMap<String,String>headers;
    private String body;

    public StompFrame (String command, HashMap<String,String> headers, String body){
        this.command=command;
        this.headers=headers;
        this.body=body;
    }

    public String getCommand(){
        return command;
    }

    public HashMap<String,String> getHeaders(){
        return headers;
    }

    public String getBody(){
        return body;
    }

    public String toString(){
        String output="";
        output+=command+"\n";
        for (Map.Entry<String,String> h:getHeaders().entrySet()){
            output+=h.getKey()+":"+h.getValue()+"\n";
        }
        output+="\n"+body;
        return output;
    }
}
