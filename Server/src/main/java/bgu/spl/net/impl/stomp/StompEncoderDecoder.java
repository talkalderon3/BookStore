package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

public class StompEncoderDecoder implements MessageEncoderDecoder<StompFrame> {
    private byte[] bytes=new byte[1<<10];//start with 1024 bytes
    private int len=0;

    public StompFrame decodeNextByte(byte nextByte) {
        if (nextByte!='\u0000'){
            pushByte(nextByte);
            return null;
        }
        return popFrame();
    }

    public byte[] encode(StompFrame message) {
        StringBuilder result= new StringBuilder(message.getCommand() + "\n");
        for (Map.Entry<String,String> h:message.getHeaders().entrySet()){
            result.append(h.getKey()).append(":").append(h.getValue()).append("\n");
        }
        result.append("\n");
        if (!message.getBody().equals(""))
            result.append(message.getBody());
        return (result+"\u0000").getBytes();
    }

    private void pushByte(byte nextByte){
        if (len >= bytes.length)
            bytes = Arrays.copyOf(bytes, len * 2);
        bytes[len++] = nextByte;
    }

    private StompFrame popFrame(){
        String s = new String(bytes, 0, len, StandardCharsets.UTF_8);
        String[] result=s.split("\n");
        String command=result[0];
        HashMap<String,String> headers= new HashMap<>();
        int i=1;
        while (i<result.length&&!result[i].isEmpty()) {
            String[] splitHeader=result[i].split(":");
            headers.put(splitHeader[0],splitHeader[1]);
            i++;
        }
        i++;
        StringBuilder body= new StringBuilder();
        while (i<result.length) {
            body.append(result[i]);
            i++;
        }
        len = 0;
        return new StompFrame(command, headers, body.toString());
    }
}