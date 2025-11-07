#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

GLFWwindow* window = nullptr;

void shutdown() {
#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
#endif
    glfwDestroyWindow(window);
    glfwTerminate();
}

void mainLoop() {
#ifdef __EMSCRIPTEN__
    if (glfwWindowShouldClose(window) != GL_FALSE) {
        shutdown();
        return;
    }
#endif

    // clear buffer
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // swap double buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    // initialize glfw
    if (glfwInit() == GL_FALSE) {
        return EXIT_FAILURE;
    }

    // specify OpengGL version
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto display = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_REFRESH_RATE, display->refreshRate);
    glfwWindowHint(GLFW_RED_BITS, display->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, display->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, display->blueBits);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // create window
    window = glfwCreateWindow(640, 480, "PhongShaderSample", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // create context
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // initialize glew
    if (glewInit() != GLEW_OK) {
        return EXIT_FAILURE;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        mainLoop();
    }

    shutdown();
#endif

    return EXIT_SUCCESS;
}
