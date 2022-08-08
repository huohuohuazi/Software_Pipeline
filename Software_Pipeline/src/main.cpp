#pragma region INCLUDE

    #include <glad/glad.h>//要放在所有头文件之前

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


#pragma region 函数声明

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


#pragma region 变量声明

    float lastX = Width / 2, lastY = Height / 2;

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间

    bool firstMouse = true;
    int pressTime = 0;

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
    #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


#pragma endregion



int main(int argc, char* argv[])
{

    #pragma region 初始化窗口

        //初始化窗口
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
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);// 窗口大小改变回调函数
        glfwSetCursorPosCallback(window, mouse_callback);// 鼠标移动回调函数
        glfwSetScrollCallback(window, scroll_callback);// 鼠标滚轮回调函数

        // 隐藏光标
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        DisplayDeviceInfo();


    #pragma region 深度缓冲

        // 开启深度测试,大部分深度缓冲的精度都是24位的
        glEnable(GL_DEPTH_TEST);// 若通过深度测试，则再深度缓冲区保存该片段/像素的深度
        // 否则舍弃

        glDepthFunc(GL_LESS);// 设置深度测试的方法
        // GL_LESS	在片段深度值小于缓冲的深度值时通过测试
        // GL_GREATER	在片段深度值大于缓冲区的深度值时通过测试
        // 还有GL_ALWAYS / GL_NEVER / GL_EQUAL / GL_LEQUAL

        // 禁用深度缓冲更新（设置掩码为false），用于不希望更新深度缓冲的情况
       // glDepthMask(GL_FALSE);

    #pragma endregion


    #pragma region 模板缓冲       

        // 模板缓冲
        glEnable(GL_STENCIL_TEST);

        // 这个是进行一次模板测试，与设定的掩码进行比较，可以自定义处理的方法
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // 1为参考值，即比较的内容
        // 0xFF为掩码，会在用参考值比较前进行与(AND)运算，默认为全1

        // 这个是渲染时要进行的掩码
        //glStencilMask(0xFF);

        // 规定缓冲行为
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // glStencilOp(GLenum sfail, GLenum dpfail,  GLenum dppass);
        // sfail：模板测试失败时;dpfail:模板测试通过但深度测试失败； dppass：都通过
        // 包括：GL_KEEP 保持当前的模板值 / GL_ZERO 设为0 / GL_REPLACE 替换为ref /  GL_INCR 模板值加1等等


    #pragma endregion


    #pragma region Alpha测试

        glEnable(GL_BLEND);

        // 混合选项
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    #pragma endregion


    #pragma region 面剔除 CULL

        // 不剔除了，地板有点问题() 其他模型没有问题

        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CW);

    #pragma endregion


        // 图片导入反转y轴
        stbi_set_flip_vertically_on_load(false);
        
