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
vec2    hytky::gravity      = vec2(0,0);
int     hytky::physicsIterations = 10;
rectangle hytky::huone      = rectangle(-200, -150, 400, 300);
const float hytky::halkaisija   = 150;



hytky::hytky(int sivu, float lujuus) : hytkynSivu(sivu) {
    
    repulsor = circle(0,0,50);
    
    if(hytkynSivu < 0) hytkynSivu = 0;
    
    int halkaisijaPisteina = 2 * hytkynSivu - 1;
    float pistevali = halkaisija / halkaisijaPisteina;    
    hytky::piste::paine_r = halkaisija * 0.2; //TODO: tämä kerroin muuttujaksi!
    springLayers = halkaisija * lujuus; //lujuus 1 tarkoittaa että jouset yltävät läpi kappaleen
    
    float initial_k = 2.5;
    
    if(springLayers < pistevali + 0.01) springLayers = pistevali + 0.01; //jousten pitää yltää vähintään viereisiin pisteisiin
    
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
    for(int i=0; i< pisteet.size(); i++) {
        for(int j=0; j<pisteet.size(); j++) {
            if(i==j) continue;
            
            //jos pisteet ovat tarpeeksi lähellä, lisätään niiden välille jousi
            if( (pisteet[j].paikka - pisteet[i].paikka).length() < springLayers ) {
                //tarkistetaan onko jo olemassa sellainen jousi, jonka päinä nämä pisteet ovat
                bool found = false;
                for(int jousi_i = 0; jousi_i < jouset.size() && !found; jousi_i++) {
                    if(jouset[jousi_i].pisteet[0] == &(pisteet[i]) && jouset[jousi_i].pisteet[1] == &(pisteet[j]) )
                        found = true;
                    else if(jouset[jousi_i].pisteet[1] == &(pisteet[i]) && jouset[jousi_i].pisteet[0] == &(pisteet[j]) )
                        found = true;
                }
                //jos jousta ei ollut, luodaan se
                if(!found) {
                    jouset.push_back(jousi(pisteet[i], pisteet[j], initial_k) ); //TODO: jousivakiosta (initial_k) säädettävä parametri
                    std::cout << "luotiin jousi " << jouset.size() << "\n";
                }
            }
        }
        keskiJouset.push_back(jousi(pisteet[i], initialPoints[i], initial_k * 0.05));
    }

    std::cout << "Luotiin " << pisteet.size() << " pistettä, " << jouset.size() << " jousta\n";
}


vec2 hytky::at(int i) {
    return pisteet[i].paikka;
}


int hytky::size() {
    return pisteet.size();
}


void hytky::step() {
    
    //joka askeleella tehdään oikeasti monta askelta eli iteraatiota:    
    for(int it = 0; it < physicsIterations; it++) {   

        //Lasketaan kaikki pisteisiin vaikuttavat voimat
        for(unsigned int i = 0; i < pisteet.size(); i++) {
            pisteet[i].F = vec2(0,0);
            pisteet[i].laskeVoimat();
            
            //repulsio
            pisteet[i].F += (repulsor.vektoriSisaltaReunalle(pisteet[i].paikka) * piste::m / physicsIterations).getSquared();

            for(unsigned int j = 0; j < pisteet.size(); j++) {
                if(i!=j)  pisteet[i].laskePaine(pisteet[j].paikka);
            }
        }
        
        for(unsigned int i = 0; i < jouset.size(); i++ )
            jouset[i].laskeVoimat();
       
        for(unsigned int i=0; i<keskiJouset.size(); i++)
            keskiJouset[i].laskeVoimat();
        
        
        //liikutetaan kaikkia pisteitä voimien perusteella
        for(unsigned int i = 0; i < pisteet.size(); i++)
            pisteet[i].liikuta();
    }
}


std::string hytky::kerro() {
    static float viime_keskiarvo; //optimoidaan käyttämällä edellistä keskiarvoa varianssin laskemiseen
    static float viimeArvot[100]; //keskiarvot viime 50 askeleelta
    static int laskuri = 0;

    float keskiarvo = 0;
    float varianssi = 0;
    
    for(int i=0; i<jouset.size(); i++) {
        float stress = jouset[i].getStress();
        keskiarvo += stress;
        varianssi += pow(stress - viime_keskiarvo, 2);
    }
    keskiarvo = keskiarvo / jouset.size();
    varianssi = varianssi / jouset.size();
    viime_keskiarvo = keskiarvo;
    
    viimeArvot[laskuri] = keskiarvo;
    laskuri++;
    if(laskuri > 99) laskuri = 0;
    
    //lasketaan viime arvojen erojen summa tämänhetkisestä keskiarvosta eli värähtely
    float varahtely = 0;
    for(int i=0; i<100; i++) {
        varahtely += pow(viimeArvot[i] - viime_keskiarvo, 2);
    }
    varahtely = varahtely / 100;
    
    return std::to_string(keskiarvo) + ", " + std::to_string(varianssi) + ", " + std::to_string(varahtely);
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
} 


hytky::jousi::jousi(piste& A, piste& B, float k) {
    pisteet[0] = &A;
    pisteet[1] = &B;
    jousivakio = k;
    if(pisteet[0] != NULL && pisteet[1] != NULL)
        lepopituus = (pisteet[0]->paikka - pisteet[1]->paikka).length();
    else 
        lepopituus = 0;
}


void hytky::jousi::laskeVoimat() {
    
    static float damageFactor = 3;
    
    if(pisteet[0] != NULL && pisteet[1] != NULL) {
        vec2 ero = pisteet[1]->paikka - pisteet[0]->paikka;
        vec2 venyma = ero.getScaled(lepopituus - ero.length() );
        
        vec2 F = venyma.getSquared() * jousivakio;
        
        pisteet[1]->F += F;
        pisteet[0]->F -= F;    
        //kuluminen:
        /*
        if(venyma.length() > lepopituus * damageFactor) {
            lepopituus += venyma.length() * 0.01;
            jousivakio *= 0.975;
        }
         */
    }
}


float hytky::jousi::getStress() {
    vec2 ero = pisteet[1]->paikka - pisteet[0]->paikka;
    vec2 venyma = ero.getScaled(lepopituus - ero.length() );
    vec2 stress = venyma * jousivakio;
    
    return stress.length();
}