#include "vector2.h"


vec2 getMedian(const std::vector<vec2>& pisteet) {
    vec2 result(0,0);
    int n=0;
    
    for(int i=0; i<pisteet.size(); i++) {
        result += pisteet[i];
        n++;
    }
    if(n!=0) result /= n;    
    return result;    
}