#include <iostream>
#include <fstream>
#include "pgr.h"
#include "render.h"
#include "ModelContainer.h"
#include "ShaderContainer.h"


int main(int argc, char** argv);

int WIN_WIDTH = 512;
int WIN_HEIGHT = 512;
const char * WIN_TITLE = "Hello World";

const char* CONFIG_FILE_PATH = "ModelsData.txt";

DirectLight direct_light;
PointLight point_light;
SpotLight spot_light;

/// <summary>
/// These parameters allows to look around with camera
/// </summary>
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat xOrigin = WIN_WIDTH / 2;
GLfloat yOrigin = WIN_HEIGHT / 2;

Camera camera_walk;
Camera camera_1;
Camera camera_2;
Camera camera_anim_obj;

int camera_mode = 0;

glm::vec3 movingDirection = glm::vec3(0.0f, 0.0f, -1.0f);
GLfloat movingSpeed = 0.3f;

/// <summary>
/// Defines borders of simple 2d collider
/// </summary>
struct Collider2D {
    glm::vec2 left_back_corner;
    glm::vec2 right_forward_corner;
};
glm::vec2 walk_area_LB = glm::vec2(- 10.25f, 5.5f);
glm::vec2 walk_area_RF = glm::vec2(4.75f, -13.5f);
std::vector<Collider2D> colliders;

bool keys[1024];
bool lockCursor = false;

enum SpecialKeys {
    KEY_RIGHT_ARROW,
    KEY_LEFT_ARROW,
    KEY_UP_ARROW,
    KEY_DOWN_ARROW
};

int last_time;

/// <summary>
/// Initialize main parameters of the program
/// </summary>
void init() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

  last_time = glutGet(GLUT_ELAPSED_TIME);

  LoadData(CONFIG_FILE_PATH);

  glm::vec3 campfire_position;
  GetCampfireData(campfire_position);
  Collider2D collider;
  collider.left_back_corner = glm::vec2(campfire_position.x - 1.0f, campfire_position.z + 1.0f);
  collider.right_forward_corner = glm::vec2(campfire_position.x + 1.0f, campfire_position.z - 1.0f);
  colliders.push_back(collider);

  camera_walk.position = glm::vec3(0.0f, 1.5f, 3.0f);
  camera_walk.direction = glm::vec3(0.0f, 0.0f, -1.0f);
  camera_walk.camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

  camera_1.position = glm::vec3(-9.59f, 1.5f, 8.9f);
  camera_1.direction = glm::vec3(0.75023f, -0.163326f, 0.640687f);
  camera_1.camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

  camera_2.position = glm::vec3(-15.64f, 1.5f, 7.86f);
  camera_2.direction = glm::vec3(0.408345f, -0.228351f, -0.883804f);
  camera_2.camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

  direct_light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
  direct_light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
  direct_light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
  direct_light.direction = glm::vec3(-0.5f, -1.0f, 1.0f);

  point_light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
  point_light.diffuse = glm::vec3(0.8f, 0.4f, 0.0f);
  point_light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
  point_light.linear = 0.045f;
  point_light.quadratic = 0.0075f;
  GetCampfireData(point_light.position);
  point_light.position += glm::vec3(0.0f, 1.0f, 0.0f);
  point_light.intensity = 1.0f;

  spot_light.point.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
  spot_light.point.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
  spot_light.point.specular = glm::vec3(1.0f, 1.0f, 1.0f);
  spot_light.point.linear = 0.045f;
  spot_light.point.quadratic = 0.0075f;
  spot_light.point.position = camera_walk.position;
  spot_light.direction = camera_walk.direction;
  spot_light.cut_off = 20.0f;
  spot_light.point.intensity = 0.0f;
}

void FollowAnimatedObject() 
{
    glm::vec3 anim_obj_position, anim_obj_direction;
    GetAnimObjectData(anim_obj_position, anim_obj_direction);
    camera_anim_obj.camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera_anim_obj.position = anim_obj_position - anim_obj_direction + glm::vec3(0.0f, 0.5f, 0.0f);
    camera_anim_obj.direction = glm::normalize(anim_obj_position - camera_anim_obj.position);
}

void draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearStencil(0);
  Camera camera;
  switch (camera_mode) {
  case 0:
      camera = camera_walk;
      break;
  case 1:
      camera = camera_1;
      break;
  case 2:
      camera = camera_2;
      break;
  case 3:
      camera = camera_anim_obj;
      break;
  }

  int current_time = glutGet(GLUT_ELAPSED_TIME);
  float dt = (current_time - last_time) / 1000.0f;
  last_time = current_time;

  float cos_val = glm::cos(current_time / 10000.0f);
  SetNightValue((cos_val + 1) / 2);
  direct_light.intensity = (-cos_val + 1) / 2;

  Draw(direct_light, point_light, spot_light, camera, (GLfloat)WIN_WIDTH, (GLfloat)WIN_HEIGHT, dt);

  glutSwapBuffers();
}

/// <summary>
/// Processes new position depending on the scene and colliders borders
/// </summary>
/// <param name="old_position"></param>
/// <param name="new_position"></param>
void ProcessNewPosition(const glm::vec3& old_position, glm::vec3& new_position)
{
    new_position.x = glm::clamp(new_position.x, walk_area_LB.x, walk_area_RF.x);
    new_position.z = glm::clamp(new_position.z, walk_area_RF.y, walk_area_LB.y);

    for (GLuint i = 0; i < colliders.size(); i++) 
        if (new_position.x > colliders[i].left_back_corner.x && new_position.x < colliders[i].right_forward_corner.x &&
            new_position.z > colliders[i].right_forward_corner.y && new_position.z < colliders[i].left_back_corner.y) {
            if (old_position.x > colliders[i].left_back_corner.x && old_position.x < colliders[i].right_forward_corner.x)
                new_position.z = old_position.z;
            else if (old_position.z > colliders[i].right_forward_corner.y && old_position.z < colliders[i].left_back_corner.y)
                new_position.x = old_position.x;
            else new_position = old_position;
        }
}

void timerCallback(int)
{
    glm::vec3 new_camera_pos = camera_walk.position;
    if (keys[KEY_UP_ARROW])
        new_camera_pos += movingDirection * movingSpeed;
    if (keys[KEY_DOWN_ARROW])
        new_camera_pos -= movingDirection * movingSpeed;
    if (keys[KEY_RIGHT_ARROW])
        new_camera_pos += glm::normalize(glm::cross(movingDirection, camera_walk.camera_up)) * movingSpeed;
    if (keys[KEY_LEFT_ARROW])
        new_camera_pos -= glm::normalize(glm::cross(movingDirection, camera_walk.camera_up)) * movingSpeed;

    ProcessNewPosition(camera_walk.position, new_camera_pos);
    camera_walk.position = new_camera_pos;

    spot_light.point.position = camera_walk.position;

    if (camera_mode == 3) FollowAnimatedObject();

    //std::cout << "position: (" << camera_walk.position.x << ", " << camera_walk.position.y << ", " << camera_walk.position.z << ")" << std::endl;

    glutTimerFunc(33, timerCallback, 0);

    glutPostRedisplay();
}

void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) 
{
    switch (specKeyPressed) {
    case GLUT_KEY_RIGHT:
        keys[KEY_RIGHT_ARROW] = true;
        break;
    case GLUT_KEY_LEFT:
        keys[KEY_LEFT_ARROW] = true;
        break;
    case GLUT_KEY_UP:
        keys[KEY_UP_ARROW] = true;
        break;
    case GLUT_KEY_DOWN:
        keys[KEY_DOWN_ARROW] = true;
        break;
    default:
        break;
    }
}

void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY) 
{
    switch (specKeyReleased) {
    case GLUT_KEY_RIGHT:
        keys[KEY_RIGHT_ARROW] = false;
        break;
    case GLUT_KEY_LEFT:
        keys[KEY_LEFT_ARROW] = false;
        break;
    case GLUT_KEY_UP:
        keys[KEY_UP_ARROW] = false;
        break;
    case GLUT_KEY_DOWN:
        keys[KEY_DOWN_ARROW] = false;
        break;
    default:
        break;
    }
}

