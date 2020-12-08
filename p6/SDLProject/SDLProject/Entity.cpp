#include "Entity.h"

bool Entity::CheckCollision(Entity* other){
    float xdist = fabs(x - other->x) - fabs((other->size+size)/2.0f);
    float ydist = fabs(y - other->y) - fabs((size+other->size)/2.0f);
    
    
    if(xdist < 0 && ydist < 0){
        return true;
    }
    
    return false;
}
