#pragma once

#include <string>
#include <vector>

#include "ofMain.h"

class GUI_element{
public:
    int x, y; //paikka on suhteellinen sisältävään elementtiin
    int w, h;
    
    GUI_element(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0);
    
    virtual void draw(int x0 = 0, int y0 = 0) {}
    virtual void update() {}
    virtual void handleKey(int key) {}
    virtual void handleClick(int mouse_x, int mouse_y) {}
    
    bool pointIsInside(int p_x, int p_y); //true jos annettu piste on elementin sisällä
};


class clickable : public GUI_element{
public:
    
    clickable(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0);
    
    bool clicked = false;
    bool disabled = false; //just disable clicking; state remains unchanged
      
    bool getState(); //true if clicked. also resets state
    bool peek();//true if clicked, doesn't reset state
    void resetState();//reset clicked state to false
    
    void handleClick(int mouse_x, int mouse_y);
};


class button : public clickable{
public:
    std::string label;    

    button(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0, std::string text = "");
    void draw(int x0 = 0, int y0 = 0);
};


class textField : public clickable{
public:

    enum textField_mode_t {
        TEXTFIELD_TYPING, TEXTFIELD_DEFAULT
    } mode;

    int max_l;
    std::string content;
    std::string label;

    textField(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0, std::string text = "", int max_length = 0);

    void draw(int x0, int y0);
    void handleKey(int key);
    void handleClick(int m_x, int m_y);
};


class console : public GUI_element {
public:
    console(int x_ = 0, int y_ = 0, int max_length_ = 0, int max_rows = 0);
    
    int rows; //leveys ja korkeus merkkeinä
    int max_length; 
    
    std::vector<std::string> contents;
    
    void draw(int x0 = 0, int y0 = 0);
    void add(const std::string& str);
};


class GUI : public GUI_element {
public:    
    GUI(int x_=0, int y_=0, int w_ = 0, int h_ = 0);
    
    textField portField;
    button runButton;
    console outputConsole;
    
    bool run = false;
    int port = 12345;
    
    void setup();
    void draw();
    void update();
    void handleKey(int key);
    void handleClick(int mouse_x, int mouse_y);
    
    void print(std::string);
    void print(std::vector<std::string>);
    void clearConsole();
};