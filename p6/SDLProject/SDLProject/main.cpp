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
#include <vector>
#include <GLUT/glut.h>
#include <iostream>
#include <SDL_mixer.h>
#include <time.h>
#include <stdlib.h>

#include "Map.h"
#include "Entity.h"
#include "Tower.h"
#include "Bullet.h"
#include "Monster.h"
#include "Util.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gamewin,gamelost;

ShaderProgram program;
ShaderProgram texture_program;
glm::mat4 viewMatrix, ballMatrix, projectionMatrix;
GLuint fontTextureID;
Mix_Music* music;



Map* map;
int cost = 4;
int money = 15;
int health = 3;
int curlevel = 1;
bool wavestart = false;
int total_wave = 9;
float prev_wave_time = 2;
float next_wave_period = 5;
bool frontPage = true;





unsigned int level_data[] = {
    2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

std::vector<Tower*> towers;
std::vector<Monster*> monsters;
std::vector<Bullet*> bullets;

bool CreateTower(std::vector<Tower*>& towers, int x, int y){
    int index = map->MousePosToIndex(x, y);
    Tower* potent = map->grids[index]->tower;
    if(map->grids[index]->type!=GRASS||(potent&&potent->level==2)) return false;
    
    // money decrease
    if(money>=cost) money-=cost;
    else return false;
//    int index, float size, float range, float damage, float cool_down, Map* map
    if(map->grids[index]->tower==nullptr){
    Tower* new_tower = new Tower(index,1,3,2,1,map);
    towers.push_back(new_tower);
        map->grids[index]->tower = new_tower;}
    else{
        potent->textureID = Util::LoadTexture("Donut.png");
        potent->level = 2;
        potent->damage *= 2;
        potent->range += 1;
    }
    return true;
}

void Level(int num, float size, float speed, float maxhealth){
    for (int i=0; i<monsters.size(); i++) {
        delete monsters[i];
        monsters[i] = nullptr;
    }
    monsters.clear();
    float startx = -5.5f;
    //float x, float y, float size, float speed, float maxhealth
    for (int i=0; i<num; i++) {
        float random = rand();
        float y = random/RAND_MAX;
        monsters.push_back(new Monster(startx,y-0.5f,size,speed,maxhealth));
        startx -= 2;
    }
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project2!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("BG Music.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/10);
    srand( (unsigned)time(NULL) );;

    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    texture_program.Load("shaders/vertex_textured.glsl","shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    fontTextureID = Util::LoadTexture("font1.png");
    texture_program.SetProjectionMatrix(projectionMatrix);
    texture_program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    map = new Map(level_data,LEVEL_WIDTH,LEVEL_HEIGHT,-6.5,3.5);
//    float x, float speed, float size, float maxhealth
//    float x, float y, float size, float speed, float maxhealth
}

void ProcessInput() {
    SDL_Event event;
    int x,y;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(frontPage) break;
                SDL_GetMouseState(&x, &y);
                CreateTower(towers, x, y);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                    case SDLK_SPACE:
                        Level(5, 0.8, 0.2, 10);
                        // Some sort of action
                        break;
                    case SDLK_RETURN:
                        frontPage = false;
                        break;
                }
                break; // SDL_KEYDOWN
        }

    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
//    if (keys[SDL_SCANCODE_LEFT]) {
//        player_movement.x = -1.0f;
//    }
//    else if (keys[SDL_SCANCODE_RIGHT]) {
//        player_movement.x = 1.0f;
//    }
    if (keys[SDL_SCANCODE_UP]) {
        
    }
    else if (keys[SDL_SCANCODE_DOWN]) {

    }
    
    if(keys[SDL_SCANCODE_W]){

    }
    else if(keys[SDL_SCANCODE_S]){

    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    if(health<=0) gamelost = true;
    else if(curlevel>total_wave) gamewin = true;
    if(gamewin||gamelost) return;
    
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;

    lastTicks = ticks;
    deltaTime += accumulator;
    if(frontPage) return;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        for (int i=0; i<monsters.size(); i++) {
            monsters[i]->Update(FIXED_TIMESTEP,monsters,bullets,health,money);
        }
        
        for (int i=0; i<towers.size(); i++) {
            towers[i]->Update(FIXED_TIMESTEP, monsters, bullets);
        }

        for(int i=0; i<bullets.size(); i++){
            bullets[i]->Update(FIXED_TIMESTEP, bullets);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    if(monsters.empty()){
        if(wavestart){
            //meaning current wave has been done
            prev_wave_time = ticks;
            wavestart = false;
            curlevel++;
        }
        else if(ticks>=prev_wave_time+next_wave_period){
            Level(curlevel, 0.6, curlevel*0.2, curlevel*5);
            wavestart = true;
        }
    }

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program.programID);
    map->Render(&program);
    glUseProgram(texture_program.programID);
    for (Tower* each : towers) {
        each->Render(&texture_program);
    }
    glUseProgram(program.programID);
    for (int i=0; i<monsters.size(); i++) {
        monsters[i]->Render(&program);
    }

    for(int i=0; i<bullets.size(); i++){
        bullets[i]->Render(&program);
    }
    std::string money_string = "Money:" + std::to_string(money);
    std::string health_string = "Health:" + std::to_string(health);
    std::string wave_string = "Wave:" + std::to_string(curlevel) + "/" + std::to_string(total_wave);
    Util::DrawText(&texture_program, fontTextureID, money_string, 0.5f, -0.14f, glm::vec3(-1.3, 3.5, 0));
    Util::DrawText(&texture_program, fontTextureID, health_string, 0.5f, -0.14f, glm::vec3(-1.3, 3, 0));
    Util::DrawText(&texture_program, fontTextureID, wave_string, 0.5f, -0.14f, glm::vec3(-1.3, 2.5, 0));
    
    if(gamewin) Util::DrawText(&texture_program, fontTextureID, "You Win!", 1, -0.14f, glm::vec3(-3, 0, 0));
    else if(gamelost) Util::DrawText(&texture_program, fontTextureID, "You Lose!", 1, -0.14f, glm::vec3(-3, 0, 0));
    
    if(frontPage){
        glUseProgram(program.programID);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.529, 0.807, 0.92, 0.0);
        Util::DrawText(&texture_program, fontTextureID, "DONUT_DEFENSE", 0.5f, -0.14f, glm::vec3(-2, 2, 0));
    }

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    std::cout << "INITIALIZING.."<< std::endl;
    Initialize();
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    delete map;
    return 0;
}
