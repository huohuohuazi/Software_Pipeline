#pragma region INCLUDE

    #include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

    #include <iostream>
    #include <stdio.h>
    #include <fstream>

    using namespace std;

    
    //#include <GL/glut.h>
    #include <gl/glut.h>
    #include <gl/GL.h>
    //#include <GLEW/GL/glew.h>

    #include <GLFW/glfw3.h>
    #include <GLM/glm.hpp>
    #include <GLM/gtc/matrix_transform.hpp>
    #include <GLM/gtc/type_ptr.hpp>

    //#define STB_IMAGE_IMPLEMENTATION

    #include "Shader.h"
    #include "Model.h"
    #include "Camera.h"
    #include "VBO.h"
    //#include "Texture.h"
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
    void DrawOutline(Shader ourlineShader, int step, float& scale, float targetScale = 1.1);
    // void DrawOutline(Shader ourlineShader, int step, float& scale);
    unsigned int CreateEmptyTexture();
    unsigned int LoadCubeTexture(vector<string> textures_faces);

#pragma endregion


#pragma region ��������

    float lastX = Width / 2, lastY = Height / 2;

    float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    float lastFrame = 0.0f; // ��һ֡��ʱ��

    bool firstMouse = true;
    int pressTime = 0;

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
    #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


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
        window = glfwCreateWindow(Width, Height, "Hello World", NULL, NULL);


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


    #pragma region ��Ȼ���

        // ������Ȳ���,�󲿷���Ȼ���ľ��ȶ���24λ��
        glEnable(GL_DEPTH_TEST);// ��ͨ����Ȳ��ԣ�������Ȼ����������Ƭ��/���ص����
        // ��������

        glDepthFunc(GL_LESS);// ������Ȳ��Եķ���
        // GL_LESS	��Ƭ�����ֵС�ڻ�������ֵʱͨ������
        // GL_GREATER	��Ƭ�����ֵ���ڻ����������ֵʱͨ������
        // ����GL_ALWAYS / GL_NEVER / GL_EQUAL / GL_LEQUAL

        // ������Ȼ�����£���������Ϊfalse�������ڲ�ϣ��������Ȼ�������
       // glDepthMask(GL_FALSE);

    #pragma endregion


    #pragma region ģ�建��       

        // ģ�建��
        glEnable(GL_STENCIL_TEST);

        // ����ǽ���һ��ģ����ԣ����趨��������бȽϣ������Զ��崦��ķ���
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // 1Ϊ�ο�ֵ�����Ƚϵ�����
        // 0xFFΪ���룬�����òο�ֵ�Ƚ�ǰ������(AND)���㣬Ĭ��Ϊȫ1

        // �������ȾʱҪ���е�����
        //glStencilMask(0xFF);

        // �涨������Ϊ
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // glStencilOp(GLenum sfail, GLenum dpfail,  GLenum dppass);
        // sfail��ģ�����ʧ��ʱ;dpfail:ģ�����ͨ������Ȳ���ʧ�ܣ� dppass����ͨ��
        // ������GL_KEEP ���ֵ�ǰ��ģ��ֵ / GL_ZERO ��Ϊ0 / GL_REPLACE �滻Ϊref /  GL_INCR ģ��ֵ��1�ȵ�


    #pragma endregion


    #pragma region Alpha����

        glEnable(GL_BLEND);

        // ���ѡ��
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    #pragma endregion


    #pragma region ���޳� CULL

        // ���޳��ˣ��ذ��е�����() ����ģ��û������

        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CW);

    #pragma endregion


        // ͼƬ���뷴תy��
        stbi_set_flip_vertically_on_load(false);
        
