#pragma once

#include <vector>
#include <string>

#include "ofMain.h"
#include "ofxNetwork.h"


class simpleServer{
public:    
    const int MAX_SIZE = 50;
    void setup();
    void update();

    bool isRunning();
    bool send(std::string viesti);
    
    bool letsRun;
    int port;
    
    std::vector<std::string> connectedIPs;
    std::vector<std::string> viestijono;
    ofxTCPServer server;
};
