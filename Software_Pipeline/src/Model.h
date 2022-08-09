#ifndef MODEL_H
#define MODEL_H

// 包含glad来获取所有的必须OpenGL头文件
#include <glad/glad.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "Mesh.h"
//#include "Texture.h"
#include "stb_image.h"


// 读取纹理并加载到显存中，返回于显存中的ID
unsigned int TextureFromFile(const char* path, const string& directory);




class Model
{
private:
    
    string directory;// 模型的目录，包括obj、mtl、材质贴图
    map<string, Texture> loadedTextureMap;// 材质与名称映射关系
    Texture default_texture;


     // 默认材质，以后用的多
    void loadDefaultTexture()
    {
        //string pa = "1001_albedo.jpg";
        string pa = "_WoodFine0058_11_M.jpg";


        //string dir = pa.substr(0, pa.find_last_of('/'));
        //string dir = "resources/objects/survival-guitar-backpack/textures";
        string dir = "resources/objects/chair";
        default_texture.id = TextureFromFile(pa.c_str(), dir);
        default_texture.type = "texture_diffuse";
        //default_texture.path = "resources/objects/survival-guitar-backpack/textures/1001_albedo.jpg";
        default_texture.path = "resources/objects/chair/_WoodFine0058_11_M.jpg";


        textures_loaded.push_back(default_texture);
    }

    
    // 开始加载模型
    void loadModel(string path)
    {
        std::cout << "Start Loading Model..." << endl;

        // 先计算deltatime
        float startTime = glfwGetTime();

        Assimp::Importer importer;
        // 调用Importer的方法
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        // aiProcess_Triangulate:所有的图元形状强制变换为三角形。aiProcess_FlipUVs：自动反转UV的y轴，和opengl对应
        // aiProcess_SplitLargeMeshes/aiProcess_OptimizeMeshes：分割/整合网格，适应最大网格数/减少drawcall的需求

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            // 检查数据，如果不完整，则标记到AI_SCENE_FLAGS_INCOMPLETE上
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }
        // 模型存放的文件目录
        directory = path.substr(0, path.find_last_of('/'));
        std::cout << "Read Model File Success： "<< directory << endl;

        //loadDefaultTexture();

        // 递归处理所有节点
        processNode(scene->mRootNode, scene);
        