#pragma endregion

    #pragma region Unifrom块

        unsigned int VPMatricxBlock;
        glGenBuffers(1, &VPMatricxBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);
        glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW); // 分配152字节的内存
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // 链接到绑定点1
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, VPMatricxBlock);

    #pragma endregion

    #pragma region FBO

        // FrameBuffer Object
        unsigned int FBO;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        unsigned int texColorBuffer = CreateEmptyTexture();

        // 讲Texture绑定到FBO上,Texture是16位的
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        // RenderBuffer Object渲染缓冲对象
        unsigned int RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);

        // 内部格式为GL_DEPTH24_STENCIL8，Depth是24位的,Stencil是8位的
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    #pragma endregion





    #pragma region 加载Shader

        // 加载shader
        Shader outlineShader("Shaders/DebugShader/StencilTest.vert", "Shaders/DebugShader/StencilTest.frag");
        Shader cubeShader("Shaders/DebugShader/BaseCube.vert", "Shaders/DebugShader/BaseCube.frag");
        Shader AlphaShader("Shaders/DebugShader/AlphaTest.vert", "Shaders/DebugShader/AlphaTest.frag");
        Shader ReflectionShader("Shaders/DebugShader/ReflectEnvironment.vert", "Shaders/DebugShader/ReflectEnvironment.frag");
        Shader RefractionShader("Shaders/DebugShader/ReflectEnvironment.vert", "Shaders/DebugShader/RefractionEnvironment.frag");
        // 三个参数的构造函数，最后一个是几何着色器路径
        //Shader PointsShader("Shaders/DebugShader/Geometry/GeometryTest.vert", "Shaders/DebugShader/Geometry/GeometryTest.frag","Shaders/DebugShader/Geometry/GeometryTest.geo");
        //Shader ShowNormalShader("Shaders/DebugShader/Model/ShowNormal.vert", "Shaders/DebugShader/Model/ShowNormal.frag","Shaders/DebugShader/Model/ShowNormal.geo");


        // 天空球
        Shader SkyboxShader("Shaders/DebugShader/Skybox.vert", "Shaders/DebugShader/Skybox.frag");

        // 我觉得还是叫后处理合适一些
        Shader PostShader("Shaders/DebugShader/PostProcess.vert", "Shaders/DebugShader/PostProcess.frag");

        // 模型（无光照）
        Shader modelShader("Shaders/DebugShader/Model/BaseModel.vert", "Shaders/DebugShader/Model/BaseModel.frag");
        Shader InstancingShader("Shaders/DebugShader/InstancingTest.vert", "Shaders/DebugShader/InstancingTest.frag");


    #pragma endregion

    #pragma region 加载纹理

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

    #pragma region 设置为Shader中的纹理指定索引

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



    #pragma region 模型信息
        
           // Model human("resources/objects/nanosuit/nanosuit.obj");
           // Model ball("resources/objects/cube.obj");
           Model planet("resources/objects/planet/planet.obj");
        
           // 天空球
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

           // （一个）方块
           // 更新为按照顺时针方向定义的
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

           //  反射方块
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

           // 地板
           float planeVertices[] = {
               // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
                5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
               -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
               -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

                5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
               -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
                5.0f, -0.5f, -5.0f,  2.0f, 2.0f
           };
           
           // （一个）草
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

           // 正方形
           float quadVertices[] = {
               // positions   // texCoords
               -1.0f,  1.0f,  0.0f, 1.0f,
               -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

               -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
           };
   
            
           // 四个点
         //       float pointVertices[] = {
    //-0.5f,  0.5f, 1.0f, 0.0f, 0.0f,1.0f, // 左上
    // 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,1.0f, // 右上
    // 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,1.0f, // 右下
    //-0.5f, -0.5f, 1.0f, 1.0f, 0.0f,1.0f  // 左下
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

         // 设置M矩阵的内容
         for (unsigned int i = 0; i < planet_num; i++)
         {
             glm::mat4 model = glm::mat4(1.0f);

             // 1. 位移：分布在半径为 radius 的圆形上，偏移的范围是[-offset, offset]
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

             // 2. 缩放：在 0.05 和 0.25f 之间缩放
             float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
             model = glm::scale(model, glm::vec3(scale));

             // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
             float rotAngle = static_cast<float>((rand() % 360));
             model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

             // cout << "(x,y,z) = (" << x<<"," << y << "," << z << ") , scale = " << scale << endl;

             // 4. 添加到矩阵的数组中
             modelMatrices[i] = model;
         }

         unsigned int PlanetInstancingBuffer;
         glGenBuffers(1, &PlanetInstancingBuffer);
         glBindBuffer(GL_ARRAY_BUFFER, PlanetInstancingBuffer);
         glBufferData(GL_ARRAY_BUFFER, planet_num * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

         // 为每个Mesh添加VAO
         for (unsigned int i = 0; i < planet.meshes.size(); i++)
         {
             // 为VAO基础上添加索引
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



    #pragma region 绘制循环

        std::cout << "BeginToLoop:" << endl;
        //float currentAngle = 0.0f;
        //float rotateSpeed = 0.01f;
        int frame = 0;

        while (!glfwWindowShouldClose(window))
        {

        #pragma region 准备工作：计算DeltaTime 处理输入 清理缓冲区 恢复状态机

            // 先计算deltatime
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            frame++;
            if (frame % 100 == 0) cout << "\rFPS: " << 1 / deltaTime << std::flush;
            if (frame % 60 == 0) frame = 1;

            // 输入
            processInput(window);


            // 渲染
            // 清除 颜色缓冲 与 深度缓冲 与 模板缓冲
            // 指定帧缓冲对象
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        #pragma endregion



        #pragma region VP矩阵

            // MVP矩阵，M是transform变换
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetView();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);

            // 用Uniform缓存区代替重复setMat4
            glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);
            
            glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &projection);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            /*ShowNormalShader.use();
            ShowNormalShader.setMat4("view", view);
            ShowNormalShader.setMat4("projection", projection);*/

            // Skybox的投影矩阵单独算

        #pragma endregion

        #pragma region 场景中的一般物体

            float scale = 1.1f;

            // STEP : 0 : 画别的
            DrawOutline(outlineShader, 0, scale);

            // 地板
            cubeShader.use();
            glBindVertexArray(planeVAO);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            model = glm::mat4(1.0f);
            cubeShader.setMat4("model", glm::mat4(1.0f));
            cubeShader.setVec3("cameraPos", camera.Position);
            glDrawArrays(GL_TRIANGLES, 0, 6);


            // 小行星
            modelShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
            // model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
            modelShader.setMat4("model", model);
            planet.Draw(modelShader);

            // 小行星带
            InstancingShader.use();
            InstancingShader.setInt("texture_diffuse1", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, planet.textures_loaded[0].id);
            // 绘制小行星
            for (unsigned int i = 0; i < planet.meshes.size(); i++)
            {
                glBindVertexArray(planet.meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, planet.meshes[i].indices.size(), GL_UNSIGNED_INT, &planet.meshes[i].indices[0], planet_num);
                glBindVertexArray(0);
            }


            /* 
            // 人物模型
            // pass1 : 绘制本体
            modelShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
            modelShader.setMat4("model", model);
            human.Draw(modelShader);

            // pass2 ： 绘制法线
            ShowNormalShader.use();
            ShowNormalShader.setMat4("projection", projection);
            ShowNormalShader.setMat4("model", model);
            human.Draw(ShowNormalShader);
            */


            // 反射立方体

            ReflectionShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(reflectioncubeVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2.0f, 0.5f, 0.0f));
            ReflectionShader.setMat4("model", model);
            ReflectionShader.setVec3("cameraPos", camera.Position);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            
            
            
            // 折射
            RefractionShader.use();
            glBindVertexArray(reflectioncubeVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(4.0f, 0.5f, 0.0f));
            RefractionShader.setMat4("model", model);
            RefractionShader.setVec3("cameraPos", camera.Position);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            //// 四个点
            //PointsShader.use();
            //glBindVertexArray(poiontsVAO);
            //glDrawArrays(GL_POINTS, 0, 4);


        #pragma endregion

        #pragma region 立方体及其描边

            //// STEP : 1  :  pass1，绘制本体
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


            //// STEP : 2  :  pass2，扩张描边
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

            // STEP : 3  :  恢复状态，绘制天空盒与其他物体
            DrawOutline(outlineShader, 3, scale);

        #pragma endregion

        #pragma region 天空盒

            // 天空球在透明物体前面
            glDepthFunc(GL_LEQUAL);
            SkyboxShader.use();
            glBindVertexArray(skyboxVAO);
            //glActiveTexture(GL_TEXTURE0);
            // 让w分量恒为1
            view = glm::mat4(glm::mat3(camera.GetView())); // remove translation from the view matrix
            SkyboxShader.setMat4("view", view);
            SkyboxShader.setMat4("projection", projection);

            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);

        #pragma endregion

        #pragma region 透明物体
            
            // windows
            // 透明物体之前会通过深度测试，因此alpha为0的部分仍然会挡住后面的半透明物体
            glBindVertexArray(grassVAO);
            AlphaShader.use();
            glBindTexture(GL_TEXTURE_2D, windowTexture);

            std::map<float, glm::vec3> sortedAlpha;
            for (unsigned int i = 0; i < vegetation.size(); i++)
            {
                float dis = glm::length(camera.Position - vegetation[i]);
                sortedAlpha[dis] = vegetation[i];
                // dis是键，用来索引vegetation[i]
                // 通过map的迭代器，可以顺序访问透明物体
            }

            // 反向迭代器
            for (std::map<float, glm::vec3>::reverse_iterator iter = sortedAlpha.rbegin(); iter != sortedAlpha.rend(); iter++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, iter->second);
                // second便是第二个元素
                AlphaShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            glBindVertexArray(0);

        #pragma endregion

        

        #pragma region 后处理
        
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑上一个FrameBuffer
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
            
            
            // 交换缓冲并查询IO事件
            glfwSwapBuffers(window);
            glfwPollEvents();

        };

    #pragma endregion



    // 释放资源
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


