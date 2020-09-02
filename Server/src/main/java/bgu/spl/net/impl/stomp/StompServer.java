package bgu.spl.net.impl.stomp;



import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Server;

import java.util.function.Supplier;

public class StompServer {

    public static void main(String[] args) {
        Supplier<StompMessagingProtocol<StompFrame>> protocol=StompProtocol::new;
        Supplier<MessageEncoderDecoder<StompFrame>> encdec=StompEncoderDecoder::new;

        if (args[1].equals("tpc")){
            Server.threadPerClient(
                    Integer.parseInt(args[0]), //port
                    protocol, //protocol factory
                    encdec//message encoder decoder factory
            ).serve();
        }

        if (args[1].equals("reactor")){
            //from the newsfeed example
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    Integer.parseInt(args[0]), //port
                    protocol, //protocol factory
                    encdec //message encoder decoder factory
            ).serve();
        }
    }


}
