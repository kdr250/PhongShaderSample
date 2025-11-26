#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr float ASPECT = (float)WIDTH / (float)HEIGHT;

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
        // positions          // normals           
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f 
    };
    // clang-format on

    // Create vertex array
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Create vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Specify the vertex attributes
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * 6,
                          reinterpret_cast<void*>(sizeof(float) * 3));

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
#ifdef __EMSCRIPTEN__
        sstream << "#version 300 es" << std::endl << "// ";
#endif
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

void setUniformVec3(const std::string& name, const glm::vec3& value) {
    auto location = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void setUniformMatrix4(const std::string& name, const glm::mat4& matrix) {
    auto location = glGetUniformLocation(shaderProgram, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw
    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArray);

    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime)
            .count();

    auto model =
        glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
    auto view = glm::lookAt(
        cameraPos, cameraPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    auto projection = glm::perspective(glm::radians(70.0f), ASPECT, 0.1f, 1000.0f);

    setUniformMatrix4("uModel", model);
    setUniformMatrix4("uView", view);
    setUniformMatrix4("uProjection", projection);
    setUniformVec3("uCameraPos", cameraPos);

    glDrawArrays(GL_TRIANGLES, 0, 36);

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
    window = glfwCreateWindow(WIDTH, HEIGHT, "PhongShaderSample", NULL, NULL);
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

    glEnable(GL_DEPTH_TEST);

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
