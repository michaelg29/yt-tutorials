#include "scene.h"

unsigned int Scene::scrWidth = 0;
unsigned int Scene::scrHeight = 0;

/*
	callbacks
*/
// window resize
void Scene::framebufferSizeCallback(GLFWwindow* widnow, int width, int height) {
	glViewport(0, 0, width, height);
	Scene::scrWidth = width;
	Scene::scrHeight = height;
}

/*
	constructor
*/
Scene::Scene() {}
Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
	const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor),
	title(title),
	activeCamera(-1) {
	
	Scene::scrWidth = scrWidth;
	Scene::scrHeight = scrHeight;

	setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);
}

/*
	initialization
*/
bool Scene::init() {
	glfwInit();

	// set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// initialize window
	window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
	if (window == NULL) {
		// not created
		return false;
	}
	glfwMakeContextCurrent(window);

	// set GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	// setup screen
	glViewport(0, 0, scrWidth, scrHeight);

	/*
		callbacks
	*/
	// frame size
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	// key pressed
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	// cursor moved
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	// mouse btn pressed
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	// mouse scroll
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	/*
		set rendering parameters
	*/
	glEnable(GL_DEPTH_TEST); // doesn't show vertices not visible to camera (back of object)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

	return true;
}

/*
	main loop methods
*/
// process input
void processInput(float dt);

// update screen before each frame
void update();

// update screen after frame
void newFrame();

// set uniform shader varaibles (lighting, etc)
void render(Shader shader, bool applyLighting = true);

/*
	cleanup method
*/
void cleanup();

/*
	accessors
*/
bool shouldClose();

Camera* getActiveCamera();

/*
	modifiers
*/
void setShouldClose(bool shouldClose);

void setWindowColor(float r, float g, float b, float a);