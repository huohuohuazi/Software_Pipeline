#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

#include <iostream>
#include<stdio.h>
#include <fstream>


#include <GL/glut.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "stb_image.h"

using namespace std;


void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);//���ɫ�ʻ�����
    glRectf(-0.5f, -0.5f, 0.5f, 0.5f);//����
    glFlush();

}

//��ӡ�豸��Ϣ
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

//���ڴ�С�ı�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    //��ESC�˳�
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main(int argc, char* argv[])
{
#pragma region ��ʼ������
    //��ʼ������
    glutInit(&argc, argv);
    if (!glfwInit())
    {
        cout << "Failed to Initialize GLUT" << endl;
        return -1;
    }
   
    GLFWwindow* window;
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);


    if (!window){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//ע��ص�����

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    DisplayDeviceInfo();

#pragma endregion

#pragma region ָ��VAO VBO ����ɫ��

    
    //������Ϣ
    float vertices[] = {
        //     ---- λ�� ----       ---- ��ɫ ----     - �������� -
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    //�½�VAO VBO����
    unsigned int VBO, VAO, EBO;
    //int index = 1;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //��VBO�����BUUFFER��
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //����shader
    Shader ourShader("./Shaders/simpleTriangle.vert", "./Shaders/simpleTriangle.frag");


    // VAO����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    //����location=0,vec3,type=float,normalize=false,����=3*float,ǿ������ת��
    glEnableVertexAttribArray(0);//���ö�������

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)((3 * sizeof(float))));
    glEnableVertexAttribArray(1);//���ö�������

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    #pragma region Texture1
    // texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("resources/textures/cat.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture1" << std::endl;
    }
    stbi_image_free(data);
#pragma endregion

    #pragma region Texture2
    // texture2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("resources/textures/gongzi.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data);

#pragma endregion

    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);

#pragma endregion

#pragma region ����ѭ��

    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // �ֶ�����
    ourShader.setInt("texture2", 1); // ����ʹ����ɫ��������

    while (!glfwWindowShouldClose(window))
    {
        // ����
        processInput(window);

        // ��Ⱦ
        // �����ɫ����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // ��shader��������
        //ourShader.setFloat("time", glfwGetTime() * 2);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //glDrawArrays(GL_TRIANGLES, 0, 3);//0�Ƕ���������3�Ƕ��㳤

        // �������岢��ѯIO�¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    cout << "Done" << endl;
    return 0;
    
}
