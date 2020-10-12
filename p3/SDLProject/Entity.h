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

enum EntityType {PLAYER, BOX, DOOR};

class Entity {
public:
    int test;
    EntityType type;
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 velocity;
    glm::vec3 acceleration; 
    float speed;
    float width = 1.0f;
    float height = 1.0f;
    EntityType collidedType;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool move = false;
    float movePower = 0.1f;
    
    Entity();
    
    void Update(float deltaTime, Entity* platforms, int num);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
};
