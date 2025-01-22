#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "headers/Math.hpp"
#include "headers/Obj.hpp"
#include "headers/Shader.hpp"
#include "headers/BMP.hpp"
#include "headers/Camera.hpp"
#include "headers/GLHelpers.hpp"
#include "headers/Light.hpp"

GLFWwindow* windowInit(int w, int h, bool debugContext){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);
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
        color,
    };

    enum PolygonModes{
        point = GL_POINT,
        line = GL_LINE,
        fill = GL_FILL,
    };

    Camera camera;
    GLFWwindow* window;
    std::string windowTitle;
    unsigned int shaderID;
    PolygonModes polyMode;
    Math::Vec2<int> shaderSelection;
    Math::Vec2<int> resolution;
    Math::Vec3<float> rotationAxis;
    float mixPercentage;
    float lastFrameTime;
    float currFrameTime;
    float rotationAngle;
    int lightCount;
    int lightTotal;
    bool isRotating;
    bool isColored;

    runState(bool debugRun): windowTitle("42 scop :: "), polyMode(fill), lightCount(3), mixPercentage(1), shaderSelection(normals, grey), rotationAngle(0), isRotating(true), rotationAxis{0, 1, 0}, isColored(true){
        glfwInit();
        const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        resolution.x = videoMode->width * 0.75;
        resolution.y = videoMode->height * 0.75;
        window = windowInit(resolution.x, resolution.y, debugRun);
    }

    ~runState(){
        glfwTerminate();
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    runState* state = (runState*)glfwGetWindowUserPointer(window);
    state->resolution.x = width;
    state->resolution.y = height;
    glViewport(0, 0, state->resolution.x, state->resolution.y);
}

void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    runState* state = (runState*)glfwGetWindowUserPointer(window);
    if(key == GLFW_KEY_T && action == GLFW_PRESS){
        state->shaderSelection.x = state->shaderSelection.y;
        if(state->isColored)
            state->shaderSelection.y = state->texCoords;
        else
            state->shaderSelection.y = state->color;
        state->isColored = !state->isColored;
        state->mixPercentage = 0;
    }
    else if(key == GLFW_KEY_N && action == GLFW_PRESS){
        state->shaderSelection.x = state->shaderSelection.y;
        state->shaderSelection.y = state->normals;
        state->mixPercentage = 0;
    }
    else if(key == GLFW_KEY_3 && action == GLFW_PRESS){
        state->shaderSelection.x = state->shaderSelection.y;
        state->shaderSelection.y = state->triplanarMapping;
        state->mixPercentage = 0;
    }
    else if(key == GLFW_KEY_G && action == GLFW_PRESS){
        state->shaderSelection.x = state->shaderSelection.y;
        state->shaderSelection.y = state->grey;
        state->mixPercentage = 0;
    }

    if(key == GLFW_KEY_I && action == GLFW_PRESS){
        if(state->lightCount > 0)
            state->lightCount--;
    }
    else if(key == GLFW_KEY_O && action == GLFW_PRESS){
        if(state->lightCount < state->lightTotal)
            state->lightCount++;
    }

    if(key == GLFW_KEY_M && action == GLFW_PRESS){
        if(state->polyMode == state->fill)
            state->polyMode = state->point;
        else
            state->polyMode = (runState::PolygonModes)((int)state->polyMode + 1);

        glPolygonMode(GL_FRONT_AND_BACK, state->polyMode);
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS)
        state->isRotating = !state->isRotating;
}