#pragma endregion

    #pragma region Unifrom��

        unsigned int VPMatricxBlock;
        glGenBuffers(1, &VPMatricxBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);
        glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW); // ����152�ֽڵ��ڴ�
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // ���ӵ��󶨵�1
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, VPMatricxBlock);

    #pragma endregion

    #pragma region FBO

        // FrameBuffer Object
        unsigned int FBO;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        unsigned int texColorBuffer = CreateEmptyTexture();

        // ��Texture�󶨵�FBO��,Texture��16λ��
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        // RenderBuffer Object��Ⱦ�������
        unsigned int RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);

        // �ڲ���ʽΪGL_DEPTH24_STENCIL8��Depth��24λ��,Stencil��8λ��
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    #pragma endregion





    #pragma region ����Shader

        // ����shader
        Shader outlineShader("Shaders/DebugShader/StencilTest.vert", "Shaders/DebugShader/StencilTest.frag");
        Shader cubeShader("Shaders/DebugShader/BaseCube.vert", "Shaders/DebugShader/BaseCube.frag");
        Shader AlphaShader("Shaders/DebugShader/AlphaTest.vert", "Shaders/DebugShader/AlphaTest.frag");
        Shader ReflectionShader("Shaders/DebugShader/ReflectEnvironment.vert", "Shaders/DebugShader/ReflectEnvironment.frag");
        Shader RefractionShader("Shaders/DebugShader/ReflectEnvironment.vert", "Shaders/DebugShader/RefractionEnvironment.frag");
        // ���������Ĺ��캯�������һ���Ǽ�����ɫ��·��
        //Shader PointsShader("Shaders/DebugShader/Geometry/GeometryTest.vert", "Shaders/DebugShader/Geometry/GeometryTest.frag","Shaders/DebugShader/Geometry/GeometryTest.geo");
        //Shader ShowNormalShader("Shaders/DebugShader/Model/ShowNormal.vert", "Shaders/DebugShader/Model/ShowNormal.frag","Shaders/DebugShader/Model/ShowNormal.geo");


        // �����
        Shader SkyboxShader("Shaders/DebugShader/Skybox.vert", "Shaders/DebugShader/Skybox.frag");

        // �Ҿ��û��ǽк������һЩ
        Shader PostShader("Shaders/DebugShader/PostProcess.vert", "Shaders/DebugShader/PostProcess.frag");

        // ģ�ͣ��޹��գ�
        Shader modelShader("Shaders/DebugShader/Model/BaseModel.vert", "Shaders/DebugShader/Model/BaseModel.frag");
        Shader InstancingShader("Shaders/DebugShader/InstancingTest.vert", "Shaders/DebugShader/InstancingTest.frag");


    #pragma endregion

    #pragma region ��������

        unsigned int cubeTexture = LoadTexture("resources/textures/cube.jpg");
        unsigned int floorTexture = LoadTexture("resources/textures/floor.jpg");
        //unsigned int grassTexture = LoadTexture("resources/textures/grass.png");
        unsigned int windowTexture = LoadTexture("resources/textures/window.png");

        vector<std::string> skyboxPath
        {
            "resources/textures/skybox/right.jpg",
            "resources/textures/skybox/left.jpg",
            "resources/textures/skybox/top.jpg",
            "resources/textures/skybox/bottom.jpg",
            "resources/textures/skybox/front.jpg",
            "resources/textures/skybox/back.jpg"
        };

        unsigned int skyboxTexture = LoadCubeTexture(skyboxPath);

   #pragma endregion

    #pragma region ����ΪShader�е�����ָ������

        // cubeShader.setInt("material.diffuse", 0);
        // cubeShader.setInt("material.specular", 1);
        cubeShader.use();
        cubeShader.setInt("texture1", 0);

        AlphaShader.use();
        AlphaShader.setInt("texture1", 0);

        // PostShader.use();
        // PostShader.setInt("screenTexture", 0);

        SkyboxShader.use();
        SkyboxShader.setInt("skybox", 0);

        ReflectionShader.use();
        ReflectionShader.setInt("skybox", 0);

        RefractionShader.use();
        RefractionShader.setInt("skybox", 0);

    #pragma endregion



    #pragma region ģ����Ϣ
        
           // Model human("resources/objects/nanosuit/nanosuit.obj");
           // Model ball("resources/objects/cube.obj");
           Model planet("resources/objects/planet/planet.obj");
        
           // �����
           float skyboxVertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

           // ��һ��������
           // ����Ϊ����˳ʱ�뷽�����
           float cubeVertices[] = {
               // Back face
               -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
                0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
               -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
               // Front face
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
               -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
               // Left face
               -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
               -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
               -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
               // Right face
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
                0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
               // Bottom face
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
               // Top face
               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
               -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
           };

           //  ���䷽��
           float ReflectionCubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
           };

           // �ذ�
           float planeVertices[] = {
               // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
                5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
               -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
               -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

                5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
               -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
                5.0f, -0.5f, -5.0f,  2.0f, 2.0f
           };
           
           // ��һ������
           float transparentVertices[] = {
               // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
               0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
               0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
               1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

               0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
               1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
               1.0f,  0.5f,  0.0f,  1.0f,  0.0f
           };

           vector<glm::vec3> vegetation;
           vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
           vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
           vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
           vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
           vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

           // ������
           float quadVertices[] = {
               // positions   // texCoords
               -1.0f,  1.0f,  0.0f, 1.0f,
               -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

               -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
           };
   
            
           // �ĸ���
         //       float pointVertices[] = {
    //-0.5f,  0.5f, 1.0f, 0.0f, 0.0f,1.0f, // ����
    // 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,1.0f, // ����
    // 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,1.0f, // ����
    //-0.5f, -0.5f, 1.0f, 1.0f, 0.0f,1.0f  // ����
    //       };

    #pragma endregion

    #pragma region VBO

         unsigned int skyboxVBO, skyboxVAO;
         unsigned int cubeVBO, cubeVAO;
         unsigned int reflectioncubeVBO, reflectioncubeVAO;
         unsigned int planeVBO, planeVAO;
         unsigned int grassVBO, grassVAO;
         unsigned int screenVBO, screenVAO;
         unsigned int poiontsVBO, poiontsVAO;

         VBOmanager skybox(&skyboxVBO);
         skybox.addStaticBuffer(skyboxVertices, sizeof(skyboxVertices));
         skybox.addVAO(&skyboxVAO);
         skybox.BindVAO(skyboxVAO);
         skybox.addVertex(3, std::vector<int>{3});// Pos*3

         VBOmanager cube(&cubeVBO);
         cube.addStaticBuffer(cubeVertices, sizeof(cubeVertices));
         cube.addVAO(&cubeVAO);
         cube.BindVAO(cubeVAO);
         cube.addVertex(5, std::vector<int>{3, 2});// Pos*3 Texcoord*2

         VBOmanager reflectioncube(&reflectioncubeVBO);
         reflectioncube.addStaticBuffer(ReflectionCubeVertices, sizeof(ReflectionCubeVertices));
         reflectioncube.addVAO(&reflectioncubeVAO);
         reflectioncube.BindVAO(reflectioncubeVAO);
         reflectioncube.addVertex(6, std::vector<int>{3, 3});// Pos*3 Normal*2

         VBOmanager floor(&planeVBO);
         floor.addStaticBuffer(planeVertices, sizeof(planeVertices));
         floor.addVAO(&planeVAO);
         floor.BindVAO(planeVAO);
         floor.addVertex(5, std::vector<int>{3, 2});// Pos*3 Texcoord*2

         VBOmanager grass(&grassVBO);
         grass.addStaticBuffer(transparentVertices, sizeof(planeVertices));
         grass.addVAO(&grassVAO);
         grass.BindVAO(grassVAO);
         grass.addVertex(5, std::vector<int>{3, 2});// Pos*3 Texcoord*2

         VBOmanager screen(&screenVBO);
         screen.addStaticBuffer(quadVertices, sizeof(quadVertices));
         screen.addVAO(&screenVAO);
         screen.BindVAO(screenVAO);
         screen.addVertex(4, std::vector<int>{2, 2});// Pos*2 Texcoord*2

         //VBOmanager points(&poiontsVBO);
         //points.addStaticBuffer(pointVertices, sizeof(pointVertices));
         //points.addVAO(&poiontsVAO);
         //points.BindVAO(poiontsVAO);
         //points.addVertex(6, std::vector<int>{2,3,1});// Pos*2 Color*3


    #pragma endregion

    #pragma region Instancing

         int planet_num = 1000;

         glm::mat4* modelMatrices;
         modelMatrices = new glm::mat4[planet_num];

         srand(static_cast<unsigned int>(glfwGetTime()));

         float radius = 25.0;
         float offset = 5.0f;

         // ����M���������
         for (unsigned int i = 0; i < planet_num; i++)
         {
             glm::mat4 model = glm::mat4(1.0f);

             // 1. λ�ƣ��ֲ��ڰ뾶Ϊ radius ��Բ���ϣ�ƫ�Ƶķ�Χ��[-offset, offset]
             float angle = (float)i / (float)planet_num * 360.0f;
             float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float x = sin(angle) * radius + displacement;
             displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
             displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float z = cos(angle) * radius + displacement;
             /*float x =  rand() % 20;
             float y =  rand() % 20;
             float z = -20 + rand() % 20;*/

             model = glm::translate(model, glm::vec3(x, y, z));

             // 2. ���ţ��� 0.05 �� 0.25f ֮������
             float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
             model = glm::scale(model, glm::vec3(scale));

             // 3. ��ת������һ�����룩���ѡ�����ת�����������������ת
             float rotAngle = static_cast<float>((rand() % 360));
             model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

             // cout << "(x,y,z) = (" << x<<"," << y << "," << z << ") , scale = " << scale << endl;

             // 4. ��ӵ������������
             modelMatrices[i] = model;
         }

         unsigned int PlanetInstancingBuffer;
         glGenBuffers(1, &PlanetInstancingBuffer);
         glBindBuffer(GL_ARRAY_BUFFER, PlanetInstancingBuffer);
         glBufferData(GL_ARRAY_BUFFER, planet_num * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

         // Ϊÿ��Mesh���VAO
         for (unsigned int i = 0; i < planet.meshes.size(); i++)
         {
             // ΪVAO�������������
             unsigned int VAO = planet.meshes[i].VAO;
             glBindVertexArray(VAO);

             GLsizei vec4Size = sizeof(glm::vec4);

             // 0 : Pos , 1 : Normal , 2 : Texcoord
             // 3 , 4 , 5 , 6 --> Mat4[1:4]
             glEnableVertexAttribArray(3);
             glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(0 * vec4Size));
             glEnableVertexAttribArray(4);
             glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
             glEnableVertexAttribArray(5);
             glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
             glEnableVertexAttribArray(6);
             glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

             glVertexAttribDivisor(3, 1);
             glVertexAttribDivisor(4, 1);
             glVertexAttribDivisor(5, 1);
             glVertexAttribDivisor(6, 1);

             glBindVertexArray(0);
         }

    #pragma endregion



    #pragma region ����ѭ��

        std::cout << "BeginToLoop:" << endl;
        //float currentAngle = 0.0f;
        //float rotateSpeed = 0.01f;
        int frame = 0;

        while (!glfwWindowShouldClose(window))
        {

        #pragma region ׼������������DeltaTime �������� �������� �ָ�״̬��

            // �ȼ���deltatime
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            frame++;
            if (frame % 100 == 0) cout << "\rFPS: " << 1 / deltaTime << std::flush;
            if (frame % 60 == 0) frame = 1;

            // ����
            processInput(window);


            // ��Ⱦ
            // ��� ��ɫ���� �� ��Ȼ��� �� ģ�建��
            // ָ��֡�������
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        #pragma endregion



        #pragma region VP����

            // MVP����M��transform�任
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetView();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);

            // ��Uniform�����������ظ�setMat4
            glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);
            
            glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &projection);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            /*ShowNormalShader.use();
            ShowNormalShader.setMat4("view", view);
            ShowNormalShader.setMat4("projection", projection);*/

            // Skybox��ͶӰ���󵥶���

        #pragma endregion

        #pragma region �����е�һ������

            float scale = 1.1f;

            // STEP : 0 : �����
            DrawOutline(outlineShader, 0, scale);

            // �ذ�
            cubeShader.use();
            glBindVertexArray(planeVAO);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            model = glm::mat4(1.0f);
            cubeShader.setMat4("model", glm::mat4(1.0f));
            cubeShader.setVec3("cameraPos", camera.Position);
            glDrawArrays(GL_TRIANGLES, 0, 6);


            // С����
            modelShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
            // model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
            modelShader.setMat4("model", model);
            planet.Draw(modelShader);

            // С���Ǵ�
            InstancingShader.use();
            InstancingShader.setInt("texture_diffuse1", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, planet.textures_loaded[0].id);
            // ����С����
            for (unsigned int i = 0; i < planet.meshes.size(); i++)
            {
                glBindVertexArray(planet.meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, planet.meshes[i].indices.size(), GL_UNSIGNED_INT, &planet.meshes[i].indices[0], planet_num);
                glBindVertexArray(0);
            }


            /* 
            // ����ģ��
            // pass1 : ���Ʊ���
            modelShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
            modelShader.setMat4("model", model);
            human.Draw(modelShader);

            // pass2 �� ���Ʒ���
            ShowNormalShader.use();
            ShowNormalShader.setMat4("projection", projection);
            ShowNormalShader.setMat4("model", model);
            human.Draw(ShowNormalShader);
            */


            // ����������

            ReflectionShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(reflectioncubeVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2.0f, 0.5f, 0.0f));
            ReflectionShader.setMat4("model", model);
            ReflectionShader.setVec3("cameraPos", camera.Position);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            
            
            
            // ����
            RefractionShader.use();
            glBindVertexArray(reflectioncubeVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(4.0f, 0.5f, 0.0f));
            RefractionShader.setMat4("model", model);
            RefractionShader.setVec3("cameraPos", camera.Position);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            //// �ĸ���
            //PointsShader.use();
            //glBindVertexArray(poiontsVAO);
            //glDrawArrays(GL_POINTS, 0, 4);


        #pragma endregion

        #pragma region �����弰�����

            //// STEP : 1  :  pass1�����Ʊ���
            //DrawOutline(outlineShader, 1, scale);

            //// cubes
            //cubeShader.use();
            //glBindVertexArray(cubeVAO);
            //glBindTexture(GL_TEXTURE_2D, cubeTexture);
            //// cube1
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
            //cubeShader.setMat4("model", model);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            //// cube2
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            //cubeShader.setMat4("model", model);
            //glDrawArrays(GL_TRIANGLES, 0, 36);


            //// STEP : 2  :  pass2���������
            //DrawOutline(outlineShader, 2, scale, 1.1);
            //// cubes
            //glBindVertexArray(cubeVAO);
            //// cube1
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
            //model = glm::scale(model, glm::vec3(scale, scale, scale));
            //outlineShader.setMat4("model", model);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            //// cube2
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            //model = glm::scale(model, glm::vec3(scale, scale, scale));
            //outlineShader.setMat4("model", model);
            //glDrawArrays(GL_TRIANGLES, 0, 36);

            // STEP : 3  :  �ָ�״̬��������պ�����������
            DrawOutline(outlineShader, 3, scale);

        #pragma endregion

        #pragma region ��պ�

            // �������͸������ǰ��
            glDepthFunc(GL_LEQUAL);
            SkyboxShader.use();
            glBindVertexArray(skyboxVAO);
            //glActiveTexture(GL_TEXTURE0);
            // ��w������Ϊ1
            view = glm::mat4(glm::mat3(camera.GetView())); // remove translation from the view matrix
            SkyboxShader.setMat4("view", view);
            SkyboxShader.setMat4("projection", projection);

            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);

        #pragma endregion

        #pragma region ͸������
            
            // windows
            // ͸������֮ǰ��ͨ����Ȳ��ԣ����alphaΪ0�Ĳ�����Ȼ�ᵲס����İ�͸������
            glBindVertexArray(grassVAO);
            AlphaShader.use();
            glBindTexture(GL_TEXTURE_2D, windowTexture);

            std::map<float, glm::vec3> sortedAlpha;
            for (unsigned int i = 0; i < vegetation.size(); i++)
            {
                float dis = glm::length(camera.Position - vegetation[i]);
                sortedAlpha[dis] = vegetation[i];
                // dis�Ǽ�����������vegetation[i]
                // ͨ��map�ĵ�����������˳�����͸������
            }

            // ���������
            for (std::map<float, glm::vec3>::reverse_iterator iter = sortedAlpha.rbegin(); iter != sortedAlpha.rend(); iter++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, iter->second);
                // second���ǵڶ���Ԫ��
                AlphaShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            glBindVertexArray(0);

        #pragma endregion

        

        #pragma region ����
        
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // �����һ��FrameBuffer
            glDisable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            PostShader.use();
            glBindVertexArray(screenVAO);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glDrawArrays(GL_TRIANGLES, 0, 6);

        #pragma endregion



            /* 
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
            */
            
            
            // �������岢��ѯIO�¼�
            glfwSwapBuffers(window);
            glfwPollEvents();

        };

    #pragma endregion



    // �ͷ���Դ
    //nanosuitModel.Release();
    #pragma region Release

        /*glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &planeVBO);
        glDeleteBuffers(1, &grassVAO);
        glDeleteBuffers(1, &grassVBO);
        glDeleteBuffers(1, &screenVAO);
        glDeleteBuffers(1, &screenVBO);*/
        //human.Release();

    #pragma endregion

  
    glfwTerminate();
    std::cout << "Done" << endl;
    return 0;
    
}


