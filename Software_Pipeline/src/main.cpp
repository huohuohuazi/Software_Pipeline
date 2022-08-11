#pragma region INCLUDE

    #include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ

    #include <iostream>
    #include <stdio.h>
    #include <fstream>

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
    unsigned int CreateDepthFrameTexture();
    unsigned int LoadCubeTexture(vector<string> textures_faces);
    unsigned int CreateDepthCubeTexture();

#pragma endregion


#pragma region ��������

    const int SHADOW_WIDTH = 1024;
    const int SHADOW_HEIGHT = 1024;

    float lastX = Width / 2, lastY = Height / 2;

    float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    float lastFrame = 0.0f; // ��һ֡��ʱ��

    // ���̱���
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



    // һЩȫ�ֿ���
    bool On_Skybox = true;// �Ƿ��������
    bool On_OtherObject = true;// �Ƿ������������
    
    
    bool On_BlinnPhong = true;// �Ƿ�ʹ��Blinn_Phong / Phong����ģ��
    GLboolean On_GammaCorrection = true;// �Ƿ���Gamma����
    GLboolean shadows = true;// �Ƿ�����Ӱ
    
    #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
    #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


#pragma endregion

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

#pragma region һ�����Դ

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
    //glm::vec3 lightColor(0.3f, 0.3f, 0.3f);
    //float K_ambient = 0.05; //4  32

#pragma endregion



// ��������ص�
void processInput(GLFWwindow* window)
{
    // ��ESC�˳�
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "QuitWindow!" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }  

    #pragma region �����ƶ�

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

    #pragma endregion

    #pragma region ��Դ�ƶ�

        // ��Դ�ƶ��ٶ�
        float lightSpeed = 2.5f * deltaTime; // �ƶ��ٶ�

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            lightPos += glm::vec3(0.0, 1.0, 0.0) * lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            lightPos += glm::vec3(0.0, -1.0, 0.0) * lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            lightPos += glm::vec3(-1.0, 0.0, 0.0) * lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            lightPos += glm::vec3(1.0, 0.0, 0.0) * lightSpeed;

#   pragma endregion


    #pragma region ��Ⱦ����

        // Tab : ��ӡ��ǰ��Ϣ
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            if (!_TabPressed) {
                DisplayCurrentState();
                _TabPressed = true;
            }}
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) _TabPressed = false;


        // 1 : �ر������
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            if (!_1Pressed) {
                On_Skybox = !On_Skybox;
                string tmp = On_Skybox ? "On" : "OFF";
                cout << "Draw SkyBox : " << tmp << endl;
                _1Pressed = true; 
            }}
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) _1Pressed = false;


        // 2 : ��������������
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            if (!_2Pressed) {
                On_OtherObject = !On_OtherObject;
                string tmp = On_OtherObject ? "On" : "OFF";
                cout << "Draw OtherObject : " << tmp << endl;
                _2Pressed = true; 
            }}
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)  _2Pressed = false;

        // 3 : ƽ�й�Դ
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            if (!_3Pressed) {
                /*On_DirectLight = !On_DirectLight;
                string tmp = On_DirectLight ? "On" : "OFF";
                cout << "Use DirectLight : " << tmp << endl;*/
                _3Pressed = true; 
            }}
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)  _3Pressed = false;

        // 4 : ���Դ
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            if (!_4Pressed) {
                /*On_PointLight = !On_PointLight;
                string tmp = On_PointLight ? "On" : "OFF";
                cout << "Use PointLight : " << tmp << endl;*/
                _4Pressed = true;
            }}
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) _4Pressed = false;

        // 5 : ��Ӱ������
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (!_5Pressed) {
                shadows = !shadows;
                string tmp = shadows ? " ON " : "OFF";
                cout << "Shadow Render? : " << tmp << endl;
                _5Pressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) _5Pressed = false;

        // 6 : Gamma����
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

    #pragma region ��ʼ������

        //��ʼ������
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
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);// ���ڴ�С�ı�ص�����
        glfwSetCursorPosCallback(window, mouse_callback);// ����ƶ��ص�����
        glfwSetScrollCallback(window, scroll_callback);// �����ֻص�����

        // ���ع��
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        glViewport(0, 0, Width, Height);

        DisplayDeviceInfo();



    #pragma region ��Ȼ���

        // ������Ȳ���,�󲿷���Ȼ���ľ��ȶ���24λ��
        glEnable(GL_DEPTH_TEST);// ��ͨ����Ȳ��ԣ�������Ȼ����������Ƭ��/���ص����
        // ��������

        //glDepthFunc(GL_LESS);// ������Ȳ��Եķ���
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
        // glStencilFunc(GL_EQUAL, 1, 0xFF);
        // 1Ϊ�ο�ֵ�����Ƚϵ�����
        // 0xFFΪ���룬�����òο�ֵ�Ƚ�ǰ������(AND)���㣬Ĭ��Ϊȫ1

        // �������ȾʱҪ���е�����
       glStencilMask(0xFF);

        // �涨������Ϊ
        /*glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);*/
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

       // glEnable(GL_CULL_FACE);
       //glCullFace(GL_BACK);
       // glCullFace(GL_FRONT);
        //glCullFace(GL_FRONT);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CW);

    #pragma endregion


        // ͼƬ���뷴תy��
        stbi_set_flip_vertically_on_load(false);
     

    #pragma region �����

        bool MSAA = false;
        if (MSAA)
        {
            // �������ز���
            glfwWindowHint(GLFW_SAMPLES, 4);//GLFW���Զ�����һ��ÿ����4���Ӳ��������Ⱥ���������
            // ������ɫ��������Ĵβ�����ƽ�����еĲ�������δ���еģ�ƬԪ��ɫ����ֻ����һ���м�ĵ�
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_DEPTH_TEST);

        }

    #pragma endregion


