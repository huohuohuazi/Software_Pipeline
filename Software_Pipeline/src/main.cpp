#pragma region INCLUDE

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
    //#include "src/Texture.h"

#pragma endregion


#pragma region ��������

    void DisplayCurrentState();
    void DisplayDeviceInfo();
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void processInput(GLFWwindow* window);
    unsigned int LoadTexture(char const* path);

#pragma endregion


#pragma region ��������

    float lastX = Width / 2, lastY = Height / 2;

    float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    float lastFrame = 0.0f; // ��һ֡��ʱ��

    bool firstMouse = true;
    int pressTime = 0;

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

#pragma endregion





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
    //Shader cubeShader("Shaders/Blinn_Phong.vert", "Shaders/Blinn_Phong.frag");
    Shader cubeShader("Shaders/simpleTexture.vert", "Shaders/simpleTexture.frag");
    Shader lightShader("Shaders/simpleLight.vert", "Shaders/simpleLight.frag");

    #pragma endregion



    #pragma region ģ����Ϣ

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // ��Դλ��
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);//��Դλ��

    #pragma endregion



    #pragma region ����VBO
 /*
    unsigned int VBO, cubeVAO, lightVAO;
    VBOmanager vbo(&VBO);

    // �����������
    vbo.addVAO(&cubeVAO);
    vbo.addStaticBuffer(VBO, vertices, sizeof(vertices));
    vbo.BindVAO(cubeVAO);
    vbo.addVertex(8, vector<int>{3,3,2});


*/
    unsigned int VBO, cubeVAO, lightVAO;
    VBOmanager vbo(&VBO);

    vbo.addVAO(&cubeVAO);
    vbo.addStaticBuffer(VBO, vertices, sizeof(vertices));
    vbo.BindVAO(cubeVAO);
    vbo.addVertex(8, vector<int>{3, 3, 2});

    
    // ��Դ
    vbo.addVAO(&lightVAO);
    vbo.addStaticBuffer(VBO);
    vbo.BindVAO(lightVAO);
    vbo.addVertex(8, vector<int>{3});

    //Texture container2("resources / textures / container2.png");
    unsigned int diffuseMap_container2 = LoadTexture("resources/textures/container2.png");
    //unsigned int diffuseMap_container2 = container2.textureID;
    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);

#pragma endregion

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������shader
        cubeShader.use();
        cubeShader.setVec3("viewPos", camera.Position);

        cubeShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        cubeShader.setFloat("material.shininess", 64.0f);

        cubeShader.setVec3("light.position", lightPos);
        cubeShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        cubeShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // �����յ�����һЩ�Դ��䳡��
        cubeShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);// �߹ⷴ��Ĭ��Ϊ��

        // MVP����
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);

        cubeShader.setMat4("model", model);
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap_container2);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // �ƹ�shader
        lightShader.use();

        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // �������岢��ѯIO�¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

#pragma endregion

    // �ͷ���Դ
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    std::cout << "Done" << endl;
    return 0;
    
}

// �����ǰ��Ϣ
void DisplayCurrentState()
{
    if (pressTime==0)
    {
        camera.PrintState();
        pressTime++;
    }
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
        DisplayCurrentState();
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
        pressTime = 0;
}

// ��ȡͼƬ������ID
unsigned int LoadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)// �Ҷ�ͼ
            format = GL_RED;
        else if (nrComponents == 3)// RGBͼ
            format = GL_RGB;
        else if (nrComponents == 4)// RGBAͼ
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Load Texture SUCCESS! " << path << std::endl;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}