// 输出当前信息
void DisplayCurrentState()
{
    if (pressTime==0)
    {
        //camera.PrintState();
        GLint MemoryKb = 0;
        glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &MemoryKb);

        GLint curmemory = 0;
        glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &curmemory);


        cout << "总显存：" << MemoryKb / 1024 << "Mb" << " ，可用显存：" << curmemory / 1024 << endl;

        pressTime++;
    }
    


}

// 打印设备信息
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

// 窗口大小改变的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 鼠标移动回调
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

// 鼠标滑轮回调
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// 键盘输入回调
void processInput(GLFWwindow* window)
{
    // 按ESC退出
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "QuitWindow!" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }

    // 相机移动
    float cameraSpeed = 2.5f * deltaTime; // 移动速度

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

// 读取图片，返回ID
unsigned int LoadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)// 灰度图
            format = GL_RED;
        else if (nrComponents == 3)// RGB图
            format = GL_RGB;
        else if (nrComponents == 4)// RGBA图
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);        glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmap

        // 纹理Repeat的bug，当为透明材质时，边缘处理仍然为进行插值，导致有半透明框，因此要改变repeart策略

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

// 生成一个纹理缓冲对象
unsigned int CreateEmptyTexture()
{
    // 生成纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    // Texture只分配了Width*Height的内存但没有填充数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texColorBuffer;
}

