
#include "application.hpp"
#include "camera.hpp"
#include "diffuse_light.hpp"
#include "lights_manager.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "plane.h"
#include <random>

LightsManager *lightsManager;
float lastX = 0;
float lastY = 0;
bool firstMouse = true;
// timing
double deltaTime = 0.0f;// time between current frame and last frame
double lastFrame = 0.0f;
Camera *camera;
int pressedKey = -1;

template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to) {
    thread_local static Generator gen(std::random_device{}());

    using dist_type = typename std::conditional<
            std::is_integral<Numeric>::value, std::uniform_int_distribution<Numeric>, std::uniform_real_distribution<Numeric> >::type;

    thread_local static dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
}

std::vector<glm::vec3> getCoordsForVertices(double xc, double yc, double size, int n) {
    std::vector<glm::vec3> vertices;
    auto xe = xc + size;
    auto ye = yc;
    vertices.emplace_back(xe, yc, ye);
    double alpha = 0;
    for (int i = 0; i < n - 1; i++) {
        alpha += 2 * M_PI / n;
        auto xr = xc + size * cos(alpha);
        auto yr = yc + size * sin(alpha);
        xe = xr;
        ye = yr;
        vertices.emplace_back(xe, yc, ye);
    }
    return vertices;
}

void programQuit([[maybe_unused]] int key, [[maybe_unused]] int action, Application *app) {
    app->close();
    LOG_S(INFO) << "Quiting...";
}

void wasdKeyPress([[maybe_unused]] int key, [[maybe_unused]] int action, [[maybe_unused]] Application *app) {
    if (action == GLFW_PRESS) { pressedKey = key; }
    if (action == GLFW_RELEASE) { pressedKey = -1; }
}

void moveCamera() {
    if (pressedKey == GLFW_KEY_W) { camera->ProcessKeyboard(FORWARD, (float) deltaTime); }
    if (pressedKey == GLFW_KEY_S) { camera->ProcessKeyboard(BACKWARD, (float) deltaTime); }
    if (pressedKey == GLFW_KEY_A) { camera->ProcessKeyboard(LEFT, (float) deltaTime); }
    if (pressedKey == GLFW_KEY_D) { camera->ProcessKeyboard(RIGHT, (float) deltaTime); }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top

    lastX = (float) xpos;
    lastY = (float) ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera->ProcessMouseScroll(yoffset);
}

