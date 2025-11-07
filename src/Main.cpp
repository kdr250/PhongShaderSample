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

    // create window
    window = glfwCreateWindow(640, 480, "PhongShaderSample", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // specify version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

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