// 立方体贴图，传入路径集合
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
            if (nrChannels == 1)// 灰度图
                format = GL_RED;
            else if (nrChannels == 3)// RGB图
                format = GL_RGB;
            else if (nrChannels == 4)// RGBA图
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


// 描边
void DrawOutline(Shader ourlineShader,int step,float& scale,float targetScale)
{
    // 原理
    /*
    在绘制（需要添加轮廓的）物体之前，将模板函数设置为GL_ALWAYS，每当物体的片段被渲染时，将模板缓冲更新为1。
    渲染物体。
    禁用模板写入以及深度测试。
    将每个物体缩放一点点。
    使用一个不同的片段着色器，输出一个单独的（边框）颜色。
    再次绘制物体，但只在它们片段的模板值不等于1时才绘制。
    再次启用模板写入和深度测试。
    */

    // 先绘制其他物体，确保绘制地板的时候不会更新模板缓冲
    if (step == 0){
        glStencilMask(0x00); // 记得保证我们在绘制地板的时候不会更新模板缓冲
    }

    // pass 1
    if (step == 1){
        // 第一次渲染时，设置模板测试为0xFF，更新所有渲染的像素的模板值
        glStencilFunc(GL_ALWAYS, 1, 0xFF);// 此时被绘制的地方的模板值都更新为1
        glStencilMask(0xFF);
    }
    // pass2
    if (step == 2)
    {
        // 禁用模板写入与深度测试
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);// 只有掩码值不为1的部分通过测试，即只绘制在之前绘制的箱子之外的部分
        glStencilMask(0x00); // 掩码全部不通过
        // 不能关深度测试啊。。不然后面绘制的会挡住
        //glDisable(GL_DEPTH_TEST);
        ourlineShader.use();
        scale = targetScale;
    }
    // 恢复模板启用与深度测试
    if (step == 3){
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
        //glEnable(GL_DEPTH_TEST);
    }
}



