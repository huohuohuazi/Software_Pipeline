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

    //#define STB_IMAGE_IMPLEMENTATION

    #include "Shader.h"
    #include "Model.h"
    #include "Camera.h"
    #include "VBO.h"
    //#include "stb_image.h"
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
    
        // ͼƬ���뷴תy��
        stbi_set_flip_vertically_on_load(true);

#pragma endregion



    #pragma region ָ��Shader

        // ����shader
        //Shader cubeShader("Shaders/Blinn_Phong.vert", "Shaders/Blinn_Phong.frag");
        Shader cubeShader("Shaders/simpleTexture.vert", "Shaders/simpleTexture.frag");
        Shader lightShader("Shaders/simpleLight.vert", "Shaders/simpleLight.frag");
        Shader modelShader("Shaders/simpleModel.vert", "Shaders/simpleModel.frag");

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


        //Model nanosuitModel("resources/objects/nanosuit/nanosuit.obj");
         Model nanosuitModel("resources/objects/chair/exp.obj");

        // positions all containers
        glm::vec3 cubePositions[] = {
       glm::vec3(0.0f,  0.0f,  0.0f),
       glm::vec3(2.0f,  5.0f, -15.0f),
       glm::vec3(-1.5f, -2.2f, -2.5f),
       glm::vec3(-3.8f, -2.0f, -12.3f),
       glm::vec3(2.4f, -0.4f, -3.5f),
       glm::vec3(-1.7f,  3.0f, -7.5f),
       glm::vec3(1.3f, -2.0f, -2.5f),
       glm::vec3(1.5f,  2.0f, -2.5f),
       glm::vec3(1.5f,  0.2f, -1.5f),
       glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        // positions of the point lights
        glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f,  0.2f,  2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f)
        };

        unsigned int diffuseMap = LoadTexture("resources/textures/container2.png");
        unsigned int specularMap = LoadTexture("resources/textures/container2_specular.png");
    
    #pragma endregion

        unsigned int VBO, cubeVAO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        cubeShader.use();
        cubeShader.setInt("material.diffuse", 0);
        cubeShader.setInt("material.specular", 1);

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
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ����shader����
            cubeShader.use();
        
            cubeShader.setVec3("viewPos", camera.Position);
            cubeShader.setFloat("material.shininess", 32.0f);

            // MVP����M��transform�任
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetView();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);

            cubeShader.setMat4("model", model);
            cubeShader.setMat4("view", view);
            cubeShader.setMat4("projection", projection);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

#pragma region Lighting
            cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
            cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
            cubeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
            cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
            // point light 1
            cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
            cubeShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
            cubeShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
            cubeShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
            cubeShader.setFloat("pointLights[0].constant", 1.0f);
            cubeShader.setFloat("pointLights[0].linear", 0.09f);
            cubeShader.setFloat("pointLights[0].quadratic", 0.032f);
            // point light 2
            cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
            cubeShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
            cubeShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
            cubeShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
            cubeShader.setFloat("pointLights[1].constant", 1.0f);
            cubeShader.setFloat("pointLights[1].linear", 0.09f);
            cubeShader.setFloat("pointLights[1].quadratic", 0.032f);
            // point light 3
            cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
            cubeShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
            cubeShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
            cubeShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
            cubeShader.setFloat("pointLights[2].constant", 1.0f);
            cubeShader.setFloat("pointLights[2].linear", 0.09f);
            cubeShader.setFloat("pointLights[2].quadratic", 0.032f);
            // point light 4
            cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
            cubeShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
            cubeShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
            cubeShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
            cubeShader.setFloat("pointLights[3].constant", 1.0f);
            cubeShader.setFloat("pointLights[3].linear", 0.09f);
            cubeShader.setFloat("pointLights[3].quadratic", 0.032f);
            // spotLight
            cubeShader.setVec3("spotLight.position", camera.Position);
            cubeShader.setVec3("spotLight.direction", camera.Front);
            cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            cubeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            cubeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            cubeShader.setFloat("spotLight.constant", 1.0f);
            cubeShader.setFloat("spotLight.linear", 0.09f);
            cubeShader.setFloat("spotLight.quadratic", 0.032f);
            cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            cubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

#pragma endregion


            glBindVertexArray(cubeVAO);
            for (unsigned int i = 0; i < 10; i++)
            {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                cubeShader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }




            // ʹ��shader����ģ��
            modelShader.use();
            modelShader.setMat4("model", model);
            modelShader.setMat4("view", view);
            modelShader.setMat4("projection", projection);
            nanosuitModel.Draw(modelShader);



            // �������岢��ѯIO�¼�
            glfwSwapBuffers(window);
            glfwPollEvents();
        };

    #pragma endregion



    // �ͷ���Դ
    //nanosuitModel.Release();



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
