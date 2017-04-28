#include <iostream>
#include <stdlib.h>
#include "hytky.h"


void moveFloatTowards(float& value, float target, float amount) {
    if(value - amount > target) value -= amount;
    else if (value > target) value = target;
    else if(value + amount < target) value += amount;
    else if(value < target) value = target;
}


void bound(float& value, float min, float max) {
    if(value < min) value = min;
    else if (value > max) value = max;
}


bool rectangle::onkoSisalla(const vec2& p) {
    if(p.x < x) return false;
    if(p.y < y) return false;
    if(p.x > x + w) return false;
    if(p.y > y + h) return false;
    return true;
}


vec2 rectangle::vektoriUlkoaSisaan(const vec2& p) {
    vec2 result(0,0);
    if(p.x < x) 
        result.x = x - p.x;
    else if (p.x > x + w)        
        result.x = x + w - p.x;    
    
    if(p.y < y) 
        result.y = y - p.y;
    else if(p.y > y + h)
        result.y = y + h - p.y;
    
    return result;

}


bool circle::onkoSisalla(const vec2& p) {
    if((p - keskipiste).length() < r)
        return true;
    else return false;
}


vec2 circle::vektoriSisaltaReunalle(const vec2& p) {
    vec2 ero = p - keskipiste;
    float l = ero.length();
    if(l < r) {
        return ero.getScaled(r-l);
    }
    else return vec2(0,0);
}


//hytkyjen globaalit ominaisuudet:
vec2    hytky::gravity      = vec2(0, 0.2);
int     hytky::physicsIterations = 8;
rectangle hytky::huone      = rectangle(-200, -150, 400, 300);
const float hytky::halkaisija   = 150;


void hytky::luoJouset(int layers) {
        
    if(layers > 4) layers = 4;
    if(layers < 1) layers = 1;
        
    float halkaisijaPisteina = 2 * hytkynSivu - 1;
    //lähimpien pisteiden välimatka fysikaalisessa avaruudessa
    float pistevali = halkaisija / halkaisijaPisteina;
    
    float margin = 0.05;
    float initial_k = 1;
    
    float r[4];
    r[0] = pistevali;
    r[1] = pistevali * sqrt(3);
    r[2] = pistevali * 2;
    r[3] = pistevali * sqrt(7);
    
    for(int l = 0; l < layers; l++) {
            
        std::vector<jousi> uusiLayer;
        
        float r_min = r[l] - margin;
        float r_max = r[l] + margin;
        
        //std::cout << r_min << ", " << r_max << "\n";
        
        for(int i=0; i< pisteet.size(); i++) {
            for(int j=0; j<pisteet.size(); j++) {
                if(i==j) continue;

                //jos pisteet ovat tarpeeksi lähellä, lisätään niiden välille jousi
                float ero = (pisteet[j].paikka - pisteet[i].paikka).length();
                if(ero < r_max && ero > r_min )  {
                    
                    //tarkistetaan onko jo olemassa sellainen jousi, jonka päinä nämä pisteet ovat
                    bool found = false;
                    for(int jousi_i = 0; jousi_i < uusiLayer.size() && !found; jousi_i++) {
                        if(uusiLayer[jousi_i].pisteet[0] == &(pisteet[i]) && uusiLayer[jousi_i].pisteet[1] == &(pisteet[j]) )
                            found = true;
                        else if(uusiLayer[jousi_i].pisteet[1] == &(pisteet[i]) && uusiLayer[jousi_i].pisteet[0] == &(pisteet[j]) )
                            found = true;
                    }
                    //jos jousta ei ollut, luodaan se
                    if(!found) {
                        uusiLayer.push_back(jousi(pisteet[i], pisteet[j], initial_k / (l+1) ) ); //TODO: jousivakiosta (initial_k) säädettävä parametri
                    }
                }
            }
            
        }
        jouset.push_back(uusiLayer);
        std::cout << "Luotiin kerros " << l << ", " << uusiLayer.size() << " jousta\n";
    }
    
    //luodaan tuntojouset:
    
    //etsitään keskipiste
    float min_d2 = pisteet[0].paikka.lengthSquared();
    int center_i = 0;    
    for(int i=0; i<pisteet.size(); i++) {
        float d = pisteet[i].paikka.lengthSquared();
        if(d < min_d2) {
            min_d2 = d;
            center_i = i;
        }
    }
    
    std::vector<int> inner;
    std::vector<int> outer;
    //std::vector<piste*> branches;
    
    //luodaan jouset keskipisteestä kulmiin kahdessa osassa
    
    float s = (float)(hytkynSivu-1) * pistevali;
    float inner_r = s * (3.0/6);
    float outer_r = s;
    float branch_r = s * (1.0/6);
    
    margin = pistevali * 0.1;
    
    //jaetaan pisteet väliaikaisiin vektoreihin
    for(int j=0; j<pisteet.size(); j++) {
        if(center_i==j) continue;

        float ero = (pisteet[j].paikka - pisteet[center_i].paikka).length();
         
        if(ero < outer_r + margin && ero > outer_r - margin)
             outer.push_back(j);

        if(ero < inner_r + margin && ero > inner_r - margin)
             inner.push_back(j);         
    }
    
    //tehdään jouset pisteiden mukaan
    //keskeltä -> inner
    //inner -> outer
    for(int i=0; i<inner.size(); i++) 
        tuntoJouset.push_back(jousi(pisteet[center_i], pisteet[inner[i] ], 0.01) );
    
    for(int i=0; i < inner.size() && outer.size() >= inner.size(); i++)
        tuntoJouset.push_back(jousi(pisteet[inner[i] ], pisteet[outer[i] ], 0.01) );
    
    //'keskijouset' pitävät koko hytkyä ruudun keskellä
    //for(int i= 0; i<pisteet.size(); i++)
    //    keskiJouset.push_back(jousi(pisteet[i], initialPoints[i], initial_k * 0.05));
    

}


