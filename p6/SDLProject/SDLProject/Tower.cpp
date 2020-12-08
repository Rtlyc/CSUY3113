#include "Tower.h"
#include "Bullet.h"
#include "Monster.h"
#include "Util.h"


Tower::Tower(int index, float size, float range, float damage, float cool_down, Map* map):Entity(size),index(index),timer(0),range(range),damage(damage),cool_down(cool_down){
    x = map->grids[index]->x;
    y = map->grids[index]->y;
    textureID = Util::LoadTexture("angrybird.png");
    bounce = Mix_LoadWAV("Shoot.wav");
}

void Tower::Update(float delta_time, std::vector<Monster*>& monsters, std::vector<Bullet*>& bullets){
    timer += delta_time;
    if(timer>=cool_down){
        for (int i=0; i<monsters.size(); i++) {
            float distance= glm::distance(glm::vec2(x,y),glm::vec2(monsters[i]->x,monsters[i]->y));
            if(distance<range){
                glm::vec2 direction = glm::normalize(glm::vec2(monsters[i]->x,monsters[i]->y)-glm::vec2(x,y));
//                float x, float y, float size, glm::vec2 direction, float speed, float damage, Tower* parent
                Bullet* newbullet = new Bullet(x,y,0.2,direction,0.5,damage,this);
                bullets.push_back(newbullet);
                timer = 0;
                Mix_PlayChannel(-1,bounce,0);
                break;
            }
        }
    }
}

void Tower::Render(ShaderProgram* program){
    glm::mat4 mv = glm::mat4(1);
    mv = glm::translate(mv, glm::vec3(x,y,0));
    program->SetModelMatrix(mv);
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
