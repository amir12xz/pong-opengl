
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 uPos;
uniform vec2 uSize;

void main()
{
    vec2 scaledPos = aPos * uSize + uPos;
    vec2 ndc = scaledPos / vec2(400.0, 300.0) - 1.0;
    gl_Position = vec4(ndc.x, -ndc.y, 0.0, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main()
{
    FragColor = vec4(uColor, 1.0);
}
)glsl";
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
const float paddleWidth = 20.0f;
const float paddleHeight = 100.0f;
const float ballSize = 20.0f;
float leftPaddleY = 250.0f;
float rightPaddleY = 250.0f;
float ballX = 390.0f, ballY = 290.0f;
float ballSpeedX = 3.0f, ballSpeedY = 2.0f;
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        leftPaddleY -= 5.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        leftPaddleY += 5.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rightPaddleY -= 5.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rightPaddleY += 5.0f;
}
GLuint compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        std::cout << "Shader Compile Error:\n" << info << std::endl;
    }
    return shader;
}

GLuint createShaderProgram()
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetProgramInfoLog(program, 512, NULL, info);
        std::cout << "Shader Linking Error:\n" << info << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}
GLuint VAO, VBO;
void initQuad()
{
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void drawRect(GLuint shader, float x, float y, float w, float h, float r, float g, float b)
{
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader, "uSize"), w, h);
    glUniform3f(glGetUniformLocation(shader, "uColor"), r, g, b);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pong 2D - OpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    GLuint shader = createShaderProgram();
    initQuad();
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        if (ballY <= 0 || ballY + ballSize >= SCR_HEIGHT)
            ballSpeedY = -ballSpeedY;
        if (ballX <= 50 && ballY + ballSize >= leftPaddleY && ballY <= leftPaddleY + paddleHeight)
            ballSpeedX = -ballSpeedX;
        if (ballX + ballSize >= SCR_WIDTH - 50 && ballY + ballSize >= rightPaddleY && ballY <= rightPaddleY + paddleHeight)
            ballSpeedX = -ballSpeedX;
        glClear(GL_COLOR_BUFFER_BIT);
        drawRect(shader, 30.0f, leftPaddleY, paddleWidth, paddleHeight, 1.0f, 1.0f, 1.0f);
        drawRect(shader, SCR_WIDTH - 50.0f, rightPaddleY, paddleWidth, paddleHeight, 1.0f, 1.0f, 1.0f);
        drawRect(shader, ballX, ballY, ballSize, ballSize, 1.0f, 1.0f, 1.0f);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
