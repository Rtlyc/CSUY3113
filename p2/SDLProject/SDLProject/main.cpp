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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, ballMatrix, projectionMatrix, modelMatrix, leftMatrix;
GLuint playerTextureID, birdTextureID;
//float bird_x = 0.0f;
//float bird_rot = 0.0f;

//float player_x = 0.0f;
// Start at 0, 0, 0
glm::vec3 player_position = glm::vec3(5, 0, 0);
glm::vec3 left_position = glm::vec3(-5,0,0);
// Don’t go anywhere (yet).
glm::vec3 player_movement = glm::vec3(0, 0, 0);
glm::vec3 left_movement = glm::vec3(0,0,0);
float player_speed = 2.0f;

glm::vec3 ball_position = glm::vec3(0,0,0);
glm::vec3 ball_movement = glm::vec3(1,1,0);
float ball_speed = 2.0f;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project2!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    leftMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
     
}

void ProcessInput() {
    player_movement = glm::vec3(0);
    left_movement = glm::vec3(0);
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
                        // Move the player right
                        break;
                    case SDLK_SPACE:
                        // Some sort of action
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
    if (keys[SDL_SCANCODE_UP] && player_position.y <= 3.0f) {
        player_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && player_position.y >= -3.0f) {
        player_movement.y = -1.0f;
    }
    
    if(keys[SDL_SCANCODE_W] && left_position.y <= 3.0f){
        left_movement.y = 1.0f;
    }
    else if(keys[SDL_SCANCODE_S] && left_position.y >= -3.0f){
        left_movement.y = -1.0f;
    }
//
//    if (glm::length(player_movement) > 1.0f) {
//        player_movement = glm::normalize(player_movement);
//    }

}


float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    modelMatrix = glm::mat4(1.0f);
    player_position += player_movement * player_speed * deltaTime;
    modelMatrix = glm::translate(modelMatrix, player_position);
    
    leftMatrix = glm::mat4(1.0f);
    left_position += left_movement * player_speed * deltaTime;
    leftMatrix = glm::translate(leftMatrix, left_position);
    
    ballMatrix = glm::mat4(1.0f);
    ball_position += ball_movement * ball_speed * deltaTime;
    ballMatrix = glm::translate(ballMatrix, ball_position);
    
    float xdist = fabs(player_position.x - ball_position.x) - ((0.5+1) / 2.0f);
    float ydist = fabs(player_position.y - ball_position.y) - ((0.5 + 2) / 2.0f);
    
    if (xdist < 0 && ydist < 0){
        ball_speed *= -1;
        ball_movement.y *= -1;
    }
    
    xdist = fabs(left_position.x - ball_position.x) - ((0.5+1) / 2.0f);
    ydist = fabs(left_position.y - ball_position.y) - ((0.5 + 2) / 2.0f);
    
    if (xdist < 0 && ydist < 0){
        ball_speed *= -1;
        ball_movement.y *= -1;
    }
    
    if(ball_position.y >= 3.5f || ball_position.y <= -3.5f){
        ball_movement.y *= -1;
    }
    
    if(ball_position.x >= 5.0f || ball_position.x <= -5.0f){
        gameIsRunning = false;
    }
    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(modelMatrix);
    float vertices[] = { 0.5f, 1.0f, 0.5f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, -0.5f, -1.0f, -0.5f, 1.0f };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    program.SetModelMatrix(leftMatrix);
    float vertices1[] = { 0.5f, 1.0f, 0.5f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, -0.5f, -1.0f, -0.5f, 1.0f };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    program.SetModelMatrix(ballMatrix);
    float vertices2[] = { -0.25f, 0.25f, 0.25f, 0.25f, 0.25f, -0.25f, 0.25f, -0.25f, -0.25f, -0.25f, -0.25f, 0.25f };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);

    
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