// �����ǰ��Ϣ
void DisplayCurrentState()
{
    if (pressTime==0)
    {
        //camera.PrintState();
        GLint MemoryKb = 0;
        glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &MemoryKb);

        GLint curmemory = 0;
        glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &curmemory);


        cout << "���Դ棺" << MemoryKb / 1024 << "Mb" << " �������Դ棺" << curmemory / 1024 << endl;

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
        GLenum format = GL_RGB;
        if (nrComponents == 1)// �Ҷ�ͼ
            format = GL_RED;
        else if (nrComponents == 3)// RGBͼ
            format = GL_RGB;
        else if (nrComponents == 4)// RGBAͼ
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);        glGenerateMipmap(GL_TEXTURE_2D);// ����mipmap

        // ����Repeat��bug����Ϊ͸������ʱ����Ե������ȻΪ���в�ֵ�������а�͸�������Ҫ�ı�repeart����

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        stbi_image_free(data);
        std::cout << "Load Texture SUCCESS! " << path << " ,ID=" << textureID << std::endl;
    }
    else
    {
        std::cout << "Load Texture Failed! " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// ����һ�����������
unsigned int CreateEmptyTexture()
{
    // ��������
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    // Textureֻ������Width*Height���ڴ浫û���������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texColorBuffer;
}

// ��������ͼ������·������
unsigned int LoadCubeTexture(vector<string> textures_faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char* data;
    for (unsigned int i = 0; i < textures_faces.size(); i++)
    {
        data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
        
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 1)// �Ҷ�ͼ
                format = GL_RED;
            else if (nrChannels == 3)// RGBͼ
                format = GL_RGB;
            else if (nrChannels == 4)// RGBAͼ
                format = GL_RGBA;

            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data); 
        }
        else
        {
            std::cout << "Load single piece of Cube Texture FAILED! " << textures_faces[i] << std::endl;
            stbi_image_free(data);
        }    
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "Load CubeTexture SUCCESS! " << " ,ID=" << textureID << std::endl;
    return textureID;
}


