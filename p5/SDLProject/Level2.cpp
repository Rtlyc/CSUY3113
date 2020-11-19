#include "Level2.h"
#include <vector>
#define LEVEL2_WIDTH 53
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMY_COUNT 2

unsigned int level2_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


void Level2::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    {
        state.door = new Entity();
        state.door->textureID = Util::LoadTexture("platformPack_tile048.png");
        state.door->type = DOOR;
        state.door->position = glm::vec3(10,-3,0);
        state.door->Update(0, NULL, state.map, NULL, NULL, NULL);
    }
     
 #pragma region Initialize Player
     {
         // Initialize Player
         state.player = new Entity();
         state.player->type = PLAYER;
         state.player->gamestate = RUNNING;
         state.player->position = glm::vec3(5,0,0);
         state.player->movement = glm::vec3(0);
         state.player->speed = 3.0f;
         state.player->textureID = Util::LoadTexture("george_0.png");
         
         state.player->animRight = new int[4] {3, 7, 11, 15};
         state.player->animLeft = new int[4] {1, 5, 9, 13};
         state.player->animUp = new int[4] {2, 6, 10, 14};
         state.player->animDown = new int[4] {0, 4, 8, 12};

         state.player->animIndices = state.player->animRight;
         state.player->animFrames = 4;
         state.player->animIndex = 0;
         state.player->animTime = 0;
         state.player->animCols = 4;
         state.player->animRows = 4;
         
         state.player->acceleration = glm::vec3(0,-8.0f,0);
         state.player->height = 0.8f;
         state.player->width = 0.65f;
     }
 #pragma endregion
     
     
 #pragma region Initialize Enemies
     {
         state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
         for (int i=0; i<LEVEL2_ENEMY_COUNT; i++) {
             state.enemies[i].textureID = Util::LoadTexture("ctg.png");
             state.enemies[i].type = ENEMY;
             state.enemies[i].acceleration = glm::vec3(0,-8.0f,0);
             state.enemies[i].speed = 1;
             state.enemies[i].height = 1.0f;
             state.enemies[i].width = 0.5f;
         }

         state.enemies[0].position = glm::vec3(1.5,3,0);
         state.enemies[0].aitype = AIJUMPER;
         state.enemies[0].aistate = AIIDLE;

         state.enemies[1].position = glm::vec3(12,3,0);
         state.enemies[1].aitype = AIWALKER;
         state.enemies[1].aistate = AIIDLE;

     }
 #pragma endregion

}
void Level2::Update(float deltaTime) {
    if(state.player->gamestate == FAIL){
        Entity::life--;
        state.nextScene = 2;
        return;
    }else if(state.player->gamestate == SUCCESS){
        state.nextScene = 3;
        return;
    }
    state.player->Update(deltaTime, state.player, state.map, state.enemies, LEVEL2_ENEMY_COUNT, state.door);
    for (int i=0; i<LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime,state.player,state.map, NULL, 0, NULL);
    }
}
void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i=0; i<LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    state.door->Render(program);
}

void Level2::Jump(){
    state.player->Jump();
}