hytky::hytky(int sivu, int springLayers_) : hytkynSivu(sivu), springLayers(springLayers_) {
    
    repulsor = circle(0, 0, 100);
    
    if( (int)hytkynSivu % 2 == 0) hytkynSivu++;    
    if(hytkynSivu < 0) hytkynSivu = 0;
    
    int halkaisijaPisteina = 2 * hytkynSivu - 1;
    float pistevali = halkaisija / halkaisijaPisteina;
    hytky::piste::paine_r = halkaisija * 0.2; //TODO: tämä kerroin muuttujaksi!    
           
    //luodaan pisteet:    
    int rivinPituus = hytkynSivu;
    float pystyvali = sqrt(3)/2;
    
    for(int rivi = 0; rivi < hytkynSivu; rivi++) {
        for(int i=0; i<rivinPituus; i++) {
            float x = pistevali * ( (rivi * -0.5) + i) - (0.5 * hytkynSivu * pistevali);
            float y = pistevali * rivi * pystyvali - ((hytkynSivu-1) * pistevali * pystyvali);
            
            pisteet.push_back(piste(x, y));
        }
        rivinPituus ++;
    }
    
    rivinPituus--;
    
    for(int rivi = hytkynSivu; rivi < halkaisijaPisteina; rivi++) { //rivien määrä on sama kuin halkaisija
        rivinPituus --;
        for(int i=0; i<rivinPituus; i++) {
            float x = pistevali * ( (rivi * 0.5) + i-hytkynSivu+1) - (0.5 * hytkynSivu * pistevali);
            float y = pistevali * rivi * pystyvali - ((hytkynSivu-1) * pistevali * pystyvali);
            
            pisteet.push_back(piste(x, y));
        }
    }

    initialPoints = pisteet;
    
    
    //luodaan jouset
    luoJouset(springLayers);

}


vec2 hytky::at(int i) {
    return pisteet[i].paikka;
}


