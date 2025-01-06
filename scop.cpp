#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "headers/Math.hpp"
#include "headers/Obj.hpp"
#include "headers/Shader.hpp"
#include "headers/BMP.hpp"
#include "headers/Camera.hpp"

void GLAPIENTRY MessageCallback( GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar* message,
                                 const void* userParam ){
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
    // std::cout << "GL CALLBACK: " << ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ) << " type = 0x" << std::hex << type << ", severity = 0x" << severity << ", message = " << message << "\n";
}

GLFWwindow* windowInit(int w, int h){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);      //Required on MacOS

    GLFWwindow* window = glfwCreateWindow(w, h, "", NULL, NULL);
    if (window == nullptr)
        std::cout << "Failed to create GLFW window" << std::endl;
    else
        glfwMakeContextCurrent(window);
    return window;
}

bool gladInit(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

struct runState{
    enum ShaderEnum{
        texCoords,
        normals,
        triplanarMapping,
        grey,
    };

    Camera camera;
    GLFWwindow* window;
    std::vector<Shader> shaderVec;
    ShaderEnum shaderSelection;
    Math::Vec2<int> resolution;
    Math::Vec3<float> rotationAxis;
    float lastFrameTime;
    float currFrameTime;
    float rotationAngle;
    bool isRotating;

    runState(): shaderSelection(grey), rotationAngle(0), isRotating(true), rotationAxis{0, 1, 0}{
        glfwInit();
        const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        resolution.x = videoMode->width * 0.75;
        resolution.y = videoMode->height * 0.75;
        window = windowInit(resolution.x, resolution.y);
        shaderVec.reserve(4);
    }

    ~runState(){
        glfwTerminate();
    }
};

void toggleRotation(GLFWwindow* window, int key, int scancode, int action, int mods){
    runState* state = (runState*)glfwGetWindowUserPointer(window);
    if(key == GLFW_KEY_T && action == GLFW_PRESS)
        state->isRotating = !state->isRotating;
}

void processInput(runState* state)
{
    if(glfwGetKey(state->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(state->window, true);

    if(glfwGetKey(state->window, GLFW_KEY_V) == GLFW_PRESS){
        if(glfwGetKey(state->window, GLFW_KEY_1) == GLFW_PRESS)
            state->shaderSelection = state->texCoords;
        else if(glfwGetKey(state->window, GLFW_KEY_2) == GLFW_PRESS)
            state->shaderSelection = state->normals;
        else if(glfwGetKey(state->window, GLFW_KEY_3) == GLFW_PRESS)
            state->shaderSelection = state->triplanarMapping;
        else if(glfwGetKey(state->window, GLFW_KEY_4) == GLFW_PRESS)
            state->shaderSelection = state->grey;
    }

    if(glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS)
        state->camera.move(Camera::Back, state->currFrameTime - state->lastFrameTime);
    if(glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS)
        state->camera.move(Camera::Right, state->currFrameTime - state->lastFrameTime);
    if(glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS)
        state->camera.move(Camera::Fore, state->currFrameTime - state->lastFrameTime);
    if(glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS)
        state->camera.move(Camera::Left, state->currFrameTime - state->lastFrameTime);
    if(glfwGetKey(state->window, GLFW_KEY_SPACE) == GLFW_PRESS)
        state->camera.move(Camera::Down, state->currFrameTime - state->lastFrameTime);
    if(glfwGetKey(state->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        state->camera.move(Camera::Up, state->currFrameTime - state->lastFrameTime);

    if(glfwGetKey(state->window, GLFW_KEY_UP) == GLFW_PRESS){
        state->camera.track(Math::Vec3(0.0f, 0.0f, 0.0f));
        state->camera.move(Camera::Up, state->currFrameTime - state->lastFrameTime);
        state->camera.stopTracking();
    }
    if(glfwGetKey(state->window, GLFW_KEY_DOWN) == GLFW_PRESS){
        state->camera.track(Math::Vec3(0.0f, 0.0f, 0.0f));
        state->camera.move(Camera::Down, state->currFrameTime - state->lastFrameTime);
        state->camera.stopTracking();
    }
    if(glfwGetKey(state->window, GLFW_KEY_LEFT) == GLFW_PRESS){
        state->camera.track(Math::Vec3(0.0f, 0.0f, 0.0f));
        state->camera.move(Camera::Left, state->currFrameTime - state->lastFrameTime);
        state->camera.stopTracking();
    }
    if(glfwGetKey(state->window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        state->camera.track(Math::Vec3(0.0f, 0.0f, 0.0f));
        state->camera.move(Camera::Right, state->currFrameTime - state->lastFrameTime);
        state->camera.stopTracking();
    }
}

void runLoop(Obj* mesh, runState* state){
    state->currFrameTime = glfwGetTime();
    processInput(state);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int currShaderID = state->shaderVec[state->shaderSelection];
    glUseProgram(currShaderID);
    int projUniform = glGetUniformLocation(currShaderID, "projection");
    int modelUniform = glGetUniformLocation(currShaderID, "model");
    int viewUniform = glGetUniformLocation(currShaderID, "view");
    if(state->shaderSelection == state->triplanarMapping)
        glUniform1i(glGetUniformLocation(currShaderID, "objScale"), mesh->getScale());

    Math::Mat4<float> model(1.0f);
    if(state->isRotating)
        state->rotationAngle += 0.01;
    Math::Mat4<float> rotation = Math::Quaternion<float>(state->rotationAxis, state->rotationAngle).getMatrix();
    Math::Vec3<float> translation = Math::Vec3{0.0f, 0.0f, 0.0f} - mesh->getMidPoint();
    model = Math::translationMatrix(model, translation);
    model = rotation * model;
    Math::Mat4<float> view = state->camera.getViewMatrix();
    float cameraDistance = (state->camera.position - Math::Vec3<float>(0,0,0)).length();
    Math::Mat4<float> perspective = Math::perspectiveMatrix(state->camera.fov, (float) state->resolution.x/state->resolution.y, 0.1f, mesh->boundingSphereRadius() + cameraDistance);
    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (GLfloat*)&model);
    glUniformMatrix4fv(projUniform, 1, GL_FALSE, &perspective[0][0]);
    glUniformMatrix4fv(viewUniform, 1, GL_FALSE, (GLfloat*)&view);
    mesh->draw();
    state->lastFrameTime = state->currFrameTime;
}

void prepareShaders(runState* state){
    state->shaderVec.emplace_back("shaders/default.vs", "shaders/vtxTexCoord.fs");
    state->shaderVec.emplace_back("shaders/default.vs", "shaders/normal.fs");
    state->shaderVec.emplace_back("shaders/default.vs", "shaders/triplanarMapping.fs");
    state->shaderVec.emplace_back("shaders/default.vs", "shaders/vtxColor.fs");
}

void printInfo(){
    std::cout << "Controls:\n";
    std::cout << "\tV + 1      - Use Object's Texture Coordinates\n";
    std::cout << "\tV + 2      - Use Face Normals to Shade\n";
    std::cout << "\tV + 3      - Use Triplanar Mapping\n";
    std::cout << "\tV + 4      - Use Greyscale Shading\n";
    std::cout << "\t\n";
    std::cout << "\tW & S      - Move Object Away & Toward Camera\n";
    std::cout << "\tA & D      - Move Object Sideways\n";
    std::cout << "\tLeft Ctrl  - Move Object Down\n";
    std::cout << "\tSpace      - Move Object Up\n";
    std::cout << "\t\n";
    std::cout << "\tT          - Toggle Object Rotation\n";
    std::cout << "\t\n";
    std::cout << "\tWASD       - Rotate Camera Around Object\n";
    std::cout << "\t\n";
    std::cout << "\tESC        - Exit Application\n";
}

int main(int argc, char** argv){
    std::cout << "=====--- 42 SCOP ---=====\n\n";

    if(argc < 2){
        std::cout << "Program must be opened with atleast a wavefront OBJ file path.\n";
        std::cout << "Optionally a BMP texture path can be specified as a second argument to replace the default one\n";
        return 1;
    }

    runState mainState;
    if(mainState.window == nullptr)
        return 1;
    gladInit();
    glfwSetInputMode(mainState.window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetWindowUserPointer(mainState.window, &mainState);
    glfwSetKeyCallback(mainState.window, toggleRotation);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glViewport(0, 0, mainState.resolution.x, mainState.resolution.y);

    Obj object{argv[1]};
    if(!object)
        return 1;

    char* texPath = (char*)"tex/gato.bmp";
    if(argc > 2)
        texPath = argv[2];
    BMP tex = BMP(texPath);
    if(!tex)
        return 1;

    mainState.camera.movSpeed *= object.getScale();
    mainState.camera.position.z = object.boundingSphereRadius() / sin(mainState.camera.fov/2);
    glfwSetWindowTitle(mainState.window, object.getName()->insert(0, "42 scop :: ").c_str());
    prepareShaders(&mainState);
    printInfo();

    while(!glfwWindowShouldClose(mainState.window)){
        runLoop(&object, &mainState);
        glfwSwapBuffers(mainState.window);
        glfwPollEvents();
    }
}
