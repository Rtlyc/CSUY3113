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

enum EntityType {PLAYER, BOX, DOOR, MYBULLET, ENEMY, ENEMYBULLET};
enum AIType {AIWALKER, AISHOOTER, AIJUMPER};
enum AIState {AIIDLE, AIWALK, AISHOOT, AIJUMP, AIDEAD};
enum GameStatus{RUNNING, FAIL, SUCCESS};

class Entity {
public:
    AIType aitype;
    AIState aistate;
    GameStatus gamestate;
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
    bool onGround = false;
    int faceRight = 1;
    float movePower = 0.1f;
    
    float shootCD = 3.0f;
    float colddown = 0.0f;
    
    Entity();
    
    void Update(float deltaTime, Entity* player, Entity* platforms, Entity* enemies,std::vector<Entity*>& mybullets, std::vector<Entity*>& enemybullets, int num, int enemynum);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    bool CheckCollision(Entity* other);
    bool CheckCollisionsY(Entity *objects, int objectCount);
    bool CheckCollisionsX(Entity *objects, int objectCount);
    void Jump();
    void Shoot();
    void AIJump(Entity* player, AIState state);
    void AIShoot(Entity* player, std::vector<Entity*>& bullets, AIState state);
    void AIIdle(Entity* player, AIState);
    void AIWalk(Entity* player, AIState);
    void AIDead();
    void AI(Entity* player, std::vector<Entity*>& bullets);
};
