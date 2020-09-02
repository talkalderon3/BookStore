package bgu.spl.net.impl.stomp;

import java.util.HashMap;

public class User {
    private String userName;
    private String password;
    private boolean isConnected;
    private HashMap<String,String> subscriptionIDs=new HashMap<>();

    public User(String userName, String password){
        this.userName=userName;
        this.password=password;
        isConnected=true;
    }
    public String getPassword(){
        return password;
    }

    public String getUserName(){
        return userName;
    }

    public boolean isConnected (){
        return isConnected;
    }

    public void setConnectStatus (boolean isConnected){
        this.isConnected=isConnected;
    }

    public void addSubscription(String topic, String subscriptionID){
        subscriptionIDs.put(topic,subscriptionID);
    }

    public boolean hasSubscriptionID(String subscriptionID){
        return subscriptionIDs.containsValue(subscriptionID);
    }

    public boolean hasTopic(String topic){
        return subscriptionIDs.containsKey(topic);
    }


    public void removeSubscription(String subscriptionID){
        subscriptionIDs.values().remove(subscriptionID);
    }

    public HashMap<String,String> getSubscriptions(){
        return subscriptionIDs;
    }

    public void removeFromAllTopics(){
        subscriptionIDs=new HashMap<>();
    }
}