// ���
void DrawOutline(Shader ourlineShader,int step,float& scale,float targetScale)
{
    // ԭ��
    /*
    �ڻ��ƣ���Ҫ��������ģ�����֮ǰ����ģ�庯������ΪGL_ALWAYS��ÿ�������Ƭ�α���Ⱦʱ����ģ�建�����Ϊ1��
    ��Ⱦ���塣
    ����ģ��д���Լ���Ȳ��ԡ�
    ��ÿ����������һ��㡣
    ʹ��һ����ͬ��Ƭ����ɫ�������һ�������ģ��߿���ɫ��
    �ٴλ������壬��ֻ������Ƭ�ε�ģ��ֵ������1ʱ�Ż��ơ�
    �ٴ�����ģ��д�����Ȳ��ԡ�
    */

    // �Ȼ����������壬ȷ�����Ƶذ��ʱ�򲻻����ģ�建��
    if (step == 0){
        glStencilMask(0x00); // �ǵñ�֤�����ڻ��Ƶذ��ʱ�򲻻����ģ�建��
    }

    // pass 1
    if (step == 1){
        // ��һ����Ⱦʱ������ģ�����Ϊ0xFF������������Ⱦ�����ص�ģ��ֵ
        glStencilFunc(GL_ALWAYS, 1, 0xFF);// ��ʱ�����Ƶĵط���ģ��ֵ������Ϊ1
        glStencilMask(0xFF);
    }
    // pass2
    if (step == 2)
    {
        // ����ģ��д������Ȳ���
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);// ֻ������ֵ��Ϊ1�Ĳ���ͨ�����ԣ���ֻ������֮ǰ���Ƶ�����֮��Ĳ���
        glStencilMask(0x00); // ����ȫ����ͨ��
        // ���ܹ���Ȳ��԰�������Ȼ������ƵĻᵲס
        //glDisable(GL_DEPTH_TEST);
        ourlineShader.use();
        scale = targetScale;
    }
    // �ָ�ģ����������Ȳ���
    if (step == 3){
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
        //glEnable(GL_DEPTH_TEST);
    }
}



