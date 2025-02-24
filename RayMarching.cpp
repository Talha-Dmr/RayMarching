#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

const int width = 1500;
const int height = 1080;

// GLSL Vertex Shader
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 fragCoord;
    void main() {
        fragCoord = aPos * 0.5 + 0.5; // [-1,1] -> [0,1]
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

// GLSL Fragment Shader  
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 fragCoord;

    uniform vec2 iResolution;
    uniform vec2 iMouse;
    uniform float iTime;

    mat2 rot2D(float a) {
        return mat2(cos(a), -sin(a), sin(a), cos(a));
    }

    vec3 palette(float t) {
        return 0.5 + 0.5 * cos(6.28318 * (t + vec3(0.1, 0.3, 0.6)));
    }

    float sdCube(vec3 p, vec3 b) {
        vec3 d = abs(p) - b;
        return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
    }

    float map(vec3 p) {
        p.z += iTime * 0.4;
        p.xy = fract(p.xy) - 0.5;
        p.z  = mod(p.z, 0.25) - 0.125;

        return sdCube(p, vec3(0.1));
    }

    void main() {
        vec2 uv = (fragCoord * 2.0 - iResolution.xy) / iResolution.y;
        vec3 ro = vec3(0.0, 0.0, -2.0);
        vec3 rd = normalize(vec3(uv, 1));

        float t = 0.0;
        for (int i = 0; i < 100; i++) {
            vec3 p = ro + rd * t;
            float d = map(p);
            t += d;
            if (d < 0.001) break;
        }

        vec3 color = palette(t * 0.1);
        FragColor = vec4(color, 1.0);
    }
)";

void checkShaderCompilation(GLuint shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
    }
}

void checkProgramLinking(GLuint program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader linking error: " << infoLog << std::endl;
    }
}

int main() {
    // GLFW Init
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "RayMarching Shader", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // Create Vertex Shader  
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader);

    // Create Fragment Shader  
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader);

    // Create and connect shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinking(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Fullscreen Quad (2D Triangle Strip)
    float vertices[] = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int resolutionLoc = glGetUniformLocation(shaderProgram, "iResolution");
    int timeLoc = glGetUniformLocation(shaderProgram, "iTime");
    int mouseLoc = glGetUniformLocation(shaderProgram, "iMouse");

    // OpenGL Render Loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Update uniforms
        glUniform2f(resolutionLoc, (float)width, (float)height);
        glUniform1f(timeLoc, (float)glfwGetTime());

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glUniform2f(mouseLoc, (float)mouseX, (float)(height - mouseY));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
