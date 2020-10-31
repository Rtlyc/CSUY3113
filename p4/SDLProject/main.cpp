#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define PLATFORM_NUM 12
#define ENEMIES_NUM 3

struct GameState {
    Entity *player;
    Entity *platforms;
    std::vector<Entity*> bullets;
    std::vector<Entity*> enemybullets;
    Entity *enemies;
};

GameState state;
bool gameover = false;
bool gamewin = false;

SDL_Window* displayWindow;
bool gameIsRunning = true;

float shootCD = 0.5f;
float shootTime = 0;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
GLuint FontTextureID, BulletTextureID, EnemyTextureID, EnemyBulletTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project4!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640*2, 480*2);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    FontTextureID = LoadTexture("font1.png");
    BulletTextureID = LoadTexture("ufoYellow.png");
    EnemyBulletTextureID = LoadTexture("ufoRed.png");
    EnemyTextureID = LoadTexture("ctg.png");
    
#pragma region Initialize Player
    {
        // Initialize Player
        state.player = new Entity();
        state.player->type = PLAYER;
        state.player->gamestate = RUNNING;
        state.player->position = glm::vec3(0);
        state.player->movement = glm::vec3(0);
        state.player->speed = 3.0f;
        state.player->textureID = LoadTexture("george_0.png");
        
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
    
#pragma region Initialize Platform
    {
        state.platforms = new Entity[PLATFORM_NUM];
        GLuint platformTextureID = LoadTexture("platformPack_tile003.png");
        for(int i=0; i<PLATFORM_NUM; i++){
            state.platforms[i].textureID = platformTextureID;
            state.platforms[i].position = glm::vec3(-5+i, -3.25f, 0);
            state.platforms[i].type = BOX;
        }
        state.platforms[PLATFORM_NUM-2].position = glm::vec3(0, -1.25, 0);
        state.platforms[PLATFORM_NUM-1].position = glm::vec3(1, -1.25, 0);
        
        
        for (int i=0; i<PLATFORM_NUM; i++) {
            state.platforms[i].Update(0,state.player,NULL,NULL,state.bullets,state.enemybullets,0,0);
        }
    }
#pragma endregion
    
#pragma region Initialize Enemies
    {
        state.enemies = new Entity[ENEMIES_NUM];
        for (int i=0; i<ENEMIES_NUM; i++) {
            state.enemies[i].textureID = EnemyTextureID;
            state.enemies[i].position = glm::vec3(-3,0,0);
            state.enemies[i].type = ENEMY;
            state.enemies[i].acceleration = glm::vec3(0,-8.0f,0);
            state.enemies[i].speed = 1;
            state.enemies[i].height = 1.0f;
            state.enemies[i].width = 0.5f;
        }
        state.enemies[0].aitype = AISHOOTER;
        state.enemies[0].aistate = AIIDLE;
        
        state.enemies[1].position = glm::vec3(1.5,3,0);
        state.enemies[1].aitype = AIJUMPER;
        state.enemies[1].aistate = AIIDLE;
        
        state.enemies[2].position = glm::vec3(3,0,0);
        state.enemies[2].aitype = AIWALKER;
        state.enemies[2].aistate = AIIDLE;

    }
#pragma endregion
    
}

void Jump(){
    if(state.player->onGround)
        state.player->Jump();
}

void Shoot(){
    state.player->Shoot();
    Entity* bullet = new Entity();
    state.bullets.push_back(bullet);
    
    bullet->type = MYBULLET;
    bullet->velocity = glm::vec3(state.player->faceRight*8.0f,0,0);
    bullet->textureID = BulletTextureID;
    bullet->position = state.player->position;
    
    shootTime = shootCD;
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        //state.player->right = true;
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        if(shootTime <= 0)
                            Shoot();
                        break;
                    
                    case SDLK_UP:
                        Jump();
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->move = true;
        state.player->animIndices = state.player->animLeft;
        state.player->faceRight = -1;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->move = true;
        state.player->animIndices = state.player->animRight;
        state.player->faceRight = 1;
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position){
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), { u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    }
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data()); glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data()); glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID); glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void DeleteBullets(Entity* target, std::vector<Entity*>& bullets){
    std::vector<Entity*> newVec;
    for (auto& each : bullets) {
        if(each->position.x > 5 || each->position.x < -5 || each==target){
            delete each;
            each = NULL;
        }else{
            newVec.push_back(each);
        }
    }
    bullets = newVec;
}

void Update() {
    if(gameover) return;
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    if(shootTime>0)
        shootTime -= deltaTime;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP,state.player,state.platforms,state.enemies, state.bullets,state.enemybullets,PLATFORM_NUM, ENEMIES_NUM);
        for (auto each:state.bullets) {
            each->Update(FIXED_TIMESTEP,state.player, NULL, state.enemies,state.bullets,state.enemybullets, 0, ENEMIES_NUM);
        }
        for (int i=0; i<ENEMIES_NUM; i++) {
            if(state.enemies[i].aitype==AISHOOTER)
                state.enemies[i].colddown -= deltaTime;
            state.enemies[i].Update(FIXED_TIMESTEP,state.player, state.platforms,NULL,state.bullets, state.enemybullets, PLATFORM_NUM,0);
        }
        for(auto each:state.enemybullets){
            each->textureID = EnemyBulletTextureID;
            each->Update(FIXED_TIMESTEP, state.player,NULL, NULL,state.bullets, state.enemybullets, 0, 0);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    DeleteBullets(NULL,state.bullets);
    DeleteBullets(NULL,state.enemybullets);
    
    bool gamechecker = true;
    for (int i=0; i<ENEMIES_NUM; i++) {
        if(state.enemies[i].type != MYBULLET){
            gamechecker = false;
            break;
        }
    }
    
    if(gamechecker){
        gameover = true;
        gamewin = true;
    }
    if(state.player->gamestate == FAIL){
        gameover = true;
        gamewin = false;
    }
    
//    if(state.player->position.x > 0){
//        gameover = true;
//    }
//    if(state.player->collidedType != PLAYER){
//        gameover = true;
//        if(state.player->collidedType == DOOR)
//            gamewin = true;
//    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i=0; i<PLATFORM_NUM; i++) {
        state.platforms[i].Render(&program);
    }
    
    if(gameover){
        if(gamewin)
            DrawText(&program, FontTextureID, "MISSION SUCCESS", 1, -0.5f, glm::vec3(-4.25f, 3, 0));
        else
            DrawText(&program, FontTextureID, "MISSION FAILED", 1, -0.5f, glm::vec3(-4.25f, 3, 0));
    }
    
    for (int i=0; i<ENEMIES_NUM; i++) {
        state.enemies[i].Render(&program);
    }
    
    for(auto each:state.enemybullets){
        each->Render(&program);
    }
    
    for(auto each:state.bullets){
        each->Render(&program);
    }
    
    
    state.player->Render(&program);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
