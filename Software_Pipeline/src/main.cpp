#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "Shader.h"
#include "src/Camera.h"
#include "stb_image.h"
#include "src/VBO.h"


// һЩȫ�ֱ���
float lastX = Width/2, lastY = Height/2;

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);//���ɫ�ʻ�����
    glRectf(-0.5f, -0.5f, 0.5f, 0.5f);//����
    glFlush();

}

// ��ӡ�豸��Ϣ
void DisplayDeviceInfo()
{
    const GLubyte* name = glGetString(GL_VENDOR); //���ظ���ǰOpenGLʵ�ֳ��̵�����
    const GLubyte* biaoshifu = glGetString(GL_RENDERER); //����һ����Ⱦ����ʶ����ͨ���Ǹ�Ӳ��ƽ̨
    const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //���ص�ǰOpenGLʵ�ֵİ汾��
    const GLubyte* gluVersion = gluGetString(GLU_VERSION); //���ص�ǰGLU���߿�汾

    printf("OpenGLʵ�ֳ��̵����֣�%s\n", name);
    printf("��Ⱦ����ʶ����%s\n", biaoshifu);
    printf("OpenGLʵ�ֵİ汾�ţ�%s\n", OpenGLVersion);
    printf("OGLU���߿�汾��%s\n", gluVersion);
}

// ���ڴ�С�ı�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ����ƶ��ص�
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}

// ��껬�ֻص�
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// ��������ص�
void processInput(GLFWwindow* window)
{
    // ��ESC�˳�
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "QuitWindow!" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }

    // ����ƶ�
    float cameraSpeed = 2.5f * deltaTime; // �ƶ��ٶ�

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


int main(int argc, char* argv[])
{
    #pragma region ��ʼ������

    //��ʼ������
    glutInit(&argc, argv);
    if (!glfwInit())
    {
        std::cout << "Failed to Initialize GLUT" << endl;
        return -1;
    }
   
    GLFWwindow* window;
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);


    if (!window){
        std::cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);// ���ڴ�С�ı�ص�����
    glfwSetCursorPosCallback(window, mouse_callback);// ����ƶ��ص�����
    glfwSetScrollCallback(window, scroll_callback);// �����ֻص�����
    // ���ع��
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    DisplayDeviceInfo();

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    

#pragma endregion

    #pragma region ָ��Shader

    // ����shader
    Shader lightingShader("Shaders/simpleColor.vert", "Shaders/simpleColor.frag");
    Shader lightCubeShader("Shaders/simpleLight.vert", "Shaders/simpleLight.frag");
#pragma endregion

    #pragma region ģ����Ϣ
    //������Ϣ
    float vertices[] = {
       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,

       -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f, -0.5f,  0.5f,

       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
       -0.5f, -0.5f,  0.5f,
       -0.5f, -0.5f, -0.5f,

       -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f,
    };
    //float* vertice_prt = vertices;
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);//��Դλ��

#pragma endregion

    

    #pragma region VBO_
 
    unsigned int VBO, cubeVAO, lightCubeVAO;
    VBOmanager vbo(&VBO);

    vbo.addVAO(&cubeVAO);
    vbo.addStaticBuffer(VBO, vertices, sizeof(vertices));
    vbo.BindVAO(cubeVAO);
    vbo.addVertex(3, vector<int>{3});

    vbo.addVAO(&lightCubeVAO);
    vbo.BindVAO(lightCubeVAO);
    vbo.addStaticBuffer(VBO);
    vbo.addVertex(3, vector<int>{3});

    //std::cout << "VBO:" << VBO << ",cubeVAO:" << cubeVAO << ",lightCubeVAO:" << lightCubeVAO << std::endl;
    //std::cout << "VBO:" << sizeof(VBO) << ",cubeVAO:" << sizeof(cubeVAO) << ",lightCubeVAO:" << sizeof(lightCubeVAO) << std::endl;


#pragma endregion

    #pragma region Texture1

        /*
    // texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);

    //// Ĭ��Ϊ ����������������� ѭ������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ����ͼƬ������mipmap
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("resources/textures/cat.jpg", &width, &height, &nrChannels, 0);
    cout << "height:" << height << ",width:" << width << ",nrchannels:" << nrChannels << endl;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // target=TEXTURE_2D,mipmap_level=0,rgba=rgb,width&height,===0,source_type=RGB,source_code=byte,data
        
        // �������Ҫ���ɶ༶mipmap,����Ҫ���ö��mipmap_level�����ϳ�
        // glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        cout << "Read Texture1 SUCCESS!" << endl;
    }
    else
    {
        cout << "Failed to load texture1" << endl;
    }
    stbi_image_free(data);

#pragma endregion

    #pragma region Texture2

    // texture2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // �������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ����ͼƬ������mipmap
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("resources/textures/ganyu.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        cout << "Read Texture2 SUCCESS!" << endl;
    }
    else
    {
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data);
    */



#pragma endregion

    //ourShader.use();
    // ����֮ǰ�ǵ�use()
    //ourShader.setInt("texture1", 0);
    //ourShader.setInt("texture2", 1);


#pragma region ����ѭ��

    std::cout << "BeginToLoop:" << endl;

    while (!glfwWindowShouldClose(window))
    {
        // �ȼ���deltatime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ����
        processInput(window);

        // ��Ⱦ
        // �����ɫ��������Ȼ���
        glClearColor(0.0f, 0.7f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);
        glm::mat4 view = camera.GetView();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // �������岢��ѯIO�¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

#pragma endregion

    // �ͷ���Դ
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    std::cout << "Done" << endl;
    return 0;
    
}