        float endTime = glfwGetTime();
        cout << "模型导入花费时间：" << endTime-startTime << endl;

    }

    
    // 将处理模型细分为处理mesh，将节点的assimp的aiMesh格式处理成为openGL的Mesh格式
    void processNode(aiNode* node, const aiScene* scene)
    {
        // std::cout << "正在转化Mesh，节点数：" << node->mNumMeshes << endl;
        // 将节点的assimp的aiMesh格式处理成为openGL的Mesh格式，并写入vector
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        // 遍历所有子节点
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }


    // 将每个assimp的aiMesh转换为Mesh格式
    Mesh processMesh(aiMesh* aimesh, const aiScene* scene)
    {
        vector<Vertex> vertices;// 顶点集合
        vector<unsigned int> indices;// 索引集合
        vector<Texture> textures;// 材质集合

    #pragma region 顶点

        // 遍历aiMesh的顶点，处理顶点位置、法线和纹理坐标uv
        for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
        {

            // Mesh的顶点对象
            Vertex vertex;


            // 顶点位置 .Position
            glm::vec3 vector;
            vector.x = aimesh->mVertices[i].x;
            vector.y = aimesh->mVertices[i].y;
            vector.z = aimesh->mVertices[i].z;
            vertex.Position = vector;


            // 顶点法线 .Normal
            if (aimesh->HasNormals())
            {
                vector.x = aimesh->mNormals[i].x;
                vector.y = aimesh->mNormals[i].y;
                vector.z = aimesh->mNormals[i].z;
                vertex.Normal = vector;
            }


            // 纹理映射 .TexCoord，以及.Tangent 与 .Bitangent
            if (aimesh->mTextureCoords[0]) // 网格是否有纹理坐标？
            {
                // cout << "The Mesh has TexCoord" << endl;
                // Assimp最多支持8个uv（是8个uv映射关系，不是8个材质），但我们用不到这么多
                glm::vec2 vec;
                vec.x = aimesh->mTextureCoords[0][i].x;
                vec.y = aimesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                
                //// tangent
                //vector.x = aimesh->mTangents[i].x;
                //vector.y = aimesh->mTangents[i].y;
                //vector.z = aimesh->mTangents[i].z;
                //vertex.Tangent = vector;
                //// bitangent
                //vector.x = aimesh->mBitangents[i].x;
                //vector.y = aimesh->mBitangents[i].y;
                //vector.z = aimesh->mBitangents[i].z;
                //vertex.Bitangent = vector;
                
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

    #pragma endregion


    #pragma region 索引

       // 处理索引(面与顶点关系)
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
        {
            aiFace face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

    #pragma endregion


    #pragma region 材质

        // cout << *aimesh->mNumUVComponents << endl;
        // 处理材质
        // std::cout << "Process materials:" << aimesh->mMaterialIndex << endl;
        if (aimesh->mMaterialIndex >= 0)
        {
            aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

            std::cout << "Material Name: " << aimaterial->GetName().C_Str() << endl;

            vector<Texture> diffuseTexture;
            processMaterial(aimaterial, scene, aiTextureType_DIFFUSE, diffuseTexture);
            textures.insert(textures.end(), diffuseTexture.begin(), diffuseTexture.end());
            
            vector<Texture> specularTexture;
            processMaterial(aimaterial, scene, aiTextureType_SPECULAR, specularTexture);
            textures.insert(textures.end(), specularTexture.begin(), specularTexture.end());

            vector<Texture> normalTexture;
            processMaterial(aimaterial, scene, aiTextureType_NORMALS, normalTexture);
            textures.insert(textures.end(), normalTexture.begin(), normalTexture.end());

            vector<Texture> heightTexture;
            processMaterial(aimaterial, scene, aiTextureType_HEIGHT, heightTexture);
            textures.insert(textures.end(), heightTexture.begin(), heightTexture.end());


        }
        // 如果没有材质，则指定为默认材质                                                               
        //if (textures.size() == 0)
        else
        {
            cout << "Use Defualt Texture..." << endl;
            // textures.push_back(default_texture);
        }
        
        cout << "Texture Number:" << textures.size() << endl;

    #pragma endregion

        //整合为Mesh对象
        // cout << "Size of vertices=" << vertices.size() << " Size of indices=" << indices.size() << " Size of textures=" << textures.size() << endl;
        return Mesh(vertices, indices,textures);
    
}
    
    // 新方法？
    bool processMaterial(const aiMaterial* matPtr, const aiScene* sceneObjPtr,const aiTextureType textureType, std::vector<Texture>& textures)
    {
        textures.clear();

        if (!matPtr
            || !sceneObjPtr)
        {
            return false;
        }
        if (matPtr->GetTextureCount(textureType) <= 0)
        {
            return false;
        }
        for (size_t i = 0; i < matPtr->GetTextureCount(textureType); ++i)
        {
            Texture text;
            aiString textPath;
            aiReturn retStatus = matPtr->GetTexture(textureType, i, &textPath);
            if (retStatus != aiReturn_SUCCESS
                || textPath.length == 0)
            {
                std::cerr << "Warning, load texture type=" << textureType
                    << "index= " << i << " failed with return value= "
                    << retStatus << std::endl;
                continue;
            }

            // 建立路径与材质的映射关系
            std::string absolutePath = this->directory + "/" + textPath.C_Str();
            map<string, Texture>::const_iterator it = this->loadedTextureMap.find(absolutePath);

            if (it == this->loadedTextureMap.end()) // 检查是否已经加载过了
            {
                // 如果纹理还没有被加载，则加载它
                //GLuint textId = TextureHelper::load2DTexture(absolutePath.c_str());
                text.id = TextureFromFile(textPath.C_Str(), directory);// id是纹理于显存中的ID
                text.path = absolutePath;// 路径是相对路径
                text.type = textureType;
                textures.push_back(text);
                textures_loaded.push_back(text);
                loadedTextureMap[absolutePath] = text;
            }
            else
            {
                // 若已经加载到内存，则直接从内存读取
                textures.push_back(it->second);
            }
        }
        return true;
    }


   
public:
    vector<Mesh> meshes;// 一个模型有多个Mesh
    vector<Texture> textures_loaded;// 已加载的纹理，在重复加载时可以直接取用

    int VertNum;
    int FaceNum;


    // 构造函数，加载模型
    Model(const char* path)
    {
        VertNum = 0;
        FaceNum = 0;
        loadModel(path);   
    }
    Model(char* path)
    {
        VertNum = 0;
        FaceNum = 0;
        loadModel(path);
    }

    // 使用shader绘制model（遍历绘制mesh）
    void Draw(Shader shader)
    {
        // meshes是vector数组，遍历渲染即可
        // cout << "Number of Meshes:" << meshes.size() << endl;
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    // 释放缓存
    void Release()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Release();
    }

};

// 读取纹理并加载到显存中，返回于显存中的ID
unsigned int TextureFromFile(const char* path, const string& directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        std::cout << "Texture LOAD Success:  " << filename <<" , ID = " << textureID << std::endl;
    }
    else
    {
        std::cout << "Texture LOAD Failed:  " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif