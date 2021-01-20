
#include "application.hpp"
#include "camera.hpp"
#include "diffuse_light.hpp"
#include "lights_manager.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shader.hpp"

LightsManager *lightsManager;
float lastX = 0;
float lastY = 0;
bool firstMouse = true;
// timing
double deltaTime = 0.0f;// time between current frame and last frame
double lastFrame = 0.0f;
Camera *camera;

void programQuit([[maybe_unused]] int key, [[maybe_unused]] int action, Application *app) {
  app->close();
  LOG_S(INFO) << "Quiting...";
}

void wasdKeyPress([[maybe_unused]] int key, [[maybe_unused]] int action, [[maybe_unused]] Application *app) {
  if ((key == GLFW_KEY_W) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	camera->ProcessKeyboard(FORWARD, (float)deltaTime);
  if ((key == GLFW_KEY_S) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	camera->ProcessKeyboard(BACKWARD, (float)deltaTime);
  if ((key == GLFW_KEY_A) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	camera->ProcessKeyboard(LEFT, (float)deltaTime);
  if ((key == GLFW_KEY_D) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	camera->ProcessKeyboard(RIGHT, (float)deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
	lastX = (float)xpos;
	lastY = (float)ypos;
	firstMouse = false;
  }

  double xoffset = xpos - lastX;
  double yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top

  lastX = (float)xpos;
  lastY = (float)ypos;

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

  Shader shader_tex("../shaders/multiple_diffuse_shader_tex.glsl", false);
  shader_tex.bind();
  shader_tex.setUniform1i("u_Texture", 0);
  shader_tex.setUniform1i("numDiffLights", 1);
  std::vector<Mesh *> meshes;

  //базовая высота -2.5
  Texture floor_metal_tex("../textures/floor/flat-floor4_8.png");
  Texture floor_blue_tex("../textures/floor/flat-flat14.png");
  Texture pillar_brown_tex("../textures/brown1.png");

  Mesh cube("../resources/models/Crate1.obj");

  for (int i = 0; i < 10; ++i) {
	for (int j = 0; j < 13; ++j) {
	  if ((i == 3 || i == 8) && (j == 3 || j == 7)) {
		if (i == 3) {
		  meshes.emplace_back(new Mesh(cube.loadedOBJ));
		  meshes.back()->setTextures({&pillar_brown_tex})->setPosition({(i * 2)-0.5, -1.2, (j * 2) + 1.5})->setScale({0.5, 1, 0.5})->compile();
		  meshes.emplace_back(new Mesh(cube.loadedOBJ));
		  meshes.back()->setTextures({&pillar_brown_tex})->setPosition({(i * 2)-0.5, 0.8, (j * 2) + 1.5})->setScale({0.5, 1, 0.5})->compile();
		} else {
		  meshes.emplace_back(new Mesh(cube.loadedOBJ));
		  meshes.back()->setTextures({&pillar_brown_tex})->setPosition({(i * 2) - 0.25, -1.2, (j * 2) + 0.25})->setScale({0.5, 1, 0.5})->compile();
		  meshes.emplace_back(new Mesh(cube.loadedOBJ));
		  meshes.back()->setTextures({&pillar_brown_tex})->setPosition({(i * 2) - 0.25, 0.8, (j * 2) + 0.25})->setScale({0.5, 1, 0.5})->compile();
		}
	  }
	  meshes.emplace_back(new Mesh(cube.loadedOBJ));
	  if (i >= 3 && i <= 7 && j >= 4 && j <= 9) {
		meshes.back()->setTextures({&floor_blue_tex})->setPosition({i * 2, -2.8, j * 2})->setScale({1, 0.01, 1})->compile();

	  } else {
		meshes.back()->setTextures({&floor_metal_tex})->setPosition({i * 2, -2.5, j * 2})->setScale({1, 0.3, 1})->compile();
	  }
	}
  }

  lightsManager = new LightsManager;
  lightsManager->addLight(DiffuseLight("1_1", {{0, 10, 0}, {0.8, 0.8, 0.8}, 0.8}));

  // camera
  camera = new Camera(glm::vec3(0.0f, 0.0f, 6.0f));
  camera->setWindowSize(app.getWindow()->getWindowSize());

  glfwSetCursorPosCallback(app.getWindow()->getGLFWWindow(), mouse_callback);
  glfwSetScrollCallback(app.getWindow()->getGLFWWindow(), scroll_callback);
  lightsManager->getLightByNameDir("1_1")->enable();

  while (!app.getShouldClose()) {
	app.getWindow()->updateFpsCounter();
	auto currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	//updating data for shader
	shader_tex.reload();
	Renderer::clear({0, 0, 0, 1});

	camera->passDataToShader(&shader_tex);
	lightsManager->passDataToShader(&shader_tex);
	for (auto &mesh : meshes) {
	  mesh->draw(&shader_tex);
	}

	glCall(glfwSwapBuffers(app.getWindow()->getGLFWWindow()));
	glfwPollEvents();
  }
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
