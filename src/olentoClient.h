#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class olentoClient : public ofThread {
public:
    ofxOscSender sendr;
    ofxOscReceiver recvr;
    bool connected;
    ofTimer timer;
    std::vector<ofxOscMessage> messages;
    std::string connectionType;
    std::string name;
    std::string defaultAddress;

    olentoClient();
    void setup(std::string address, std::string name, std::string connection_type);
    void threadedFunction();
    
    virtual void handleMessages(ofxOscMessage msg);
    void connectToHub();
    bool checkConnection(ofxOscMessage m);

    
    void addMessage(std::string message, std::string content);
    void addMessages(std::vector<std::string> str_messages, std::string content);
    void addMessages(std::vector<float> flt_messages, std::string content);
    
    
private:

};

// paketin muoto /nimi/connection_type/.../value

