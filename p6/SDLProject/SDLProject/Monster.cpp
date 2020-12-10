#include "Monster.h"
#include "Bullet.h"
#include <stdlib.h>
#include <time.h>


Monster::Monster(float x, float speed, float size, float maxhealth): Entity(size),speed(speed),health(maxhealth),cur_health(maxhealth){
    this->x = x;
    srand((unsigned)time(NULL));
    y = rand()-0.5f;
}

Monster::Monster(float x, float y, float size, float speed, float maxhealth): Entity(x,y,size), speed(speed),health(maxhealth),cur_health(maxhealth){}

void Monster::Destroy(std::vector<Monster*>& monsters){
    for (size_t i=0; i<monsters.size(); i++) {
        if(monsters[i]==this){
            monsters[i] = monsters[monsters.size()-1];
            monsters[monsters.size()-1] = nullptr;
            monsters.pop_back();
            delete this;
            return;
        }
    }
}

void Monster::Update(float delta_time, std::vector<Monster*>& monsters, std::vector<Bullet*>& bullets, int& player_health, int& money){
    x += speed*delta_time;
    int ind = 0;
    while(ind<bullets.size()){
        if(bullets[ind] && CheckCollision(bullets[ind])){
            cur_health-=bullets[ind]->damage;
            bullets[ind]->Destroy(bullets);
        }else ind++;
    }
    
    if(cur_health<=0){
        money+=3;
        Destroy(monsters);
    }
    else if(x>5.5){
        player_health--;
        //lose health
        Destroy(monsters);
    }
}


void Monster::Render(ShaderProgram* program){
    glm::mat4 mv = glm::mat4(1);
    mv = glm::translate(mv, glm::vec3(x,y,0));
    program->SetModelMatrix(mv);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, drawArray);
    glEnableVertexAttribArray(program->positionAttribute);
    program->SetColor(1-cur_health/health, cur_health/health, 0, 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
}
