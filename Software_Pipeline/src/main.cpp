#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

#include <iostream>
#include<stdio.h>
#include <fstream>


#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

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

    //������Ȳ���
    glEnable(GL_DEPTH_TEST);


#pragma endregion


    #pragma region ģ����Ϣ
    //������Ϣ
    float vertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

#pragma endregion


    #pragma region ָ��Shader

    // ����shader
    Shader ourShader("./Shaders/simpleTriangle.vert", "./Shaders/simpleTriangle.frag");

#pragma endregion


    #pragma region VBO

    // �½�VAO VBO����
    unsigned int VBO, VAO, EBO;
    //int index = 1;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // ��������������BUUFFER��
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // VAO����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //����location=0,vec3,type=float,normalize=false,����=3*float,ǿ������ת��
    glEnableVertexAttribArray(0);//���ö�������

    // ������Ϣ
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

#pragma endregion

    #pragma region Texture1

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

#pragma endregion

    ourShader.use();
    // ����֮ǰ�ǵ�use()
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);



#pragma region ����ѭ��

    while (!glfwWindowShouldClose(window))
    {
        // ����
        processInput(window);

        // ��Ⱦ
        // �����ɫ����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//��������Ȳ���

        
        // ����
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

#pragma region ͶӰ����

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)1280 / (float)720, 0.1f, 100.0f);

#pragma endregion

        // �任-���任���󴫵�����ɫ��
        ourShader.use();

        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        ourShader.setMat4("projection", projection);


        // ���ö�����Ϣ����������
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);//0�Ƕ���������36�Ƕ�����Ŀ

        // �������岢��ѯIO�¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#pragma endregion

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    glfwTerminate();
    cout << "Done" << endl;
    return 0;
    
}
