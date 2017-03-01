#include "simpleServer.h"


string luoViesti() {
    static int refCount = 0;
    string viesti = "hei maailma " + ofToString(refCount);
    refCount++;
    return viesti;
}


void simpleServer::setup() {
    letsRun = false;
    port = 12345;
}


void simpleServer::update() {
    connectedIPs.clear();
    
    //Jos serveri ei vielä ole käynnissä, mutta run-nappia on painettu, käynnistetään se
    if(server.isConnected() == false) {
        if(letsRun) {
            server.setup(port);
            
            //jos setup ei onnistu, ei yritetä heti uudestaan vaan resetoidaan run-pyyntö
            if(server.isConnected() == false) {
                letsRun = false;
            }
        }
    }
    
    //Jos serveri on käynnissä:
    //  Jos run-nappia painettiin, suljetaan serveri. HUOM: jos näin tehdään, socket jää roikkumaan ja serveriä ei enää voida käynnistää,
    //          ennen kuin server- ja client-ohjelmat on kokonaan suljettu! 
    //  Jos run-nappia ei ole painettu eli se on pohjassa, lähetetään viestejä clienteille
    if(server.isConnected() ) {
        if(letsRun == false) {
            server.disconnectAllClients();
            std::cout << "disconnected clients\n";
            server.close();
            std::cout << "closed server\n";
        }
        
        else {
            //fill connected IP's list
            for(int i=0; i < server.getLastID(); i++) {
                if(server.isClientConnected(i) ) {
                    connectedIPs.push_back(server.getClientIP(i) );
                    while(!viestijono.empty() ) {
                        server.send(i, viestijono[0] );
                        viestijono.erase(viestijono.begin() );
                    }

                }
            }
        }
    }    
}


bool simpleServer::isRunning() {
    return server.isConnected();
}


bool simpleServer::send(std::string str) {
    if(!connectedIPs.empty() && viestijono.size() < MAX_SIZE) {
        viestijono.push_back(str);
        return true;
    }
    else return false;
}