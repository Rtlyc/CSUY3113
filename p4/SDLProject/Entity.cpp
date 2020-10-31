#include "Entity.h"
#include <iostream>

Entity::Entity()
{
    position = glm::vec3(0);
    speed = 0;
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0); 
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other){
    float xdist = fabs(position.x - other->position.x) - fabs((other->width+width)/2.0f);
    float ydist = fabs(position.y - other->position.y) - fabs((height+other->height)/2.0f);
    
    if(xdist < 0 && ydist < 0){
        return true;
    }
    
    return false;
}

bool Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object)){
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
            }else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedType = object->type;
                test = i;
                onGround = true;
            }
            return true;
        }
    }
    return false;
}

bool Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedType = object->type;
                collidedType = PLAYER;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedType = object->type;
                collidedType = PLAYER;
            }
            return true;
        }
    }
    return false;
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Jump(){
    if(onGround){
        velocity.y += 6.0f;
        onGround = false;
    }
}

void Entity::Shoot(){
    
}

void Entity::AIIdle(Entity* player, AIState state){
    movement = glm::vec3(0);
    if(glm::distance(player->position,position) < 1.5)
        aistate = state;
}

void Entity::AIWalk(Entity* player, AIState state){
    if(glm::distance(player->position,position) < 3){
        if(player->position.x < position.x)
            movement = glm::vec3(-1,0,0);
        else
            movement = glm::vec3(1,0,0);
    }else
        aistate = state;
}

void Entity::AIJump(Entity* player, AIState state){
    if(glm::distance(player->position,position) < 2){
        Jump();
    }else
        aistate = state;
}

void Entity::AIShoot(Entity* player, std::vector<Entity*>& bullets, AIState state){
    if(colddown <= 0){
        Entity* bullet = new Entity();
        bullets.push_back(bullet);
        bullet->position = position;
        bullet->type = ENEMYBULLET;
        bullet->height = 0.5f;
        bullet->width = 0.5f;
        if(player->position.x < position.x){
            bullet->velocity = glm::vec3(-2.0,0,0);
        }else{
            bullet->velocity = glm::vec3(2.0,0,0);
        }
        colddown = shootCD;
    }else{
        aistate = state;
    }
}

void Entity::AIDead(){
    type = MYBULLET;
}

void Entity::AI(Entity* player, std::vector<Entity*>& bullets){
    switch (aitype) {
        case AIJUMPER:
            switch (aistate) {
                case AIIDLE:
                    AIIdle(player,AIJUMP);
                    break;
                    
                case AIDEAD:
                    AIDead();
                    break;
                    
                default:
                    AIJump(player,AIIDLE);
                    break;
            }
            break;
            
        case AIWALKER:
            switch (aistate) {
                case AIIDLE:
                    AIIdle(player,AIWALK);
                    break;
                case AIWALK:
                    AIWalk(player, AIIDLE);
                    break;
                case AIDEAD:
                    AIDead();
                    break;
                default:
                    break;
            }
            break;
            
        case AISHOOTER:
            switch (aistate) {
                case AIIDLE:
                    AIIdle(player,AISHOOT);
                    break;
                case AISHOOT:
                    AIShoot(player,bullets,AIWALK);
                    break;
                case AIWALK:
                    if(colddown < 0) aistate = AISHOOT;
                    else AIWalk(player,AIIDLE);
                    break;
                case AIDEAD:
                    AIDead();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


void Entity::Update(float deltaTime, Entity* player, Entity* platforms, Entity* enemies, std::vector<Entity*>& mybullets, std::vector<Entity*>& enemybullets, int platformCount, int enemyCount)
{
    collidedType = PLAYER;
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    if(type == PLAYER)
        velocity.x = movement.x * speed;
    else if(type == ENEMY){
        velocity.x = movement.x * speed;
        AI(player,enemybullets);
    }
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime; // Move on Y
    if(type==PLAYER || type==ENEMY)
        CheckCollisionsY(platforms, platformCount);// Fix if needed
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    
    if(type==ENEMY){
        for (auto each : mybullets) {
            if(CheckCollision(each)){
                aistate = AIDEAD;
                return;
            }
        }
    }else if(type == PLAYER){
        for (int i=0; i<enemyCount; i++){
            if(CheckCollision(&enemies[i])){
                player->gamestate=FAIL;
                return;
            }
        }
        for(auto each:enemybullets){
            if(CheckCollision(each)){
                player->gamestate=FAIL;
                return;
            }
        }
    }
    
    
    
//    for (int i = 0; i < platformCount; i++)
//    {
//        Entity *platform = &platforms[i];
//        if (CheckCollision(platform)){
//            float ydist = fabs(position.y - platform->position.y);
//            float penetrationY = fabs(ydist - (height / 2.0f) - (platform->height / 2.0f));
//            if (velocity.y > 0) {
//                position.y -= penetrationY;
//                velocity.y = 0;
//            } else if (velocity.y < 0) {
//                position.y += penetrationY;
//                velocity.y = 0;
//            }
//        }
//    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}
