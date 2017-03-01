#pragma once

#include <vector>
#include <vector2.h>
#include <string>


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


class hytky {
private:

    class piste;  //hytkyn partikkeli
    class jousi;  //pisteiden välillä oleva jousi
    
    std::vector<piste> pisteet; //kaikki hytkyn partikkelit
    std::vector<piste> initialPoints;
    
    std::vector<jousi> jouset; //kaikki pisteiden välillä olevat jouset
    std::vector<jousi> keskiJouset; //jouset jotka pitävät hytkyä paikoillaan
       
public:

    //Hytkyjen staattiset eli globaalit ominaisuudet:
    static vec2 gravity;
    static int physicsIterations;
    static rectangle huone; //hytky on huoneessa
    static const float halkaisija; //miten iso hytky on fysikaalisessa tilassa
    
    //Hytkykohtaiset ominaisuudet:
    float hytkynSivu; //montako partikkelia per sivu; hytky on kuusikulmainen
    float springLayers; //miten sikinsokin pisteiden väliset jouset yhdistyvät -- vaikuttaa aineen lujuutta lisäävästi.
    
    circle repulsor;
    
    hytky(int sivu = 4, float lujuus = 0); //sivu: montako pistettä per sivu, kun hytky on kuusikulmainen

    vec2 at(int i); //hae pisteen sijainti indeksin mukaan
    int size(); //pisteiden määrä hytkyssä
      
    void step(); //laske voimat ja liikuta pisteitä
    std::string kerro(); //palauttaa tietoja
 
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
    
    float getStress();    
    
    void laskeVoimat(); // vaikuttaa pisteisiin
};