int main(int argc, char *argv[]) {
    Application app({1280, 720}, argc, argv);
    Application::setOpenGLFlags();
    app.registerKeyCallback(GLFW_KEY_ESCAPE, programQuit);

    app.registerKeyCallback(GLFW_KEY_W, wasdKeyPress);
    app.registerKeyCallback(GLFW_KEY_A, wasdKeyPress);
    app.registerKeyCallback(GLFW_KEY_S, wasdKeyPress);
    app.registerKeyCallback(GLFW_KEY_D, wasdKeyPress);

    lastX = app.getWindow()->getWindowSize().x / 2.0f;
    lastY = app.getWindow()->getWindowSize().y / 2.0f;

    glDepthFunc(GL_LESS);
    glCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

    Shader shader_tex("../shaders/multiple_diffuse_shader.glsl", false);
    shader_tex.bind();
    shader_tex.setUniform1i("u_Texture", 0);
    shader_tex.setUniform1i("numDiffLights", 1);
    ObjLoader objLoader;
    std::vector<Mesh *> meshes;

    //  water.setUniform1i("numDiffLights", 1);

    std::vector<Plane *> planes;
    meshes.push_back(new Mesh("../resources/models/mountain.obj"));
    meshes.back()->addTexture("../textures/mountain.png")->setScale({0.2, 1, 0.2})->setPosition(
            {196, 10, -173})->setRotation({180, 43, 0})->compile();

    meshes.push_back(new Mesh(meshes.back()->loadedOBJ));
    meshes.back()->addTexture("../textures/mountain.png")->setScale({0.2, 1.2, 0.45})->setPosition(
            {289, 10, -88})->setRotation({180, 245, 0})->compile();

    meshes.push_back(new Mesh("../resources/models/lake.obj"));
    meshes.back()->addTexture("../textures/sand.png")->setScale({0.1, 0.1, 0.1})->setPosition(
            {0, 0, 0})->setRotation({0, 0, 0})->compile();
    lightsManager = new LightsManager;
    lightsManager->addLight(DiffuseLight("1_1", {{5, 60, 5}, {0.8, 0.8, 0.8}, 0.8}));

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {60, 60, 60}, false));
    planes.back()->addTexture("../textures/Water_002_COLOR.png")->
            addTexture("../textures/Water_001_SPEC.png")->setPosition({-30, -1.5, 30})->compile();


    auto treeObj = objLoader.loadObj("../resources/models/lowpolytree.obj");
    auto trees = getCoordsForVertices(0, 0, 100, 400);
    for (auto &tree:trees) {
        meshes.push_back(new Mesh(treeObj));
        auto scale = random<float>(0.5f, 2.5f);
        if (scale >= 1) {
            meshes.back()->setPosition(
                    {tree.x + random<float>(-40.f, 40.f), tree.y + scale + 1, tree.z + random<float>(-40.f, 40.f)})->
                    setScale({scale, scale, scale})->compile();
        } else {
            meshes.back()->setPosition(
                    {tree.x + random<float>(-40.f, 40.f),
                     tree.y + scale + 0.3, tree.z + random<float>(-40.f, 40.f)})->
                    setScale({scale, scale, scale})->compile();
        }

    }

    // sand to grass blend
    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {60, 60, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend.png")->setPosition({-30, 0, 60})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {60, 60, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend.png")->setPosition({-30, 0, 60})->setRotation(
            {0, 180, 0})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {60, 60, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend.png")->setPosition({-30, 0, 60})->setRotation(
            {0, 90, 0})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {60, 60, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend.png")->setPosition({-30, 0, 60})->setRotation(
            {0, 270, 0})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {30, 30, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend_angle.png")->setPosition({-60, 0, 60})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {30, 30, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend_angle.png")->setPosition({-60, 0, 60})->setRotation(
            {0, 90, 0})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {30, 30, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend_angle.png")->setPosition({-60, 0, 60})->setRotation(
            {0, 180, 0})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {30, 30, 30}, true));
    planes.back()->addTexture("../textures/sand_and_grass_blend_angle.png")->setPosition({-60, 0, 60})->setRotation(
            {0, 270, 0})->compile();

    //grass

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {600, 600, 600}, {8, 24}));
    planes.back()->addTexture("../textures/grass.png")->setPosition({-660, 0, 300})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {600, 600, 600}, {8, 24}));
    planes.back()->addTexture("../textures/grass.png")->setPosition({-660, 0, 300})->setRotation(
            {0, 180, 0})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {120, 600, 400}, {24, 8}));
    planes.back()->addTexture("../textures/grass.png")->setPosition({-60, 0, -60})->compile();

    planes.push_back(new Plane({0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0}, {120, 600, 400}, {24, 8}));
    planes.back()->addTexture("../textures/grass.png")->setPosition({-60, 0, -60})->setRotation({0, 180, 0})->compile();

    auto carsPath = getCoordsForVertices(0, 0, 155, 18000);
    Mesh car("../resources/models/car_1.fbx");
    car.addTexture("../textures/Car Texture 1.png")->setOrigin({carsPath[0].x, 0, carsPath[0].z})->
            setPosition({carsPath[0].x, 0, carsPath[0].z})->setRotation({0, 90, 0})->compile();
    // camera
    camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->setWindowSize(app.getWindow()->getWindowSize());

    glfwSetCursorPosCallback(app.getWindow()->getGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(app.getWindow()->getGLFWWindow(), scroll_callback);
    lightsManager->getLightByNameDir("1_1")->enable();

    int carRot{0};
    float car1R={90};
    while (!app.getShouldClose()) {
        app.getWindow()->updateFpsCounter();
        auto currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        moveCamera();
        Renderer::clear({0, 0, 0, 1});


        camera->passDataToShader(&shader_tex);
        lightsManager->passDataToShader(&shader_tex);
        //plane.draw(&shader_tex);
        for (auto &plane:planes) {
            plane->draw(&shader_tex);
        }
        for (auto &mesh:meshes) {
            mesh->draw(&shader_tex);
        }
        car.draw(&shader_tex);

        glCall(glfwSwapBuffers(app.getWindow()->getGLFWWindow()));
        glfwPollEvents();
        if(carRot==carsPath.size()-1){
            carRot=0;
        }
        carRot++;
        car.setOrigin({carsPath[carRot].x, 0, carsPath[carRot].z})->
                setPosition({carsPath[carRot].x, 0, carsPath[carRot].z})->setRotation({0, car1R, 0});
        car1R-=0.02;
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
