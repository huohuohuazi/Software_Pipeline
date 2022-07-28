#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

#include <iostream>
#include<stdio.h>

#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include"Shader.h"

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
        // λ��              // ��ɫ
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ����
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ����
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ����
    };

    //�½�VAO VBO����
    unsigned int VBO, VAO;
    int index = 1;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //��VBO�����BUUFFER��
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //����shader
    Shader ourShader("./Shaders/simpleTriangle.vert", "./Shaders/simpleTriangle.frag");


    // VAO����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //����location=0,vec3,type=float,normalize=false,����=3*float,ǿ������ת��
    glEnableVertexAttribArray(0);//���ö�������

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)((3 * sizeof(float))));
    glEnableVertexAttribArray(1);//���ö�������

#pragma endregion

#pragma region ����ѭ��
    while (!glfwWindowShouldClose(window))
    {
        // ����
        processInput(window);

        // ��Ⱦ
        // �����ɫ����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // ����uniform��ɫ
        //float timeValue = glfwGetTime();
        //float greenValue = sin(timeValue) / 2.0f + 0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // ��shader��������
        ourShader.setFloat("time", glfwGetTime() * 2);
        glBindVertexArray(VAO);

        ourShader.use();

        glDrawArrays(GL_TRIANGLES, 0, 3);//0�Ƕ���������3�Ƕ��㳤

        // �������岢��ѯIO�¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion

    // �ͷ���Դ
    glDeleteVertexArrays(index, &VAO);
    glDeleteBuffers(index, &VBO);


    glfwTerminate();
    cout << "Done" << endl;
    return 0;
    
}
