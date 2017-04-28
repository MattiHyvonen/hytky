#include "olentoClient.h"

olentoClient::olentoClient() {
    sendr.setup("localhost", 1337);
    recvr.setup(1338);


}

void olentoClient::setup(std::string address, std::string name_, std::string connection_type) {

    sendr.setup(address, 1337);
    connectionType = connection_type;
    name = name_;
    defaultAddress = "/" + name + "/" + connectionType;
    startThread(true);
    timer.setPeriodicEvent(100000000);
}

void olentoClient::threadedFunction() {

    while (isThreadRunning()) {
        //check connection
        if (!connected) {
            connectToHub();
            timer.waitNext();

            if (recvr.hasWaitingMessages()) {
                for (ofxOscMessage m; recvr.getNextMessage(m);) {
                    if (checkConnection(m))
                        connected = true;
                }
            }

        } else {

            if (recvr.hasWaitingMessages()) {
                ofxOscMessage msg;
                while (recvr.getNextMessage(msg)) {
                    handleMessages(msg);
                }
            }

            for (int i = 0; i < messages.size(); i++)
                sendr.sendMessage(messages[i], false);

            messages.clear();
        }
    }
}

void olentoClient::connectToHub() {
    ofxOscMessage m;
    m.setAddress("/" + name + "/connection");
    cout << "connecting.. " << m.getAddress() << "\n";
    cout.flush();
    m.addStringArg("true");
    sendr.sendMessage(m, false);
}

bool olentoClient::checkConnection(ofxOscMessage m) {
    if (m.getAddress() == "/connected")
        if (m.getArgAsString(0) == "true") {
            cout << "connected!\n";
            return true;
        }
    return false;
}

void olentoClient::handleMessages(ofxOscMessage msg) {
    if (msg.getAddress() == "/connected") {
        if (msg.getArgAsString(0) == "false")
            connected = false;
    }
}

void olentoClient::addMessage(std::string message, std::string content) {
    olentoClient::lock();
    ofxOscMessage msg;
    msg.setAddress(defaultAddress + content);
    msg.addStringArg(message);
    messages.push_back(msg);
    olentoClient::unlock();

}

void olentoClient::addMessages(std::vector<std::string> str_messages, std::string content) {
    olentoClient::lock();
    ofxOscMessage msg;
    msg.setAddress(defaultAddress + content);
    for (int i = 0; i < str_messages.size(); i++)
        msg.addStringArg(str_messages[i]);

    messages.push_back(msg);
    olentoClient::unlock();
}

void olentoClient::addMessages(std::vector<float> flt_messages, std::string content) {
    olentoClient::lock();
    ofxOscMessage msg;
    msg.setAddress(defaultAddress + content);
    for (int i = 0; i < flt_messages.size(); i++)
        msg.addFloatArg(flt_messages[i]);

    messages.push_back(msg);
    olentoClient::unlock();
}