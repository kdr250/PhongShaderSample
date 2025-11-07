#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

GLFWwindow* window = nullptr;

unsigned int vertexArray = 0;
unsigned int vertexBuffer = 0;
unsigned int indexBuffer = 0;
GLuint shaderProgram = 0;
GLuint vertexShader = 0;
GLuint fragShader = 0;

bool createVertexArray() {
    // clang-format off
    float vertices[] = {
        -0.5f, 0.5f,  0.0f, // top left
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f // bottom left
    };
    // clang-format on
    unsigned int numVerts = 4;
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    unsigned int numIndices = 6;

    // Create vertex array
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Create vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVerts * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    // Create index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Specify the vertex attributes
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    return true;
}

bool isCompiled(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    char buffer[512];
    memset(buffer, 0, 512);
    glGetShaderInfoLog(shader, 511, nullptr, buffer);
    std::cout << "compile GLSL status: " << buffer << std::endl;
    return status == GL_TRUE;
}

bool compileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader) {
    // Open file
    std::ifstream shaderFile(fileName);
    if (shaderFile.is_open()) {
        // Read all the text into a string
        std::stringstream sstream;
        sstream << shaderFile.rdbuf();
        std::string contents = sstream.str();
        const char* contentsChar = contents.c_str();

        // Create a shader of the specified type
        outShader = glCreateShader(shaderType);

        // Set the source characters and try to compile
        glShaderSource(outShader, 1, &contentsChar, nullptr);
        glCompileShader(outShader);

        if (!isCompiled(outShader)) {
            std::cout << "Failed to comple shader " << fileName << std::endl;
            return false;
        }
    } else {
        std::cout << "Shader file not found: " << fileName << std::endl;
        return false;
    }

    return true;
}

bool isValidShader() {
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    char buffer[512];
    memset(buffer, 0, 512);
    glGetProgramInfoLog(shaderProgram, 511, nullptr, buffer);
    std::cout << "GLSL Link status: " << buffer << std::endl;
    return status == GL_TRUE;
}

bool loadShaders(const std::string& vertName, const std::string& fragName) {
    // Compile vertex and pixel shaders
    if (!compileShader(vertName, GL_VERTEX_SHADER, vertexShader) ||
        !compileShader(fragName, GL_FRAGMENT_SHADER, fragShader)) {
        return false;
    }

    // Now create a shader program that links together the vertex/frag shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Verify that the program linked successfully
    if (!isValidShader()) {
        return false;
    }
    return true;
}

void shutdown() {
#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
#endif
    // Delete the program and shaders
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    // Delete vertex array
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    // Terminate glfw
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw
    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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

    if (!loadShaders("resources/Sample.vert", "resources/Sample.frag")) {
        std::cout << "Failed to load shaders" << std::endl;
        return EXIT_FAILURE;
    }

    if (!createVertexArray()) {
        std::cout << "Failed to create vertex array" << std::endl;
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
