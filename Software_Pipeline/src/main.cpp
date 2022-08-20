#pragma region INCLUDE

    #include <glad/glad.h>//要放在所有头文件之前

    #include <iostream>
    #include <stdio.h>
    #include <fstream>
    #include <random>

    using namespace std;

    
    //#include <GL/glut.h>
    #include <gl/glut.h>
    #include <gl/GL.h>


    #include <GLFW/glfw3.h>
    #include <GLM/glm.hpp>
    #include <GLM/gtc/matrix_transform.hpp>
    #include <GLM/gtc/type_ptr.hpp>
    

    //#define STB_IMAGE_IMPLEMENTATION

    #include "Shader.h"
    #include "Model.h"
    #include "Camera.h"
    #include "VBO.h"
    #include "Gbuffer.h"

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
    unsigned int CreateEmptyTexture(bool HDR);
    unsigned int CreateDepthFrameTexture();
    unsigned int LoadCubeTexture(vector<string> textures_faces);
    unsigned int CreateDepthCubeTexture(); 
    unsigned int lerp(GLfloat a, GLfloat b, GLfloat f);

#pragma endregion


#pragma region 变量声明

    const int SHADOW_WIDTH = 1024;
    const int SHADOW_HEIGHT = 1024;

    float lastX = Width / 2, lastY = Height / 2;

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间
    const GLfloat clearcolor[] = { 0.1f, 0.1f, 0.1f, 1.0f };

    // 键盘变量
    bool firstMouse = true;
    bool _TabPressed = false;
    bool _0Pressed = false;
    bool _1Pressed = false;
    bool _2Pressed = false;
    bool _3Pressed = false;
    bool _4Pressed = false;
    bool _5Pressed = false;
    bool _6Pressed = false;
    bool _7Pressed = false;
    bool _8Pressed = false;
    bool _9Pressed = false;



    // 一些全局控制
    bool On_Skybox = true;// 是否开启天空球
    bool On_OtherObject = true;// 是否绘制其他物体
    
    
    bool On_BlinnPhong = true;// 是否使用Blinn_Phong / Phong光照模型
    GLboolean On_GammaCorrection = true;// 是否开启Gamma矫正
    GLboolean shadows = true;// 是否开启阴影
    bool on_HDR = true;// 是否使用HDR缓冲纹理
    bool On_Bloom = true;/// 是否开启Bloom泛光
    bool On_SSAO = true;// 是否使用SSAO


    #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
    #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


#pragma endregion

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

#pragma region 一个点光源

    glm::vec3 lightPos(0.0f,0.5f, 0.0f);
    glm::vec3 lightColor(150.0f, 150.0f, 150.0f);
    //glm::vec3 lightColor(0.3f, 0.3f, 0.3f);
    //float K_ambient = 0.05; //4  32

#pragma endregion



// 键盘输入回调
void processInput(GLFWwindow* window)
{
    // 按ESC退出
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "QuitWindow!" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }  

    #pragma region 人物移动

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

    #pragma endregion

    #pragma region 光源移动

        // 光源移动速度
        float lightSpeed = 2.5f * deltaTime; // 移动速度

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            lightPos += glm::vec3(0.0, 1.0, 0.0) * lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            lightPos += glm::vec3(0.0, -1.0, 0.0) * lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            lightPos += glm::vec3(-1.0, 0.0, 0.0) * lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            lightPos += glm::vec3(1.0, 0.0, 0.0) * lightSpeed;

#   pragma endregion


    #pragma region 渲染开关

        // Tab : 打印当前信息
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            if (!_TabPressed) {
                DisplayCurrentState();
                _TabPressed = true;
            }}
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) _TabPressed = false;


        // 1 : 关闭天空球
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            if (!_1Pressed) {
                On_Skybox = !On_Skybox;
                string tmp = On_Skybox ? "On" : "OFF";
                cout << "Draw SkyBox : " << tmp << endl;
                _1Pressed = true; 
            }}
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) _1Pressed = false;


        // 2 : 不绘制其他物体
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            if (!_2Pressed) {
                On_OtherObject = !On_OtherObject;
                string tmp = On_OtherObject ? "On" : "OFF";
                cout << "Draw OtherObject : " << tmp << endl;
                _2Pressed = true; 
            }}
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)  _2Pressed = false;

        // 3 : Bloom
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            if (!_3Pressed) {
                On_Bloom = !On_Bloom;
                string tmp = On_Bloom ? "On" : "OFF";
                cout << "Use Bloom : " << tmp << endl;
                _3Pressed = true; 
            }}
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)  _3Pressed = false;

        // 4 : 点光源
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            if (!_4Pressed) {
                /*On_PointLight = !On_PointLight;
                string tmp = On_PointLight ? "On" : "OFF";
                cout << "Use PointLight : " << tmp << endl;*/
                _4Pressed = true;
            }}
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) _4Pressed = false;

        // 5 : 阴影开启？
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (!_5Pressed) {
                shadows = !shadows;
                string tmp = shadows ? " ON " : "OFF";
                cout << "Shadow Render? : " << tmp << endl;
                _5Pressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) _5Pressed = false;

        // 6 : Gamma矫正
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            if (!_6Pressed) {
                On_GammaCorrection = !On_GammaCorrection;
                string tmp = On_GammaCorrection ? "On"  : "OFF";
                cout <<"Gamma Correction : "<< tmp << endl;
                _6Pressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) _6Pressed = false;

    #pragma endregion


}

int main(int argc, char* argv[])
{

    #pragma region 初始化窗口

        //初始化窗口
        glutInit(&argc, argv);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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

        glViewport(0, 0, Width, Height);

        DisplayDeviceInfo();



    #pragma region 深度缓冲

        // 开启深度测试,大部分深度缓冲的精度都是24位的
        glEnable(GL_DEPTH_TEST);// 若通过深度测试，则再深度缓冲区保存该片段/像素的深度
        // 否则舍弃

        //glDepthFunc(GL_LESS);// 设置深度测试的方法
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
        // glStencilFunc(GL_EQUAL, 1, 0xFF);
        // 1为参考值，即比较的内容
        // 0xFF为掩码，会在用参考值比较前进行与(AND)运算，默认为全1

        // 这个是渲染时要进行的掩码
       glStencilMask(0xFF);

        // 规定缓冲行为
        /*glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);*/
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

       // glEnable(GL_CULL_FACE);
       //glCullFace(GL_BACK);
       // glCullFace(GL_FRONT);
        //glCullFace(GL_FRONT);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CW);

    #pragma endregion


        // 图片导入反转y轴
        stbi_set_flip_vertically_on_load(false);
     

    #pragma region 抗锯齿

        bool MSAA = false;
        if (MSAA)
        {
            // 开启多重采样
            glfwWindowHint(GLFW_SAMPLES, 4);//GLFW会自动创建一个每像素4个子采样点的深度和样本缓冲
            // 顶点着色器会进行四次采样，平均命中的采样点与未命中的，片元着色器则只计算一次中间的点
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_DEPTH_TEST);

        }

    #pragma endregion


#pragma endregion

    #pragma region Unifrom块

        // VP矩阵
        unsigned int VPMatricxBlock;
        glGenBuffers(1, &VPMatricxBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);
        glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW); // 分配152字节的内存
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // 链接到绑定点1
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, VPMatricxBlock);


    #pragma endregion

    #pragma region FBO

        #pragma region 默认FBO，用于后处理

            // FBO
            // FrameBufferBuffer Object 帧缓冲对象
            unsigned int FBO;
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // CBO
            // ColorBuffer Obect 颜色缓冲对象
            // 这里指定两个，一个用于颜色缓冲，一个用于Bloom后处理
            unsigned int CBO[2];
            for (int i = 0; i < 2; i++)
            {
                CBO[i] = CreateEmptyTexture(on_HDR);
                // GL_COLOR_ATTACHMENT0与GL_COLOR_ATTACHMENT1
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, CBO[i], 0);

            }
            cout << "CBO:" << CBO[0] << " , " << CBO[1] << endl;
            GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);


            // RBO
            // RenderBuffer Object 渲染缓冲对象 包括深度与模板缓冲对象
            GLuint RBO;
            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            // 内部格式为GL_DEPTH24_STENCIL8，Depth是24位的,Stencil是8位的
            // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        #pragma endregion

        
        #pragma region 阴影FBO，用于shadowMap

            // 阴影FBO
            unsigned DepthMapFBO;
            glGenFramebuffers(1, &DepthMapFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
            //// 单一方向深度采样
            //unsigned int depthMap = CreateDepthFrameTexture();
            //glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); 
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            //glDrawBuffer(GL_NONE);
            //glReadBuffer(GL_NONE);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // 立方体方向深度采样
            unsigned int cubeDepthMap = CreateDepthCubeTexture();
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeDepthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        #pragma endregion

        
        #pragma region BloomFBO，用于泛光

            // 注：Bloom有三个CBO：一个在默认FBO上，用于记录灯光亮度值；另外两个在BloomFBO上，用于迭代处理泛光特效

            // BloomFBO
            // 用于多次迭代后处理缓冲，Bloom使用垂直与水平方向交替处理，共需要32+32次后处理
            unsigned int BloomFBO[2];
            glGenFramebuffers(2, BloomFBO);

            unsigned BloomCBO[2];
            glGenTextures(2, BloomCBO);

            for (int i = 0; i < 2; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, BloomFBO[i]);
                BloomCBO[i] = CreateEmptyTexture(on_HDR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BloomCBO[i], 0);
            }

        #pragma endregion


        #pragma region SSAOFBO

            // SSAOshader
            unsigned int ssaoFBO;
            glGenFramebuffers(1, &ssaoFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

            // 这里和Gbuffer.SSAOBuffer完全一样
            unsigned int ssaoCBO;
            glGenTextures(1, &ssaoCBO);
            glBindTexture(GL_TEXTURE_2D, ssaoCBO);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoCBO, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // SSAOblurShader
            unsigned int SSAOblurFBO;
            glGenFramebuffers(1,&SSAOblurFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, SSAOblurFBO);

            // 这里和SSAOBuffer完全一样
            unsigned int SSAOblurCBO;
            glGenTextures(1, &SSAOblurCBO);
            glBindTexture(GL_TEXTURE_2D, SSAOblurCBO);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOblurCBO, 0);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        #pragma endregion


        // Gbuffer
        GBuffer gbuffer;  

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

        // 后处理
        Shader PostShader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/PostProcess.frag");
        Shader BloomShader("Shaders/PostProcess/Bloom.vert", "Shaders/PostProcess/Bloom.frag");
        Shader SSAOShader("Shaders/PostProcess/SSAO.vert", "Shaders/PostProcess/SSAO.frag");
        Shader SSAOblurShader("Shaders/PostProcess/SSAOblur.vert", "Shaders/PostProcess/SSAOblur.frag");


        // 模型（无光照）
        Shader modelShader("Shaders/DebugShader/Model/BaseModel.vert", "Shaders/DebugShader/Model/BaseModel.frag");
        // Shader InstancingShader("Shaders/DebugShader/InstancingTest.vert", "Shaders/DebugShader/InstancingTest.frag");



        // 光照模型
        // 灯光可视化
        Shader PointLightShader("Shaders/Lighting/PointLight.vert", "Shaders/Lighting/PointLight.frag");
        // shadowMap采样
        //Shader ShadowMapShader("Shaders/Shadow/ShadowMap.vert", "Shaders/Shadow/ShadowMap.frag");
        Shader cubeShadowMapShader("Shaders/Shadow/cubeShadowMap.vert","Shaders/Shadow/cubeShadowMap.geom", "Shaders/Shadow/cubeShadowMap.frag");
        // 阴影渲染
        //Shader ShadowShader("Shaders/Shadow/ShadowRenderer.vert", "Shaders/Shadow/ShadowRenderer.frag");
        Shader cubeShadowShader("Shaders/Shadow/cubeShadowMapRenderer.vert", "Shaders/Shadow/cubeShadowMapRenderer.frag");
        // shadowmap可视化
        // Shader ShadowMapDebugShader("Shaders/Shadow/ShadowMapDebug.vert", "Shaders/Shadow/ShadowMapDebug.frag");
        Shader cubeShadowMapDebugShader("Shaders/Shadow/cubeShadowMapDebug.vert", "Shaders/Shadow/cubeShadowMapDebug.frag");


        // GBuffer
        Shader GBufferGeometryShader("Shaders/Gbuffer/GeometryProcess.vert", "Shaders/Gbuffer/GeometryProcess.frag");
        Shader GBufferLightingShader("Shaders/Gbuffer/LightingProcess.vert", "Shaders/Gbuffer/LightingProcess.frag");

        // PBR
        Shader pbrShader("Shaders/PBR/pbr.vert", "Shaders/PBR/pbr.frag");




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

        unsigned int albedo = LoadTexture("resources/textures/PBR/rustediron2_basecolor.png");
        unsigned int normal = LoadTexture("resources/textures/PBR/rustediron2_normal.png");
        unsigned int metallic = LoadTexture("resources/textures/PBR/rustediron2_metallic.png");
        unsigned int roughness = LoadTexture("resources/textures/PBR/rustediron2_roughness.png");
        //unsigned int ao = LoadTexture("resources/textures/PBR/rusted_iron/rustediron2_basecolor.png");

   #pragma endregion

    #pragma region 设置为Shader中的纹理指定纹理单元索引

        cubeShader.use();
        cubeShader.setInt("texture1", 0);

        AlphaShader.use();
        AlphaShader.setInt("texture1", 0);

        // 后处理
        PostShader.use();
        PostShader.setInt("screenTexture", 0);
        PostShader.setInt("bloomTexture", 1);
        PostShader.setInt("DebugTexture", 2);


        BloomShader.use();
        BloomShader.setInt("lightTexture", 0); 

        SSAOShader.use();
        SSAOShader.setInt("viewPosition", 0);
        SSAOShader.setInt("viewNormal", 1);
        SSAOShader.setInt("aTexNoise", 2);
        // SSAOShader.setInt("gSSAOvalue", 3);

        SSAOblurShader.use();
        SSAOblurShader.setInt("gSSAO", 0);


        // Gbuffer
        GBufferGeometryShader.use();
        GBufferGeometryShader.setInt("texture_diffuse1", 0);

        // GBuffer光照阶段，输入三个缓冲
        GBufferLightingShader.use();
        GBufferLightingShader.setInt("gPositionDepth", 0);
        GBufferLightingShader.setInt("gNormal", 1);
        GBufferLightingShader.setInt("gAlbedoSpec", 2);
        GBufferLightingShader.setInt("gSSAO_blur", 3);
        GBufferLightingShader.setInt("depthMap", 4);

        // 阴影Shader
        //ShadowShader.use();
        //ShadowShader.setInt("diffuseTexture", 0);
        //ShadowShader.setInt("shadowMap",1);
        cubeShadowShader.use();
        cubeShadowShader.setInt("texture_diffuse1", 0);
        cubeShadowShader.setInt("depthMap",1);

        // 阴影Debug
        // ShadowMapDebugShader.use();
        //ShadowMapDebugShader.setInt("depthMap", 0);
        cubeShadowMapDebugShader.use();
        cubeShadowMapDebugShader.setInt("depthMap", 0);
        

        // 天空球
        SkyboxShader.use();
        SkyboxShader.setInt("skybox", 0);

        // 反射/折射Shader
        ReflectionShader.use();
        ReflectionShader.setInt("skybox", 0);
        RefractionShader.use();
        RefractionShader.setInt("skybox", 0);

        //BlingPhongShader.use();
        //BlingPhongShader.setInt("texture_diffuse1", 0);


        // PBR
        pbrShader.use();
        pbrShader.setInt("albedoMap", 0);
        pbrShader.setInt("normalMap", 1);
        pbrShader.setInt("metallicMap", 2);
        pbrShader.setInt("roughnessMap", 3);
        pbrShader.setInt("aoMap", 4);

        

    #pragma endregion


    #pragma region 模型信息
        
          //Model human("resources/objects/nanosuit/nanosuit.obj");
          // Model ball("resources/objects/ball.obj");
           // Model planet("resources/objects/planet/planet.obj");
        
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
               // back face
               -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
               -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
               -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
               // front face
               -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
               -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
               -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
               // left face
               -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
               -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
               -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
               -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
               -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
               -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
               // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
               // bottom face
               -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
               -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
               -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
               // top face
               -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
               -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
               -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
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
               // positions            // normals         // texcoords
                25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
               -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
               -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

                25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
               -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
                25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
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
           //vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
           //vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
           //vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
           //vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

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
   
           // 球体
           std::vector<glm::vec3> sphere_positions;
           std::vector<glm::vec2> sphere_uv;
           std::vector<glm::vec3> sphere_normals;
           std::vector<unsigned int> sphere_indices;

           const unsigned int X_SEGMENTS = 64;
           const unsigned int Y_SEGMENTS = 64;
           const float PI = 3.14159265359f;
           unsigned int indexCount;

           for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
           {
               for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
               {
                   float xSegment = (float)x / (float)X_SEGMENTS;
                   float ySegment = (float)y / (float)Y_SEGMENTS;
                   float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                   float yPos = std::cos(ySegment * PI);
                   float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                   sphere_positions.push_back(glm::vec3(xPos, yPos, zPos));
                   sphere_uv.push_back(glm::vec2(xSegment, ySegment));
                   sphere_normals.push_back(glm::vec3(xPos, yPos, zPos));
               }
           }

           bool oddRow = false;
           for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
           {
               if (!oddRow) // even rows: y == 0, y == 2; and so on
               {
                   for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                   {
                       sphere_indices.push_back(y * (X_SEGMENTS + 1) + x);
                       sphere_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                   }
               }
               else
               {
                   for (int x = X_SEGMENTS; x >= 0; --x)
                   {
                       sphere_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                       sphere_indices.push_back(y * (X_SEGMENTS + 1) + x);
                   }
               }
               oddRow = !oddRow;
           }
           indexCount = static_cast<unsigned int>(sphere_indices.size());

           std::vector<float> sphere_data;
           for (unsigned int i = 0; i < sphere_positions.size(); ++i)
           {
               sphere_data.push_back(sphere_positions[i].x);
               sphere_data.push_back(sphere_positions[i].y);
               sphere_data.push_back(sphere_positions[i].z);
               if (sphere_normals.size() > 0)
               {
                   sphere_data.push_back(sphere_normals[i].x);
                   sphere_data.push_back(sphere_normals[i].y);
                   sphere_data.push_back(sphere_normals[i].z);
               }
               if (sphere_uv.size() > 0)
               {
                   sphere_data.push_back(sphere_uv[i].x);
                   sphere_data.push_back(sphere_uv[i].y);
               }
           }
            
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
         //unsigned int sphereVBO, sphereVAO;
        // unsigned int poiontsVBO, poiontsVAO;

         VBOmanager skybox(&skyboxVBO);
         skybox.addStaticBuffer(skyboxVertices, sizeof(skyboxVertices));
         skybox.addVAO(&skyboxVAO);
         skybox.BindVAO(skyboxVAO);
         skybox.addVertex(3, std::vector<int>{3});// Pos*3

         VBOmanager cube(&cubeVBO);
         cube.addStaticBuffer(cubeVertices, sizeof(cubeVertices));
         cube.addVAO(&cubeVAO);
         cube.BindVAO(cubeVAO);
         cube.addVertex(8, std::vector<int>{3, 3,2});// Pos*3 Texcoord*2

         VBOmanager reflectioncube(&reflectioncubeVBO);
         reflectioncube.addStaticBuffer(ReflectionCubeVertices, sizeof(ReflectionCubeVertices));
         reflectioncube.addVAO(&reflectioncubeVAO);
         reflectioncube.BindVAO(reflectioncubeVAO);
         reflectioncube.addVertex(6, std::vector<int>{3, 3});// Pos*3 Normal*2

         VBOmanager floor(&planeVBO);
         floor.addStaticBuffer(planeVertices, sizeof(planeVertices));
         floor.addVAO(&planeVAO);
         floor.BindVAO(planeVAO);
         floor.addVertex(8, std::vector<int>{3,3,2});// Pos*3 Normal*3 Texcoord*2

         VBOmanager grass(&grassVBO);
         grass.addStaticBuffer(transparentVertices, sizeof(transparentVertices));
         grass.addVAO(&grassVAO);
         grass.BindVAO(grassVAO);
         grass.addVertex(5, std::vector<int>{3, 2});// Pos*3 Texcoord*2

         VBOmanager screen(&screenVBO);
         screen.addStaticBuffer(quadVertices, sizeof(quadVertices));
         screen.addVAO(&screenVAO);
         screen.BindVAO(screenVAO);
         screen.addVertex(4, std::vector<int>{2, 2});// Pos*2 Texcoord*2

         unsigned int sphereVBO, sphereVAO, sphereEBO;
         glGenBuffers(1, &sphereVBO);
         glGenBuffers(1, &sphereEBO);
         glGenVertexArrays(1, &sphereVAO);
         glBindVertexArray(sphereVAO);
         glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
         glBufferData(GL_ARRAY_BUFFER, sphere_data.size() * sizeof(float), &sphere_data[0], GL_STATIC_DRAW);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(unsigned int), &sphere_indices[0], GL_STATIC_DRAW);
         unsigned int stride = (3 + 2 + 3) * sizeof(float);
         glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
         glEnableVertexAttribArray(1);
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
         glEnableVertexAttribArray(2);
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));


    #pragma endregion

    // Instancing
    /* 
    #pragma region Instancing

         int planet_num = 1000;

         // M矩阵数组
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
             // 固定角度，坐标用sin/cos求出
             float angle = (float)i / (float)planet_num * 360.0f;

             float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float x = sin(angle) * radius + displacement;
             
             displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float y = displacement * 0.4f;
             
             displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float z = cos(angle) * radius + displacement;

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
         
         // 设置Instancing的M矩阵集合Buffer
         unsigned int PlanetInstancingBuffer;
         glGenBuffers(1, &PlanetInstancingBuffer);
         glBindBuffer(GL_ARRAY_BUFFER, PlanetInstancingBuffer);
         glBufferData(GL_ARRAY_BUFFER, planet_num * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

         // 在每个MeshVAO的基础上添加Mat数据，代替uniform
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
        */
    
        #pragma region SSAO

             // ssaoKernel包括64个随机方向
            
             // uniform_real_distribution在<random.h>里
             // uniform_real_distribution : 随机数分布
             // default_random_engine : 迭代器
             std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // 随机浮点数，范围0.0 - 1.0
             std::default_random_engine generator;
             std::vector<glm::vec3> ssaoKernel; // Kernel
             for (unsigned int i = 0; i < 64; ++i)
             {
                 // x=(-1,1) y=(-1,1) z=(0,1)
                 // 半球形的采样区域
                 glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
                 sample = glm::normalize(sample);
                 sample *= randomFloats(generator);
                 unsigned int scale = float(i) / 64.0;
                 // 使得scale的分布更集中于中心区域
                 scale = lerp(0.1f, 1.0f, scale * scale);
                 sample *= scale;
                 ssaoKernel.push_back(sample);
             }

             // 噪声纹理 4 * 4
             std::vector<glm::vec3> ssaoNoise;
             for (unsigned int i = 0; i < 16; i++)
             {
                 // 随机方向
                 glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
                 ssaoNoise.push_back(noise);
             }


             unsigned int noiseTexture; 
             glGenTextures(1, &noiseTexture);
             glBindTexture(GL_TEXTURE_2D, noiseTexture);
             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
             glBindTexture(GL_TEXTURE_2D, 0);

        #pragma endregion


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
           
    #pragma region 绘制循环

        std::cout << "BeginToLoop:" << endl;
        int frame = 0;

        while (!glfwWindowShouldClose(window))
        {

        #pragma region 准备工作：计算DeltaTime 处理输入 清理缓冲区 恢复状态机

            // 先计算deltatime
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            frame++;
            if (frame % 400 == 0) cout << "\rFPS: " << 1 / deltaTime << std::flush;
            if (frame % 400 == 0) frame = 1;

            // 输入
            processInput(window);

            // 是否开启gamma矫正
            // 移至后处理阶段
            /*if (On_GammaCorrection) {
                glEnable(GL_FRAMEBUFFER_SRGB);
            }else{
                glDisable(GL_FRAMEBUFFER_SRGB);
            }*/

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        #pragma endregion

        // 阴影阶段
        #pragma region 光源VP矩阵
              
            glm::mat4 lightProjection, lightView;
            std::vector<glm::mat4> lightSpaceMatrix;
            float near_plane;
            float far_plane;

            // 单一方向的光源shadowmap，平行投影
            /*
            near_plane = 1.0f;
            far_plane = 7.5f;
            //glm::vec3 Front= glm::normalize(glm::vec3(2, -4, 1));
            glm::vec3 Front = glm::vec3(2, -4, 1);
            glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0,1,0)));
            glm::vec3 Up = glm::normalize(glm::cross(Right, Front));
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            //lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));           
            lightView = glm::lookAt(lightPos, lightPos+Front, Up);
            lightSpaceMatrix = lightProjection * lightView;       
            */

            // shadowmap立方体贴图，透视投影
            near_plane = 1.0f;
            far_plane = 25.0f;
            // 视角是90f(相机为45f)
            lightProjection = glm::perspective(glm::radians(90.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);

            lightSpaceMatrix.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
            lightSpaceMatrix.push_back(lightProjection *glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
            lightSpaceMatrix.push_back(lightProjection *glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
            lightSpaceMatrix.push_back(lightProjection *glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
            lightSpaceMatrix.push_back(lightProjection *glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
            lightSpaceMatrix.push_back(lightProjection *glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


        #pragma endregion

        #pragma region 从光源处深度采样depth map

            // 开启正面剔除，避免阴影漂浮
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

            // 绑定深度FBO
            glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);


            cubeShadowMapShader.use();
            cubeShadowMapShader.setFloat("far_plane", far_plane);
            cubeShadowMapShader.setVec3("lightPos", lightPos);
            // glUniform3fv(glGetUniformLocation(cubeShadowMapShader.ID, "lightPos"), 1, &lightPos[0]);
            for (GLuint i = 0; i < 6; ++i)
                cubeShadowMapShader.setMat4("lightSpaceMatrix[" + std::to_string(i) + "]", lightSpaceMatrix[i]);
     

            // floor
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0));
            cubeShadowMapShader.setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            

            // cubes         
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0));
            model = glm::scale(model, glm::vec3(0.4f));
            cubeShadowMapShader.setMat4("model", model);
            //glBindVertexArray(cubeVAO);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

            /*model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0));
            model = glm::scale(model, glm::vec3(0.1f));
            cubeShadowMapShader.setMat4("model", model);*/
            //human.Draw(cubeShadowMapShader);


            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
           //glm::mat4 model = glm::mat4(1.0f);
            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);
            
        #pragma endregion

        #pragma region 相机VP矩阵

            // MVP矩阵，M是transform变换
            glm::mat4 view = camera.GetView();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)Width / (float)Height, 0.1f, 100.0f);

            // 用Uniform缓存区代替重复setMat4
            glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);

            glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &projection);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);

        #pragma endregion
      
        // 返回相机空间
        glViewport(0, 0, Width, Height);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        #pragma region 延迟渲染

            #pragma region GBuffer几何阶段

                    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.gBuffer);
                    // 之后的Buffer都会输出到GBuffer对象上
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glEnable(GL_DEPTH_TEST);

                    GBufferGeometryShader.use();
                    GBufferGeometryShader.setMat4("projection", projection);
                    GBufferGeometryShader.setMat4("view", view);

                    // floor
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, floorTexture);
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0));
                    GBufferGeometryShader.setMat4("model", model);
                    glBindVertexArray(planeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    // cubes
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, cubeTexture);


                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0));
                    model = glm::scale(model, glm::vec3(0.4f));
                    GBufferGeometryShader.setMat4("model", model);
                    //glBindVertexArray(cubeVAO);
                    //glDrawArrays(GL_TRIANGLES, 0, 36);
                    glBindVertexArray(sphereVAO);
                    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

                    /*model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0));
                    model = glm::scale(model, glm::vec3(0.1f));
                    GBufferGeometryShader.setMat4("model", model);*/
                    //human.Draw(GBufferGeometryShader);


            #pragma endregion

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    // 将GBuffer中的 深度缓冲 复制到 默认FBO的深度缓冲上
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer.gBuffer);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
                    glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


            #pragma region GBuffer光照阶段

                // 这里都是后处理

                #pragma region SSAO

                    // 生成阶段

                    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
                    // 其实相当于后处理，因此先禁用深度测试
                    glDisable(GL_DEPTH_TEST);

                    SSAOShader.use();
                    // 输入kernel以及采样值
                    for (unsigned int i = 0; i < 64; ++i)
                        SSAOShader.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
                    SSAOShader.setMat4("projection", projection);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, gbuffer.viewPosition); // PositionCBO是RGBA纹理
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, gbuffer.viewNormal);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, noiseTexture);

                    glBindVertexArray(screenVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    // 结果将输出到gbuffer.ssaoCBO上

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);


                    
                    // 模糊处理

                    glBindFramebuffer(GL_FRAMEBUFFER, SSAOblurFBO);
                    glClear(GL_COLOR_BUFFER_BIT);

                    SSAOblurShader.use();
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, ssaoCBO);

                    glBindVertexArray(screenVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);


                #pragma endregion

                #pragma region 混合阶段

                    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    //glClear(GL_COLOR_BUFFER_BIT);

                    const float linear = 0.09f;
                    const float quadratic = 0.032f;
                    //glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
                    glm::vec3 lightColor = glm::vec3(0.4);
                    GBufferLightingShader.use();
                    GBufferLightingShader.setVec3("lightPos", lightPos);
                    GBufferLightingShader.setVec3("lightColor", lightColor);
                    GBufferLightingShader.setFloat("light.Linear", linear);
                    GBufferLightingShader.setFloat("light.Quadratic", quadratic);



                    //GBufferLightingShader.setVec3("lightPos", lightPos);
                    GBufferLightingShader.setVec3("viewPos", camera.Position);
                    GBufferLightingShader.setBool("shadows", shadows);
                    GBufferLightingShader.setFloat("far_plane", far_plane);


                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, gbuffer.PositionDepthCBO);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, gbuffer.NormalCBO);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, gbuffer.ColorAndSpecularCBO);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, ssaoCBO);
                    glActiveTexture(GL_TEXTURE4);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeDepthMap);

                    glBindVertexArray(screenVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);


                #pragma endregion

                    // glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    glEnable(GL_DEPTH_TEST);

            #pragma region ShadowDebug

                    //// ShadowMap Visualization
                    //ShadowMapDebugShader.use();
                    /*cubeShadowMapDebugShader.use();
                    cubeShadowMapDebugShader.setFloat("near_plane", near_plane);
                    cubeShadowMapDebugShader.setFloat("far_plane", far_plane);
                    model = glm::mat4(1.0f);
                    cubeShadowMapDebugShader.setMat4("model", model);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, cubeDepthMap);
                    glBindVertexArray(grassVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);*/

                    /*
                    cubeShadowMapDebugShader.use();
                    glm::vec3 Ball_center(4, 0, 0);
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, Ball_center);
                    model = glm::scale(model, glm::vec3(0.2));
                    cubeShadowMapDebugShader.setMat4("model", model);
                    cubeShadowMapDebugShader.setVec3("center", Ball_center);
                    cubeShadowMapDebugShader.setFloat("near_plane", near_plane);
                    cubeShadowMapDebugShader.setFloat("far_plane", far_plane);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, DepthMapFBO);
                    ball.Draw(cubeShadowMapDebugShader);
                    // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                    */

            #pragma endregion


            #pragma endregion

        #pragma endregion    
            

        #pragma region 绘制光源

            // Bloom有三个CBO，其中一个绑在默认CBO上
            PointLightShader.use();
            //glClearBufferfv(GL_COLOR,CBO[1], clearcolor);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.1));   
            PointLightShader.setMat4("model", model);
            PointLightShader.setMat4("view", view);
            PointLightShader.setMat4("projection", projection);
            //glBindVertexArray(cubeVAO);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

        #pragma endregion

        #pragma region 场景中的一般物体

            float scale = 1.1f;

            // STEP : 0 : 画别的
            DrawOutline(outlineShader, 0, scale);



            if (On_OtherObject)
            {
                // 大星球
               /*modelShader.use();
               model = glm::mat4(1.0f);
               model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
               modelShader.setMat4("model", model);
               planet.Draw(modelShader);*/

               // Instancing小行星带
               /*InstancingShader.use();
               InstancingShader.setInt("texture_diffuse1", 0);
               glActiveTexture(GL_TEXTURE0);
               glBindTexture(GL_TEXTURE_2D, planet.textures_loaded[0].id);

           // 用Instanced方法绘制
           for (unsigned int i = 0; i < planet.meshes.size(); i++)
           {
               glBindVertexArray(planet.meshes[i].VAO);
               glDrawElementsInstanced(GL_TRIANGLES, planet.meshes[i].indices.size(), GL_UNSIGNED_INT, &planet.meshes[i].indices[0], planet_num);
               glBindVertexArray(0);
           }*/


               // 人物模型
               /*
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
                //glBindVertexArray(reflectioncubeVAO); 
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.4));
                ReflectionShader.setMat4("model", model);
                ReflectionShader.setVec3("cameraPos", camera.Position);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                //glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

                // 折射
                RefractionShader.use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.4));
                RefractionShader.setMat4("model", model);
                RefractionShader.setVec3("cameraPos", camera.Position);
                //glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);


                // PBR材质球
                pbrShader.use();
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.4));
                pbrShader.setMat4("model", model);
                pbrShader.setMat4("view", view);
                pbrShader.setMat4("projection", projection);
                pbrShader.setVec3("cameraPos", camera.Position);

                pbrShader.setVec3("lightPositions",lightPos);
                pbrShader.setVec3("lightColors", lightColor);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, albedo);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, normal);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, metallic);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, roughness);
                //glActiveTexture(GL_TEXTURE4);
                //glBindTexture(GL_TEXTURE_2D, ao);
                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);


            }

           


        #pragma endregion

        #pragma region 立方体及其描边

            if (false)
            {
                // STEP : 1  :  pass1，绘制本体
                DrawOutline(outlineShader, 1, scale);

                // cubes
                cubeShader.use();
                glBindVertexArray(cubeVAO);
                glBindTexture(GL_TEXTURE_2D, cubeTexture);
                // cube1
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(8.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.2,0.2,0.2));
                cubeShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // cube2
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(8.5f, 2.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
                cubeShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);


                // STEP : 2  :  pass2，扩张描边
                DrawOutline(outlineShader, 2, scale, 1.1);
                // cubes
                glBindVertexArray(cubeVAO);
                // cube1
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(8.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
                model = glm::scale(model, glm::vec3(scale, scale, scale));
                outlineShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // cube2
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(8.5f, 2.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
                model = glm::scale(model, glm::vec3(scale, scale, scale));
                outlineShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                // STEP : 3  :  恢复状态，绘制天空盒与其他物体
                DrawOutline(outlineShader, 3, scale);
            }
            // DrawOutline(outlineShader, 3, scale);
            

        #pragma endregion

        #pragma region 天空盒

            if (On_Skybox)
            {
               // 天空球在透明物体前面
                glDepthFunc(GL_LEQUAL);
                SkyboxShader.use();
                glBindVertexArray(skyboxVAO);
                //glActiveTexture(GL_TEXTURE0);
               // // 让w分量恒为1
                view = glm::mat4(glm::mat3(camera.GetView())); // remove translation from the view matrix
                SkyboxShader.setMat4("view", view);
                SkyboxShader.setMat4("projection", projection);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                glDepthFunc(GL_LESS);
            }

            

        #pragma endregion

        #pragma region 透明物体
            
            if (On_OtherObject)
            {
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
                //glBindVertexArray(0);
            }
            
        #pragma endregion      

        #pragma region 后处理
        
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑FBO

            #pragma region Bloom

                bool horizontal = true;
                unsigned int amount = 10;

                if (On_Bloom)
                {
                    
                    BloomShader.use();
                    glBindVertexArray(screenVAO);
                    glActiveTexture(GL_TEXTURE0);

                    /*glBindFramebuffer(GL_FRAMEBUFFER, BloomFBO[0]);
                    glClearBufferfv(GL_COLOR, BloomCBO[0], clearcolor);
                    glBindFramebuffer(GL_FRAMEBUFFER, BloomFBO[1]);
                    glClearBufferfv(GL_COLOR, BloomCBO[1], clearcolor);*/

                    for (unsigned int i = 0; i < amount; i++)
                    {
                        glBindFramebuffer(GL_FRAMEBUFFER, BloomFBO[horizontal]);
                        BloomShader.setBool("horizontal", horizontal);

                        // 第一次时，输入来自灯光的颜色缓冲
                        glBindTexture(GL_TEXTURE_2D, i == 0 ? CBO[1] : BloomCBO[!horizontal]);
                        glDrawArrays(GL_TRIANGLES, 0, 6);

                        horizontal = !horizontal;
                    }
                }

                
            #pragma endregion
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑FBO

            #pragma region 最终屏幕

            
                glDisable(GL_DEPTH_TEST);
                glClear(GL_COLOR_BUFFER_BIT);

                PostShader.use();
                glBindVertexArray(screenVAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, CBO[0]);
                //glBindTexture(GL_TEXTURE_2D,);
                if (On_Bloom)
                {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, BloomCBO[!horizontal]);
                }
                float exposure = 1.0;// 曝光参数
                // 开启gamma矫正
                PostShader.setBool("On_GammaCorrection", On_GammaCorrection);
                PostShader.setBool("On_Bloom", On_Bloom);
                PostShader.setFloat("exposure", exposure);
                PostShader.setBool("hdr", on_HDR);

                PostShader.setBool("Debug", false);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, gbuffer.viewPosition);

                glDrawArrays(GL_TRIANGLES, 0, 6);
                glEnable(GL_DEPTH_TEST);

            #pragma endregion


            
            
        #pragma endregion
            
            
            // 交换缓冲并查询IO事件
            glfwSwapBuffers(window);
            glfwPollEvents();

        };

    #pragma endregion



    // 释放资源
    //nanosuitModel.Release();
    #pragma region Release

        //human.Release();

    #pragma endregion

  
    glfwTerminate();
    std::cout << "Done" << endl;
    return 0;
    
}


// 加载图片至显存
unsigned int LoadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    unsigned char* data= stbi_load(path, &width, &height, &nrComponents, 0);
    
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
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmap
            // 纹理Repeat的bug，当为透明材质时，边缘处理仍然为进行插值，导致有半透明框，因此要改变repeart策略  
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


            stbi_image_free(data);
            std::cout << "Load Texture SUCCESS : " << path << " , channels = "<< nrComponents <<" ,ID=" << textureID << std::endl;
        }
        else
        {
            std::cout << "Load Texture Failed! " << path << std::endl;
            stbi_image_free(data);
        }
        return textureID;
    //}

    
}

// 加载立方体贴图
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

           /* if (HDR){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);
            }
            else {*/
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            // }
            

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


// 帧缓冲对象FBO Width * Height
unsigned int CreateEmptyTexture(bool HDR)
{

    // 生成纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    // Texture只分配了Width*Height的内存但没有填充数据
    if (HDR) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cout << "Generate Frame Texture Success : " << texColorBuffer << endl;

    return texColorBuffer;
}

// 深度缓冲对象 SHADOW_WIDTH * SHADOW_HEIGHT
unsigned int CreateDepthFrameTexture()
{

    // 生成纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    // Texture只分配了Width*Height的内存但没有填充数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindTexture(GL_TEXTURE_2D, 0);

    cout << "Generate Frame Texture Success : " << texColorBuffer << endl;

    return texColorBuffer;
}

// 立方体深度缓冲对象 6 * SHADOW_WIDTH * SHADOW_HEIGHT
unsigned int CreateDepthCubeTexture()
{
    unsigned int depthCubemapID;
    glGenTextures(1, &depthCubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapID);

    for (unsigned i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "Load Empty Depth CubeTexture SUCCESS! " << " ,ID=" << depthCubemapID << std::endl;
    return depthCubemapID;
}



// 输出当前信息
void DisplayCurrentState()
{

    //camera.PrintState();
    GLint MemoryKb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &MemoryKb);

    GLint curmemory = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &curmemory);


    cout << "总显存：" << MemoryKb / 1024 << "Mb" << " ，可用显存：" << curmemory / 1024 << endl;

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

// 描边
void DrawOutline(Shader ourlineShader, int step, float& scale, float targetScale)
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
    if (step == 0) {
        glStencilMask(0x00); // 记得保证我们在绘制地板的时候不会更新模板缓冲
    }

    // pass 1
    if (step == 1) {
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
    if (step == 3) {
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
        //glEnable(GL_DEPTH_TEST);
    }
}

// 插值函数
unsigned int lerp(GLfloat a, GLfloat b, GLfloat f)
{
    return a + f * (b - a);
}