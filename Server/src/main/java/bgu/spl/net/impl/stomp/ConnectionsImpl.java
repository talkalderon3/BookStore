package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;
public class ConnectionsImpl implements Connections<StompFrame> {
    private AtomicInteger messageId = new AtomicInteger(1); // this is for unique message id in Message frame
    private ConcurrentHashMap<Integer, ConnectionHandler<StompFrame>> activeClients=new ConcurrentHashMap<>();// map of connectionID and ConnectionHandler
    private ConcurrentHashMap<Integer,User> userIDsMap=new ConcurrentHashMap<>();// map of connectionID and users
    private String errorMessage;
    private Object lock=new Object();

    private static class ConnectionsImplSingleton{
        private static ConnectionsImpl Instance = new ConnectionsImpl();
    }

    public static ConnectionsImpl getInstance(){
        return ConnectionsImplSingleton.Instance;
    }

    public boolean send (int connectionId, StompFrame msg) {// send a message to the user with the connectionID
        ConnectionHandler client = activeClients.get(connectionId);
        if(client!=null) {
            client.send(msg); // call send function in ConnectionHandler
            return true;
        }
        else
            return false;
    }

    public void send (String channel, StompFrame msg) {// send a message to all users subscribed to the channel
        synchronized (lock) {// lock so all messages in broadcast will be sent together
            for (Map.Entry<Integer, User> user : userIDsMap.entrySet()) {
                if (user.getValue().getSubscriptions().containsKey(channel)) {
                    HashMap<String, String> headers = msg.getHeaders();
                    headers.put("subscription", user.getValue().getSubscriptions().get(channel));//add subscriptionID header
                    send(user.getKey(), new StompFrame(msg.getCommand(), headers, msg.getBody()));
                }
            }
        }
    }

    public void disconnect (int connectionId) {// disconnect
        activeClients.remove(connectionId);
        userIDsMap.get(connectionId).setConnectStatus(false);
        userIDsMap.get(connectionId).removeFromAllTopics();
    }

    public boolean connect(int connectionId, String userName, String password) {// check if user can connect
        for (Map.Entry<Integer, User> user:userIDsMap.entrySet()){// if the user exists in the system
            if (userName.equals(user.getValue().getUserName())){
                if (user.getValue().isConnected()){// if the user is already connected
                    errorMessage="User already logged in";
                    return false;
                }
                if (!password.equals(user.getValue().getPassword())){// if the password is wrong
                    errorMessage="Wrong password";
                    return false;
                }
                user.getValue().setConnectStatus(true);
                userIDsMap.put(connectionId, user.getValue());
                userIDsMap.remove(user.getKey());
                return true;
            }
        }
        userIDsMap.put(connectionId,new User(userName,password));// if the user never logged in before
        return true;
    }

    public String getErrorMessage(){
        return errorMessage;
    }

    public void Subscribe (int connectionID, String subscriptionID, String topic) {// if the user isn't subscribed to the topic - add the subscription ID
        if (!userIDsMap.get(connectionID).hasTopic(topic))
            userIDsMap.get(connectionID).addSubscription(topic,subscriptionID);
    }

    public void Unsubscribe (String subscriptionID, int connectionID) {// if the user is subscribed to the topic - remove the subscription
        if (userIDsMap.get(connectionID).hasSubscriptionID(subscriptionID))
            userIDsMap.get(connectionID).removeSubscription(subscriptionID);
    }


    public String MessageIDtoString (){// turn the message id to string
        int id=messageId.get();
        String output="";
        while (id<10000) {
            output += 0;
            id=id*10;
        }
        return output+messageId.get();
    }

    public void incrementMessageID(){
        messageId.getAndIncrement();
    }

    public void addActiveClient(int connectionID, ConnectionHandler handler){
        activeClients.put(connectionID,handler);
    }
}