void mouseMove(int x, int y) {

    if (camera_mode != 0) return;

    yaw+= ((GLfloat)x - xOrigin) * 0.1f;
    pitch -= ((GLfloat)y - yOrigin) * 0.1f;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 new_direction;
    new_direction.x = cos(glm::radians(yaw));
    new_direction.z = sin(glm::radians(yaw));

    movingDirection = glm::normalize(new_direction);

    new_direction.x *= cos(glm::radians(pitch));
    new_direction.y = sin(glm::radians(pitch));
    new_direction.z *= cos(glm::radians(pitch));

    camera_walk.direction = glm::normalize(new_direction);
    spot_light.direction = camera_walk.direction;

    glutWarpPointer(xOrigin, yOrigin);
}

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) 
{
    switch (keyPressed)
    {
    case 27:
        ClearData();
#ifndef __APPLE__
        glutLeaveMainLoop();
#else
        exit(0);
#endif
        break;
    case 'f':
        if (!lockCursor) {
            xOrigin = WIN_WIDTH / 2;
            yOrigin = WIN_HEIGHT / 2;
            glutWarpPointer(xOrigin, yOrigin);
            glutPassiveMotionFunc(mouseMove);
        }
        else 
            glutPassiveMotionFunc(NULL);
        lockCursor = !lockCursor;
        break;
    case 'r':
        ClearData();
        LoadData(CONFIG_FILE_PATH);
        break;
    case 'w':
        keys[KEY_UP_ARROW] = true;
        break;
    case 's':
        keys[KEY_DOWN_ARROW] = true;
        break;
    case 'd':
        keys[KEY_RIGHT_ARROW] = true;
        break;
    case 'a':
        keys[KEY_LEFT_ARROW] = true;
        break;
    case 'c':
        keys['c'] = true;
        break;
    case 'm':
        keys['m'] = true;
        break;
    case '1':
        if (keys['c']) camera_mode = 0;
        if (keys['m']) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case '2':
        if (keys['c']) camera_mode = 1;
        if (keys['m']) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case '3':
        if (keys['c']) camera_mode = 2;
        if (keys['m']) glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case '4':
        if (keys['c']) camera_mode = 3;
        break;
    case 'g':
        SwitchFog();
        break;
    case 'l':
        spot_light.point.intensity += 1.0;
        if (spot_light.point.intensity > 10) spot_light.point.intensity = 0;
        break;
    default:
        break;
    }    
}

void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY)
{
    switch (keyReleased)
    {
    case 'w':
        keys[KEY_UP_ARROW] = false;
        break;
    case 's':
        keys[KEY_DOWN_ARROW] = false;
        break;
    case 'd':
        keys[KEY_RIGHT_ARROW] = false;
        break;
    case 'a':
        keys[KEY_LEFT_ARROW] = false;
        break;
    case 'c':
        keys['c'] = false;
        break;
    case 'm':
        keys['m'] = false;
        break;
    default:
        break;
    }
}

void reshapeCallback(int newWidth, int newHeight) {

    WIN_WIDTH = newWidth;
    WIN_HEIGHT = newHeight;

    glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

void mouseCallback(int button, int state, int x, int y) {
    // do picking only on mouse down
    if (state == GLUT_DOWN) {
        unsigned char pixelID;
        glReadPixels(x, WIN_HEIGHT - 1 - y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &pixelID);

        if (pixelID == 0) std::cout << "clicked on background" << std::endl;
        else std::cout << "clicked on object with ID: " << (int)pixelID << std::endl;

        switch (pixelID) {
        case 1:
            point_light.intensity = SwitchCampfire() ? 1.0f : 0.0f;
            break;
        case 2:
            SwitchFog();
            break;
        case 3:
            SwitchAnimObject();
            break;
        default:
            break;
        }

    }
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);

    glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow(WIN_TITLE);

    glutDisplayFunc(draw);
    glutSpecialFunc(specialKeyboardCallback);
    glutSpecialUpFunc(specialKeyboardUpCallback);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutReshapeFunc(reshapeCallback);
    glutMouseFunc(mouseCallback);
    
    glutTimerFunc(33, timerCallback, 0);

    if(!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
      pgr::dieWithError("pgr init failed, required OpenGL not supported?");

    init();

    glutMainLoop();

    return 0;
}
