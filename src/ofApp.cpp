#include "ofApp.h"

#include <string>


ofColor randomPalette::getColor(int i) {
    if(i > MAX_COLORS) i = MAX_COLORS;
    
    while(i >= colors.size() ) {
        colors.push_back(ofColor(ofRandom(50, 255),ofRandom(50, 255),ofRandom(50, 255) ) );
        //std::cout << "created color " << colors.size()-1 << "\n";
    }
    return colors[i];
}


ofColor randomPalette::getNewColor() {
    colors.push_back(ofColor(ofRandom(255),ofRandom(255),ofRandom(255) ) );
    return colors.back();
}


std::string getFilename() {
    static int frame_n = 0;
    
    std::string result = "frames/frame" + std::to_string(frame_n) + ".png";
    frame_n++;
    cout << result << "\n";
    return result;
}


ofApp::ofApp() : H(4, 2),
        cursor_pos(0,0), cursor_target(0,0), 
        hytky_pos(ofGetWidth() / 2, ofGetHeight() /2) {
    
    hytky_scale = (float)(ofGetHeight()-100) / (hytky::huone.h);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(0, 0, 0);
    hytky_GUI.setup();
    server.setup();
    
    H.asetaJousivakio(3);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //katso tiedot GUI:sta ja päivitä serveriin
    hytky_GUI.update();    
    
    server.port = hytky_GUI.port;
    server.letsRun = hytky_GUI.run;
    server.update();
    
    //tarkista tiedot serveristä ja päivitä GUI
    if(server.isRunning() ) {        
        hytky_GUI.clearConsole();
        hytky_GUI.print("Server is running on port " + ofToString(server.port));
        if(!server.connectedIPs.empty()) {
            hytky_GUI.print("Now sending to clients: ");
            hytky_GUI.print(server.connectedIPs);
        }
        else {
            hytky_GUI.print("No clients connected");
        }
        
        //lähetetään jousista tietoja
        server.send(H.kerro() );
    }

    hytky_GUI.run = server.isRunning();


    //lasketaan hytky
    ofVec2f cursor_v = (cursor_target - cursor_pos);
    cursor_pos += cursor_v * 0.9;

    H.repulsor.keskipiste = vec2((cursor_pos.x - hytky_pos.x) / hytky_scale, (cursor_pos.y - hytky_pos.y) / hytky_scale);
    
    H.step();
}

//--------------------------------------------------------------
void ofApp::draw(){
        
    //ofSetColor(20,50,100);
    //ofDrawCircle(cursor_pos.x, cursor_pos.y, 100);
       
    //piirretään jouset
    int layer = drawnLayer;
    ofSetColor(paletti.getColor(layer));
    for(int j=0; j<H.joustenMaara(layer) ; j++) {
        vec2 alku = H.haeJousenAlku(j, layer);
        vec2 loppu = H.haeJousenLoppu(j, layer);

        ofDrawLine(alku.x * hytky_scale + hytky_pos.x, 
                alku.y * hytky_scale + hytky_pos.y, 
                loppu.x * hytky_scale + hytky_pos.x, 
                loppu.y * hytky_scale + hytky_pos.y);
    }
    
    ofSetColor(255,200,150);  
    
    //piirretään pisteet
    for(int i=0; i<H.size(); i++) {
        vec2 p = H.at(i);
        ofDrawCircle(p.x * hytky_scale + hytky_pos.x, p.y * hytky_scale + hytky_pos.y, hytky_scale * 1.5);
    }
    
    if(recording) {
        frame.grabScreen(0,0,ofGetWidth(), ofGetHeight() );
        frame.save(getFilename());
        
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Recording!", 800, 40);
    }

    if(GUI_shown) {
        hytky_GUI.draw();
        ofSetColor(180, 180, 180);
        ofDrawBitmapString("press F11 to hide options", 800, 20);
    }
    else {
        ofSetColor(180, 180, 180);
        ofDrawBitmapString("press F11 to show options", 800, 20);
    }
        
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_F11)
        GUI_shown = !GUI_shown;
        
    else if(GUI_shown)
        hytky_GUI.handleKey(key);
    
    if(key=='r')
        recording = !recording;
    
    else if(key == '+') {
        drawnLayer ++;
        if(drawnLayer >= H.springLayers)
            drawnLayer = 0;
    }
    
    else if (key == ' ') {
        H.extending = !H.extending;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){    
    if(!GUI_shown) {
        cursor_target.x = x;
        cursor_target.y = y;
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(GUI_shown) 
        hytky_GUI.handleClick(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}