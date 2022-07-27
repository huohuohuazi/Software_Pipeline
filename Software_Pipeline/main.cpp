#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

#include <iostream>
#include<stdio.h>

#include <GL/glut.h>
#include <GLFW/glfw3.h>


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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    DisplayDeviceInfo();

    glViewport(0, 0, 1280, 720);
    //ע��ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //��ѭ��
    while (!glfwWindowShouldClose(window))//���glfw�Ƿ�Ҫ���˳�
    {
        //����
        processInput(window);

        //��Ⱦָ��
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//�����ɫ


        glClear(GL_COLOR_BUFFER_BIT);//������ɫ����


        //��鲢�����¼�����������
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    cout << "Done" << endl;
    return 0;
    
}
