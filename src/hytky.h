#pragma once

#include <vector>
#include <vector2.h>
#include <string>
#include "hytkyClient.h"

class rectangle{
public:
    float x, y, w, h;
    
    rectangle(float x_ = 0, float y_ = 0, float w_ = 0, float h_ = 0) 
    : x(x_), y(y_), w(w_), h(h_) {}
    
    bool onkoSisalla(const vec2& p);
    vec2 vektoriUlkoaSisaan(const vec2& p); //palauta vektori, joka osoittaa annetusta pisteestä nelikulmion sisään (reunaan)
};


class circle{
public:
    vec2 keskipiste;
    float r;
    
    circle(float x=0, float y=0, float r_=0) : keskipiste(x,y), r(r_) {}
    
    bool onkoSisalla(const vec2& p);
    vec2 vektoriSisaltaReunalle(const vec2& p); //palauta vektori, joka osoittaa annetusta pisteestä ympyrän kehälle, kun piste on ympyrän sisällä
};


class hytky: public hytkyClient {
private:

    class piste;  //hytkyn partikkeli
    class jousi;  //pisteiden välillä oleva jousi
    
    std::vector<piste> pisteet; //kaikki hytkyn partikkelit
    std::vector<piste> initialPoints;
    
    std::vector<std::vector<jousi> > jouset; //kaikki pisteiden välillä olevat jouset
    std::vector<jousi> keskiJouset; //jouset jotka pitävät hytkyä keskellä
    std::vector<jousi> tuntoJouset;
    
    void luoJouset(int layers = 1);
       
public:

    //Hytkyjen staattiset eli globaalit ominaisuudet:
    static vec2 gravity;
    static int physicsIterations;
    static rectangle huone; //hytky on huoneessa
    static const float halkaisija; //miten iso hytky on fysikaalisessa tilassa
    
    bool extending = true; //pullistetaanko vai rutistetaanko
    
    //Hytkykohtaiset ominaisuudet:
    float hytkynSivu; //montako partikkelia per sivu; hytky on kuusikulmainen
    int springLayers; //miten monta kerrosta jousia on -- vaikuttaa aineen lujuutta lisäävästi.
    
    circle repulsor;
    
    hytky(int sivu = 4, int springLayers_ = 1); //sivu: montako pistettä per sivu, kun hytky on kuusikulmainen

    vec2 at(int i); //hae pisteen sijainti indeksin mukaan
    int size(); //pisteiden määrä hytkyssä
    
    int joustenMaara(int layer = 0);
    
    vec2 haeJousenAlku(int i, int layer=0);
    vec2 haeJousenLoppu(int i, int layer=0);
    
    void asetaJousivakio(float new_k);
      
    void step(); //laske voimat ja liikuta pisteitä
    std::string kerro(); //palauttaa tietoja
    void extend(float x,float y, float force);
    void handleMessages(ofxOscMessage msg);
    
    
};


class hytky::piste{ //hytkyn partikkeli
public:
    
    //pisteiden staattiset eli globaalit ominaisuudet
    static float m;
    static float kitka;
    static float max_v;   
    static float paine_r; //säde jota sovelletaan sisäisen paineen laskemiseen
    static float paine_m; //sisäisen paineen magnitudi
    
    //pistekohtaiset ominaisuudet
    vec2 paikka;
    vec2 F; //pisteeseen vaikuttava hetkellinen kokonaisvoima
    vec2 v; //pisteen nopeus
  
    piste(float x = 0, float y = 0);
    
    vec2 laskeVoimat();
    void laskePaine(const vec2& p); //laske toisen pisteen pisteeseen aiheuttama sisäisen paineen voima
    void liikuta();
};


class hytky::jousi{
private:
public:
    piste* pisteet[2];

    jousi();
    jousi(piste& alku, piste& loppu, float k = 1);
    
    float jousivakio;
    float lepopituus;
    float ekstensio;
    
    float getStress();    
    void extend(float amount = 0.05);
    void relax(float amount = 0.01);
    
    void laskeVoimat(); // vaikuttaa pisteisiin
};