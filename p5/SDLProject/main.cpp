#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "util.h"

#include "Entity.h"
#include "Map.h"
#include <vector>

#include "scene.h"
#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

Scene *currentScene;
Scene *sceneList[4];
void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

bool gameover = false;
bool gamewin = false;

SDL_Window* displayWindow;
Mix_Music* music;
Mix_Chunk* bounce;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

int Entity::life = 3;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Project4!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/10);
    
    bounce = Mix_LoadWAV("bounce.wav");
    
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
    
    sceneList[0] = new Level0();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
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
                        //currentScene->state.player->right = true;
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        break;
                    
                    case SDLK_RETURN:
                        if(currentScene == sceneList[0])
                            currentScene->state.nextScene = 1;
                        break;
                    
                    case SDLK_UP:
                        if(currentScene->state.player->Jump())
                            Mix_PlayChannel(-1,bounce,0);
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->move = true;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        currentScene->state.player->faceRight = -1;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->move = true;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
        currentScene->state.player->faceRight = 1;
    }


    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    if(Entity::life <= 0) gameover = true;
    if(currentScene->state.nextScene == 4) gamewin = true;
    if(!(gamewin || gameover)){
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;

        lastTicks = ticks;
        deltaTime += accumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }
        while (deltaTime >= FIXED_TIMESTEP) {
            currentScene->Update(FIXED_TIMESTEP);
            deltaTime -= FIXED_TIMESTEP;
        }
    }

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    
    if(dynamic_cast<Level0*>(currentScene))
        Util::DrawText(&program, fontTextureID, "Project5", 0.5f, -0.1f, glm::vec3(3.5, -2, 0));
    else Util::DrawText(&program, fontTextureID, std::to_string(Entity::life), 0.4f, 0.1f, glm::vec3(10, -3, 0));
    
    if(gameover){
        Util::DrawText(&program, fontTextureID, "YOU LOSE", 0.5f, -0.1f, currentScene->state.player->position);
    }else if(gamewin){
        Util::DrawText(&program, fontTextureID, "YOU WIN", 0.5f, -0.1f, currentScene->state.player->position);
    }
    
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
        
        if((currentScene->state.nextScene != -1) && Entity::life > 0 && (currentScene->state.nextScene != 4))
            SwitchToScene(sceneList[currentScene->state.nextScene]);
//
        
        Render();
    }
    
    Shutdown();
    return 0;
}
