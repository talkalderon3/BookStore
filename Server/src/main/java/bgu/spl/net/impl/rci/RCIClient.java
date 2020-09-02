package bgu.spl.net.impl.rci;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.impl.stomp.StompEncoderDecoder;
import bgu.spl.net.impl.stomp.StompFrame;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.Closeable;
import java.io.IOException;
import java.io.Serializable;
import java.net.Socket;

public class RCIClient implements Closeable {

    private final MessageEncoderDecoder encdec;
    private final Socket sock;
    private final BufferedInputStream in;
    private final BufferedOutputStream out;

    public RCIClient(String host, int port) throws IOException {
        sock = new Socket("localhost", 7777);
        encdec = new StompEncoderDecoder();
        in = new BufferedInputStream(sock.getInputStream());
        out = new BufferedOutputStream(sock.getOutputStream());
    }

    public void send(StompFrame cmd) throws IOException {
        out.write(encdec.encode(cmd));
        out.flush();
    }

    public StompFrame receive() throws IOException {
        int read;
        while ((read = in.read()) >= 0) {
            StompFrame msg = (StompFrame)encdec.decodeNextByte((byte) read);
            if (msg != null) {
                return msg;
            }
        }

        throw new IOException("disconnected before complete reading message");
    }

    @Override
    public void close() throws IOException {
        out.close();
        in.close();
        sock.close();
    }

}