void fetchContinuousInput(runState* state){
    if(glfwGetKey(state->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(state->window, true);

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
    int instantaneousFPS = 1 / (state->currFrameTime - state->lastFrameTime);
    glfwSetWindowTitle(state->window, (state->windowTitle + std::to_string(instantaneousFPS) + " FPS").c_str());
    fetchContinuousInput(state);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int projUniform = glGetUniformLocation(state->shaderID, "projection");
    int modelUniform = glGetUniformLocation(state->shaderID, "model");
    int viewUniform = glGetUniformLocation(state->shaderID, "view");
    glUniform1i(glGetUniformLocation(state->shaderID, "objScale"), mesh->getScale());
    glUniform1i(glGetUniformLocation(state->shaderID, "lightCount"), state->lightCount);
    glUniform1f(glGetUniformLocation(state->shaderID, "percentage"), state->mixPercentage);
    glUniform2iv(glGetUniformLocation(state->shaderID, "chosenShaders"), 1, &state->shaderSelection.x);
    glUniform3fv(glGetUniformLocation(state->shaderID, "viewPos"), 1, &state->camera.position.x);

    if(state->mixPercentage < 1)
        state->mixPercentage += 0.01;

    Math::Mat4<float> model(1.0f);
    if(state->isRotating)
        state->rotationAngle += (state->currFrameTime - state->lastFrameTime);
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

void printInfo(){
    std::cout << "Controls:\n";
    std::cout << "\tT          - Toggle between colors and texture\n";
    std::cout << "\tN          - Use Face Normals to Shade\n";
    std::cout << "\t3          - Use Triplanar Texture Mapping\n";
    std::cout << "\tG          - Use Greyscale Shading\n";
    std::cout << "\tM          - Cycle between Fill, Point and Line Rendering modes\n";
    std::cout << "\t\n";
    std::cout << "\tW & S      - Move Object Away & Toward Camera\n";
    std::cout << "\tA & D      - Move Object Sideways\n";
    std::cout << "\tLeft Ctrl  - Move Object Down\n";
    std::cout << "\tSpace      - Move Object Up\n";
    std::cout << "\t\n";
    std::cout << "\tR          - Toggle Object Rotation\n";
    std::cout << "\t\n";
    std::cout << "\tWASD       - Rotate Camera Around Object\n";
    std::cout << "\t\n";
    std::cout << "\tI & O      - To Increase/Decrease Number of Point Lights\n";
    std::cout << "\t\n";
    std::cout << "\tESC        - Exit Application\n";
}

void prepareLights(float objBounds, runState* state){
    std::vector<Light> lights;
    state->lightTotal = 4;
    lights.reserve(state->lightTotal);
    lights.push_back(Light{Math::Vec3{objBounds, 0.0f, 0.0f}, Math::Vec4{1.0f, 0.0f, 0.0f, 1.0f}});
    lights.push_back(Light{Math::Vec3{0.0f, objBounds, 0.0f}, Math::Vec4{0.0f, 1.0f, 0.0f, 1.0f}});
    lights.push_back(Light{Math::Vec3{0.0f, 0.0f, objBounds}, Math::Vec4{0.0f, 0.0f, 1.0f, 1.0f}});
    lights.push_back(Light{Math::Vec3{0.0f, 0.0f, -objBounds}, Math::Vec4{1.0f, 1.0f, 1.0f, 1.0f}});
    setLights(lights, state->shaderID);
}

int main(int argc, char** argv){
    std::cout << "=====--- 42 SCOP ---=====\n\n";

    if(argc < 2){
        std::cout << "Program must be opened with atleast a wavefront OBJ file path.\n";
        std::cout << "Optionally a BMP texture path can be specified as a second argument to replace the default one\n";
        std::cout << "USAGE: scop [--debug] <input_obj_path> [<input_texture_path>]\n";
        return 1;
    }

    int argIdx = 1;
    bool debugRun = false;
    if((std::string_view{argv[argIdx]}) == "--debug"){
        debugRun = true;
        argIdx++;
    }

    if((!debugRun && argc > 3) || argc > 4){
        std::cout << "Too many arguments\n";
        std::cout << "USAGE: scop [--debug] <input_obj_path> [<input_texture_path>]\n";
        return 1;
    }

    runState mainState{debugRun};
    if(mainState.window == nullptr)
        return 1;
    gladInit();
    glfwSetInputMode(mainState.window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetWindowUserPointer(mainState.window, &mainState);
    glfwSetFramebufferSizeCallback(mainState.window, framebuffer_size_callback);
    glfwSetKeyCallback(mainState.window, inputCallback);

    enableGLDebugMessages();
    glEnable(GL_DEPTH_TEST);
    glPointSize(2);

    Obj object{argv[argIdx++]};
    if(!object)
        return 1;

    char* texPath = (char*)"tex/gato.bmp";
    if(argIdx < argc)
        texPath = argv[argIdx];
    BMP tex = BMP(texPath);
    if(!tex)
        return 1;

    mainState.camera.movSpeed *= object.getScale();
    mainState.camera.position.z = object.boundingSphereRadius() / sin(mainState.camera.fov/2);
    mainState.windowTitle.append(object.getName());
    mainState.windowTitle.append(" :: ");
    Shader shdr{"shaders/default.vs", "shaders/default.fs"};
    mainState.shaderID = shdr;
    glUseProgram(shdr);
    prepareLights(object.boundingSphereRadius(), &mainState);
    printInfo();

    while(!glfwWindowShouldClose(mainState.window)){
        runLoop(&object, &mainState);
        glfwSwapBuffers(mainState.window);
        glfwPollEvents();
    }
}