int hytky::size() {
    return pisteet.size();
}


int hytky::joustenMaara(int layer) {
    if(layer >= springLayers) return tuntoJouset.size();
    else return jouset[layer].size();
}
    
vec2 hytky::haeJousenAlku(int i, int layer){
    if(layer >= springLayers) return tuntoJouset[i].pisteet[0]->paikka;
    else return jouset[layer][i].pisteet[0]->paikka;
}

vec2 hytky::haeJousenLoppu(int i, int layer){
    if(layer >= springLayers) return tuntoJouset[i].pisteet[1]->paikka;
    else return jouset[layer][i].pisteet[1]->paikka;
}


void hytky::asetaJousivakio(float new_k) {

    float k_per_l = new_k / (float)jouset.size();
    
    for(int layer = 0; layer < jouset.size(); layer++) {
        for(int i=0; i<jouset[layer].size(); i++) {
            
            float k = k_per_l * (jouset.size() - layer);
            //float k = new_k;
            jouset[layer][i].jousivakio = k;
            //std::cout << "set stiffness " << layer << " : " << k << "\n";
        }
    }
}



void hytky::step() {
    
    //joka askeleella tehdään oikeasti monta askelta eli iteraatiota:    
    for(int it = 0; it < physicsIterations; it++) {

        //Lasketaan kaikki pisteisiin vaikuttavat voimat
        for(unsigned int i = 0; i < pisteet.size(); i++) {
            pisteet[i].F = vec2(0,0);
            pisteet[i].laskeVoimat();
            
            //repulsio
            //pisteet[i].F += (repulsor.vektoriSisaltaReunalle(pisteet[i].paikka) * piste::m / physicsIterations).getSquared();
            
            for(unsigned int j = 0; j < pisteet.size(); j++) {
                if(i!=j)  pisteet[i].laskePaine(pisteet[j].paikka);
            }
        }
        
        for(unsigned int i = 0; i < jouset.size(); i++) {
            for(unsigned int j = 0; j < jouset[i].size(); j++) {
                
                //tehdään "repulsorilla" eli hiirellä ohjattavalla ympyrällä ekstensio
                //ekstension määrä riippuu siitä onko jousen alku- ja loppupää repulsorin sisällä
                float repulsion = 0;
                if(repulsor.onkoSisalla(haeJousenAlku(j, i) ) )
                    repulsion += repulsor.vektoriSisaltaReunalle(haeJousenAlku(j, i)).lengthSquared() / (repulsor.r*repulsor.r);
                if(repulsor.onkoSisalla(haeJousenLoppu(j, i) ) )
                    repulsion += repulsor.vektoriSisaltaReunalle(haeJousenLoppu(j, i)).lengthSquared() / (repulsor.r*repulsor.r);
                repulsion /= 2;
                repulsion *= 0.02;
                
                //jos extending==true pullistetaan, jos false, supistetaan
                if(extending == false) repulsion *= -1;
                
                jouset[i][j].extend(repulsion);

                //rentoutetaan kaikkia jousia
                jouset[i][j].relax(0.005);
                
                //lasketaan venymät ja voimat niiden mukaan
                jouset[i][j].laskeVoimat();
            }
        }
       
        for(unsigned int i=0; i<keskiJouset.size(); i++)
            keskiJouset[i].laskeVoimat();        
        
        
        //liikutetaan kaikkia pisteitä voimien perusteella
        for(unsigned int i = 0; i < pisteet.size(); i++)
            pisteet[i].liikuta();
    }
}


std::string hytky::kerro() {
    std::string result = "/body/sense/stress ";
    
    for(int i=0; i < tuntoJouset.size(); i++) {
        result = result + std::to_string(tuntoJouset[i].getStress() * 10) + " ";
    }
    
    return result;
}