#pragma endregion


    #pragma region Unifrom��

        // VP����
        unsigned int VPMatricxBlock;
        glGenBuffers(1, &VPMatricxBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);
        glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW); // ����152�ֽڵ��ڴ�
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // ���ӵ��󶨵�1
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, VPMatricxBlock);


    #pragma endregion

    #pragma region FBO

        // ��ȾFBO
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


        // ��ӰFBO
        unsigned depthMapFBO;
        glGenFramebuffers(1, &depthMapFBO);
        
        // ��һ������Ȳ���
        unsigned int depthMap = CreateDepthFrameTexture();
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); 
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        

        // �����巽����Ȳ���
        unsigned int depthCubemap = CreateDepthCubeTexture();
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
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
        // Shader InstancingShader("Shaders/DebugShader/InstancingTest.vert", "Shaders/DebugShader/InstancingTest.frag");

        // ����ģ��
        Shader PointLightShader("Shaders/Lighting/PointLight.vert", "Shaders/Lighting/PointLight.frag");
        Shader ShadowMapShader("Shaders/Shadow/ShadowMap.vert", "Shaders/Shadow/ShadowMap.frag");
        Shader ShadowShader("Shaders/Shadow/ShadowRenderer.vert", "Shaders/Shadow/ShadowRenderer.frag");
        Shader ShadowMapDebugShader("Shaders/Shadow/ShadowMapDebug.vert", "Shaders/Shadow/ShadowMapDebug.frag");


        Shader BlingPhongShader("Shaders/Lighting/Bling_Phong.vert", "Shaders/Lighting/Bling_Phong.frag");



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

    #pragma region ����ΪShader�е�����ָ������Ԫ����

        cubeShader.use();
        cubeShader.setInt("texture1", 0);

        AlphaShader.use();
        AlphaShader.setInt("texture1", 0);

        // ����Shader
        PostShader.use();
        PostShader.setInt("screenTexture", 0);

        // ��ӰShader
        ShadowShader.use();
        //ShadowShader.setInt("diffuseTexture", 0);
        //ShadowShader.setInt("shadowMap",1);
        glUniform1i(glGetUniformLocation(ShadowShader.ID, "diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(ShadowShader.ID, "shadowMap"), 1);

        ShadowMapDebugShader.use();
        ShadowMapDebugShader.setInt("depthMap", 0);

        

        // �����
        SkyboxShader.use();
        SkyboxShader.setInt("skybox", 0);

        // ����/����Shader
        ReflectionShader.use();
        ReflectionShader.setInt("skybox", 0);
        RefractionShader.use();
        RefractionShader.setInt("skybox", 0);

        BlingPhongShader.use();
        BlingPhongShader.setInt("texture_diffuse1", 0);

    #pragma endregion



    #pragma region ģ����Ϣ
        
           // Model human("resources/objects/nanosuit/nanosuit.obj");
          // Model ball("resources/objects/ball.obj");
           // Model planet("resources/objects/planet/planet.obj");
        
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
               // positions            // normals         // texcoords
                25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
               -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
               -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

                25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
               -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
                25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
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
           //vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
           //vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
           //vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
           //vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

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

         //VBOmanager points(&poiontsVBO);
         //points.addStaticBuffer(pointVertices, sizeof(pointVertices));
         //points.addVAO(&poiontsVAO);
         //points.BindVAO(poiontsVAO);
         //points.addVertex(6, std::vector<int>{2,3,1});// Pos*2 Color*3


    #pragma endregion

    // Instancing
    /* 
    #pragma region Instancing

         int planet_num = 1000;

         // M��������
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
             // �̶��Ƕȣ�������sin/cos���
             float angle = (float)i / (float)planet_num * 360.0f;

             float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float x = sin(angle) * radius + displacement;
             
             displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float y = displacement * 0.4f;
             
             displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
             float z = cos(angle) * radius + displacement;

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
         
         // ����Instancing��M���󼯺�Buffer
         unsigned int PlanetInstancingBuffer;
         glGenBuffers(1, &PlanetInstancingBuffer);
         glBindBuffer(GL_ARRAY_BUFFER, PlanetInstancingBuffer);
         glBufferData(GL_ARRAY_BUFFER, planet_num * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

         // ��ÿ��MeshVAO�Ļ��������Mat���ݣ�����uniform
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
        */
    

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
           
    #pragma region ����ѭ��

        std::cout << "BeginToLoop:" << endl;
        int frame = 0;

        while (!glfwWindowShouldClose(window))
        {

        #pragma region ׼������������DeltaTime �������� �������� �ָ�״̬��

            // �ȼ���deltatime
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            frame++;
            if (frame % 400 == 0) cout << "\rFPS: " << 1 / deltaTime << std::flush;
            if (frame % 400 == 0) frame = 1;

            // ����
            processInput(window);

            // �Ƿ���gamma����
            // ��������׶�
            /*if (On_GammaCorrection) {
                glEnable(GL_FRAMEBUFFER_SRGB);
            }else{
                glDisable(GL_FRAMEBUFFER_SRGB);
            }*/

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        #pragma endregion

        // ��Ӱ�׶�
        #pragma region ��ԴVP����
            
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;

            float near_plane = 1.0f;
            float far_plane = 7.5f;
            //glm::vec3 Front= glm::normalize(glm::vec3(2, -4, 1));
            glm::vec3 Front = glm::vec3(2, -4, 1);
            glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0,1,0)));
            glm::vec3 Up = glm::normalize(glm::cross(Right, Front));
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            //lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));           
            lightView = glm::lookAt(lightPos, lightPos+Front, Up);
            lightSpaceMatrix = lightProjection * lightView;       

        #pragma endregion

        #pragma region Pass1 : �ӹ�Դ����Ȳ���depth map

            // ���������޳���������ӰƯ��
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

            // �����FBO
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);


            ShadowMapShader.use();
            ShadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            // floor
            glm::mat4 model = glm::mat4(1.0f);
            ShadowMapShader.setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // cubes
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
            model = glm::scale(model, glm::vec3(0.5f));
            ShadowMapShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
            model = glm::scale(model, glm::vec3(0.5f));
            ShadowMapShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            

            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);

        #pragma endregion

        #pragma region ���VP����

            // MVP����M��transform�任
            glm::mat4 view = camera.GetView();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)Width / (float)Height, 0.1f, 100.0f);

            // ��Uniform�����������ظ�setMat4
            glBindBuffer(GL_UNIFORM_BUFFER, VPMatricxBlock);

            glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &projection);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);

        #pragma endregion

           
        #pragma region Pass2: Render Shadow
            
            // ��������ռ�
            glViewport(0, 0, Width, Height);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ShadowShader.use();

            ShadowShader.setMat4("projection", projection);
            ShadowShader.setMat4("view", view);

            ShadowShader.setVec3("lightPos", lightPos);
            ShadowShader.setVec3("viewPos", camera.Position);
            ShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            ShadowShader.setBool("shadows", shadows);
            

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);

           

            ShadowShader.use();
            // floor
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            model = glm::mat4(1.0f);
            ShadowShader.setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);


            // cubes
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
            model = glm::scale(model, glm::vec3(0.5f));
            ShadowShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
            model = glm::scale(model, glm::vec3(0.5f));
            ShadowShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            
            //// ShadowMap Visualization
            //ShadowMapDebugShader.use();
            ShadowMapDebugShader.use();
            ShadowMapDebugShader.setFloat("near_plane", near_plane);
            ShadowMapDebugShader.setFloat("far_plane", far_plane);
            model = glm::mat4(1.0f);
            ShadowMapDebugShader.setMat4("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glBindVertexArray(grassVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

        #pragma endregion
        
       

        #pragma region ���ƹ�Դ

            PointLightShader.use();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.1));
            model = glm::translate(model, lightPos);
            PointLightShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        #pragma endregion

        #pragma region �����е�һ������

            float scale = 1.1f;

            // STEP : 0 : �����
            // DrawOutline(outlineShader, 0, scale);


            if (On_OtherObject)
            {
                // ������
               /*modelShader.use();
               model = glm::mat4(1.0f);
               model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
               modelShader.setMat4("model", model);
               planet.Draw(modelShader);*/

               // InstancingС���Ǵ�
               /*InstancingShader.use();
               InstancingShader.setInt("texture_diffuse1", 0);
               glActiveTexture(GL_TEXTURE0);
               glBindTexture(GL_TEXTURE_2D, planet.textures_loaded[0].id);

           // ��Instanced��������
           for (unsigned int i = 0; i < planet.meshes.size(); i++)
           {
               glBindVertexArray(planet.meshes[i].VAO);
               glDrawElementsInstanced(GL_TRIANGLES, planet.meshes[i].indices.size(), GL_UNSIGNED_INT, &planet.meshes[i].indices[0], planet_num);
               glBindVertexArray(0);
           }*/


               // ����ģ��
               /*
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
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
                ReflectionShader.setMat4("model", model);
                ReflectionShader.setVec3("cameraPos", camera.Position);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);


                // ����
                RefractionShader.use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
                RefractionShader.setMat4("model", model);
                RefractionShader.setVec3("cameraPos", camera.Position);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

           


        #pragma endregion

        #pragma region �����弰�����

            if (On_OtherObject)
            {
            //    // STEP : 1  :  pass1�����Ʊ���
            //    DrawOutline(outlineShader, 1, scale);

            //    // cubes
            //    cubeShader.use();
            //    glBindVertexArray(cubeVAO);
            //    glBindTexture(GL_TEXTURE_2D, cubeTexture);
            //    // cube1
            //    model = glm::mat4(1.0f);
            //    model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
            //    cubeShader.setMat4("model", model);
            //    glDrawArrays(GL_TRIANGLES, 0, 36);
            //    // cube2
            //    model = glm::mat4(1.0f);
            //    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            //    cubeShader.setMat4("model", model);
            //    glDrawArrays(GL_TRIANGLES, 0, 36);


            //    // STEP : 2  :  pass2���������
            //    DrawOutline(outlineShader, 2, scale, 1.1);
            //    // cubes
            //    glBindVertexArray(cubeVAO);
            //    // cube1
            //    model = glm::mat4(1.0f);
            //    model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
            //    model = glm::scale(model, glm::vec3(scale, scale, scale));
            //    outlineShader.setMat4("model", model);
            //    glDrawArrays(GL_TRIANGLES, 0, 36);
            //    // cube2
            //    model = glm::mat4(1.0f);
            //    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            //    model = glm::scale(model, glm::vec3(scale, scale, scale));
            //    outlineShader.setMat4("model", model);
            //    glDrawArrays(GL_TRIANGLES, 0, 36);

            //    // STEP : 3  :  �ָ�״̬��������պ�����������
            //    DrawOutline(outlineShader, 3, scale);
            }
            // DrawOutline(outlineShader, 3, scale);
            

        #pragma endregion

        #pragma region ��պ�

            if (On_Skybox)
            {
               // �������͸������ǰ��
                glDepthFunc(GL_LEQUAL);
                SkyboxShader.use();
                glBindVertexArray(skyboxVAO);
                //glActiveTexture(GL_TEXTURE0);
               // // ��w������Ϊ1
                view = glm::mat4(glm::mat3(camera.GetView())); // remove translation from the view matrix
                SkyboxShader.setMat4("view", view);
                SkyboxShader.setMat4("projection", projection);

                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                glDepthFunc(GL_LESS);
            }

            

        #pragma endregion

        #pragma region ͸������
            
            if (On_OtherObject)
            {
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
                //glBindVertexArray(0);
            }
            
        #pragma endregion

        

        #pragma region ����
        
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // ���FBO
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            PostShader.use();
            glBindVertexArray(screenVAO);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            // ����gamma����
            PostShader.setBool("On_GammaCorrection", On_GammaCorrection);
            glDrawArrays(GL_TRIANGLES, 0, 6);

        #pragma endregion
            
            
            // �������岢��ѯIO�¼�
            glfwSwapBuffers(window);
            glfwPollEvents();

        };

    #pragma endregion



    // �ͷ���Դ
    //nanosuitModel.Release();
    #pragma region Release

        //human.Release();

    #pragma endregion

  
    glfwTerminate();
    std::cout << "Done" << endl;
    return 0;
    
}


// �����ǰ��Ϣ
void DisplayCurrentState()
{
    
    //camera.PrintState();
    GLint MemoryKb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &MemoryKb);

    GLint curmemory = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &curmemory);


    cout << "���Դ棺" << MemoryKb / 1024 << "Mb" << " �������Դ棺" << curmemory / 1024 << endl;

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
        std::cout << "Load Texture SUCCESS : " << path << " ,ID=" << textureID << std::endl;
    }
    else
    {
        std::cout << "Load Texture Failed! " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// ����һ��֡������������
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
    cout << "Generate Frame Texture Success : " << texColorBuffer << endl;

    return texColorBuffer;
}

// ����һ����Ȼ�����������
unsigned int CreateDepthFrameTexture()
{

    // ��������
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    // Textureֻ������Width*Height���ڴ浫û���������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);





    //glBindTexture(GL_TEXTURE_2D, 0);
    cout << "Generate Frame Texture Success : " << texColorBuffer << endl;

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


// �յ���������ͼ������shadowMap
unsigned int CreateDepthCubeTexture()
{
    unsigned int depthCubemapID;
    glGenTextures(1, &depthCubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapID);

    for (GLuint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "Load Empty Depth CubeTexture SUCCESS! " << " ,ID=" << depthCubemapID << std::endl;
    return depthCubemapID;
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
