#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../includes/shader.h"
#include "../includes/stb_image.h"

#include <iostream>
#include <string>
#include <math.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCALL(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLog(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

static bool GLLog(const char* func, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout << "ERROR::" + error << std::endl;
        return false;
    }
    return true;
}

// prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main(int argc, char **argv)
{
    // glfw: initialize and configure - for Windows OS
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("src/vertex.shader", "src/fragment.shader");

    /******************** Data preparation ********************/
    /**********************************************************/
    // Set up vertex data and configure attributes.
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    // generate 1 vertex buffer by generating its unique reference ID
    unsigned int VBOs, VAOs, EBO;
    glGenVertexArrays(1, &VAOs);
    glGenBuffers(1, &VBOs);
    glGenBuffers(1, &EBO);
        
    // bind the vertex buffer type and load the vertices into the allocated bound buffer in the GPUs memory
    GLCALL(glBindVertexArray(VAOs));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBOs));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // link vertex input data to vertex shader attributes
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0));
    GLCALL(glEnableVertexAttribArray(0));
    GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))));
    GLCALL(glEnableVertexAttribArray(1));
    GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float))));
    GLCALL(glEnableVertexAttribArray(2));

    // // unbind vertex buffer and vertex array because we already binded and "registered them"
    // GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    // GLCALL(glBindVertexArray(0));

    /**************** Texture generation ****************/
    unsigned int texture;
    GLCALL(glGenTextures(1, &texture));
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // load texture
    int width, height, nChannels;
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nChannels, 0);
    if(data)
    {
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // clearing the window to the specified color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
        // draw call
        shader.use();
        shader.setInt("ourTexture", 0);

        glBindVertexArray(VAOs); // not needed to bind every frame since we have only one
        // GLCALL(glDrawArrays(GL_TRIANGLES, 0, 3));

        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}