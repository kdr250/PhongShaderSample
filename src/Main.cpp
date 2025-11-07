#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
    // initialize glfw
    if (glfwInit() == GL_FALSE) {
        return EXIT_FAILURE;
    }

    // create window
    GLFWwindow* window = glfwCreateWindow(640, 480, "PhongShaderSample", NULL, NULL);
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

    // game loop
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // clear buffer
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // swap double buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate glfw
    glfwTerminate();

    return EXIT_SUCCESS;
}
