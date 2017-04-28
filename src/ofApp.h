#pragma once

#include "ofMain.h"
#include "hytky.h"
#include "GUI.h"
#include "simpleServer.h"

class randomPalette{ 
public:
    static const int MAX_COLORS = 50;
    std::vector<ofColor > colors;
    ofColor getColor(int i);
    ofColor getNewColor();
};


class ofApp : public ofBaseApp {
	public:
                ofApp();
                
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

                hytky H;
                ofImage frame;
                bool recording = false;
                
                ofVec2f cursor_pos;
                ofVec2f cursor_target;
                
                ofVec2f hytky_pos;
                float hytky_scale;
                
                GUI hytky_GUI;
                bool GUI_shown = false;
                //simpleServer server;
                
                randomPalette paletti;
                int drawnLayer = 0;

};