//Pisteiden globaalit ominaisuudet
float hytky::piste::m = 10; // partikkelien massa
float hytky::piste::kitka = 0.9; // kitkakerroin
float hytky::piste::max_v = 100; // partikkelien maksiminopeus
float hytky::piste::paine_r;    // sisäisen paineen vaikutussäde
float hytky::piste::paine_m = 0.4; //sisäisen paineen magnitudi


hytky::piste::piste(float x, float y) : paikka(x, y), v(0,0) {}


vec2 hytky::piste::laskeVoimat() {
    //pehmeät reunat
    F += hytky::huone.vektoriUlkoaSisaan(paikka) * piste::m;
    return F;
}


void hytky::piste::laskePaine(const vec2& p) {
    vec2 d = paikka - p;
    float l = d.length();
    
    if(l < piste::paine_r)
        F += d.getScaled(piste::paine_r - l) * piste::paine_m;
}


void hytky::piste::liikuta() {
    vec2 a = F / hytky::piste::m; // hetkellinen kiihtyvyys a = F/m
    v += a / hytky::physicsIterations;
    
    v += gravity / hytky::physicsIterations;
    v *= pow(hytky::piste::kitka, (float)1 / (float)hytky::physicsIterations); // globaali kitkakerroin 0...1
    
    if(v.length() > hytky::piste::max_v) 
        v = v.getScaled(hytky::piste::max_v); // absoluuttinen maksiminopeus
    
    paikka += v / hytky::physicsIterations;
}


hytky::jousi::jousi() {
    pisteet[0] = NULL;
    pisteet[1] = NULL;
    jousivakio = 1;
    lepopituus = 0;
    ekstensio = 1;
} 


hytky::jousi::jousi(piste& A, piste& B, float k) {
    pisteet[0] = &A;
    pisteet[1] = &B;
    jousivakio = k;
    ekstensio = 1;
    
    if(pisteet[0] != NULL && pisteet[1] != NULL)
        lepopituus = (pisteet[0]->paikka - pisteet[1]->paikka).length();
    else 
        lepopituus = 0;
}


void hytky::jousi::laskeVoimat() {
    
    if(pisteet[0] != NULL && pisteet[1] != NULL) {
        vec2 ero = pisteet[1]->paikka - pisteet[0]->paikka;
        vec2 venyma = ero.getScaled( (lepopituus * ekstensio) - ero.length() );
        
        vec2 F = venyma.getSquared() * jousivakio;
        
        pisteet[1]->F += F;
        pisteet[0]->F -= F;    
    }
}


float hytky::jousi::getStress() {
    vec2 ero = pisteet[1]->paikka - pisteet[0]->paikka;
    vec2 venyma = ero.getScaled(lepopituus - ero.length() );
    vec2 stress = venyma * jousivakio;
    
    return stress.length();
}


void hytky::jousi::extend(float amount) {
    ekstensio += amount;
    if(ekstensio > 2) ekstensio = 2;
    if(ekstensio < 0.5) ekstensio = 0.5;
}


void hytky::jousi::relax(float amount) {
    if(ekstensio > 1-amount) ekstensio -= amount;
    else if (ekstensio < 1+amount) ekstensio += amount;
    else ekstensio = 1;
}


void hytky::handleMessages(ofxOscMessage msg) {

    //yhteysrutiinit
    olentoClient::handleMessages(msg);
    
    std::vector<std::string> address = ofSplitString(msg.getAddress(),"/",true,false);
    
    //paketin muoto: /*/expression/extend <x> <y> <force>
    if(address.size() > 2 && address[1] == "expression") {
        if(address[2] == "extend") {
            float x = msg.getArgAsFloat(0);
            float y = msg.getArgAsFloat(1);
            float f = msg.getArgAsFloat(2);
            extend(x,y,f);
        }
    }
}

void hytky::extend(float x, float y, float force) {
    repulsor.keskipiste = vec2(  pisteet[18].paikka.x + x*80,pisteet[18].paikka.y + y*80);
    repulsor.r = force*80;    
}
