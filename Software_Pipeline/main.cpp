#include <glad/glad.h>//要放在所有头文件之前

#include <iostream>
#include<stdio.h>

#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include"Shader.h"

using namespace std;


void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);//清空色彩缓存区
    glRectf(-0.5f, -0.5f, 0.5f, 0.5f);//绘制
    glFlush();

}

//打印设备信息
void DisplayDeviceInfo()
{
    const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
    const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
    const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
    const GLubyte* gluVersion = gluGetString(GLU_VERSION); //返回当前GLU工具库版本

    printf("OpenGL实现厂商的名字：%s\n", name);
    printf("渲染器标识符：%s\n", biaoshifu);
    printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
    printf("OGLU工具库版本：%s\n", gluVersion);
}

//窗口大小改变的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    //按ESC退出
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main(int argc, char* argv[])
{
#pragma region 初始化窗口
    //初始化窗口
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//注册回调函数

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    DisplayDeviceInfo();

#pragma endregion

#pragma region 指定VAO VBO 与着色器

    
    //绘制信息
    float vertices[] = {
        // 位置              // 颜色
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    };

    //新建VAO VBO对象
    unsigned int VBO, VAO;
    int index = 1;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //将VBO添加至BUUFFER中
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //加载shader
    Shader ourShader("./Shaders/simpleTriangle.vert", "./Shaders/simpleTriangle.frag");


    // VAO设置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //对于location=0,vec3,type=float,normalize=false,补偿=3*float,强制类型转换
    glEnableVertexAttribArray(0);//启用顶点属性

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)((3 * sizeof(float))));
    glEnableVertexAttribArray(1);//启用顶点属性

#pragma endregion

#pragma region 绘制循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        // 渲染
        // 清除颜色缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // 更新uniform颜色
        //float timeValue = glfwGetTime();
        //float greenValue = sin(timeValue) / 2.0f + 0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // 给shader传入数据
        ourShader.setFloat("time", glfwGetTime() * 2);
        glBindVertexArray(VAO);

        ourShader.use();

        glDrawArrays(GL_TRIANGLES, 0, 3);//0是顶点索引，3是顶点长

        // 交换缓冲并查询IO事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion

    // 释放资源
    glDeleteVertexArrays(index, &VAO);
    glDeleteBuffers(index, &VBO);


    glfwTerminate();
    cout << "Done" << endl;
    return 0;
    
}
