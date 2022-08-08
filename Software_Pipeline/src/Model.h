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


// 纹理可能不是一个模型独有的，不作为私有变量了
vector<Texture> textures_loaded;// 已加载的纹理，在重复加载时可以直接取用
Texture default_texture;

// 读取纹理并加载到显存中，返回于显存中的ID
unsigned int TextureFromFile(const char* path, const string& directory);


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



class Model
{
private:
    vector<Mesh> meshes;// 一个模型有多个Mesh
    string directory;// 模型的目录，包括obj、mtl、材质贴图
    map<string, Texture> loadedTextureMap;// 材质与名称映射关系
    

    
    // 开始加载模型
    void loadModel(string path)
    {
        std::cout << "Start Loading Model..." << endl;

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

        loadDefaultTexture();

        // 递归处理所有节点
        processNode(scene->mRootNode, scene);
        
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
        //std::cout << "aimesh->mNumVertices = " << aimesh->mNumVertices << endl;
        //std::cout << " x= " << aimesh->mVertices[0].x << " y= " << aimesh->mVertices[0].y << " z= " << aimesh->mVertices[0].z << endl;
        // std::cout << "Process Vertices:" << endl;
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

        // std::cout << "Process indices:" << endl;
       // 处理索引(面与顶点关系)
       // std::cout << "aimesh->mNumFaces = " << aimesh->mNumFaces << endl;
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

            /* 
            // cout<<"Processing Material" << endl;
            // assimp的material
            
            


            // 纹理贴图
            vector<Texture> diffuseMaps = loadMaterialTextures(aimaterial,
                aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // 高光贴图
            vector<Texture> specularMaps = loadMaterialTextures(aimaterial,
                aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // 法线贴图
            std::vector<Texture> normalMaps = loadMaterialTextures(aimaterial,
                aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // 高度贴图
            std::vector<Texture> heightMaps = loadMaterialTextures(aimaterial,
                aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            */
        }
        // 如果没有材质，则指定为默认材质                                                               
        //if (textures.size() == 0)
        else
        {
            cout << "Use Defualt Texture..." << endl;
            textures.push_back(default_texture);
        }
        
        cout << "Texture Number:" << textures.size() << endl;

    #pragma endregion

        //整合为Mesh对象
        cout << "Size of vertices=" << vertices.size() << " Size of indices=" << indices.size() << " Size of textures=" << textures.size() << endl;
        return Mesh(vertices, indices,textures);
    
}
    
    /*
    // 将assimp的材质转化为mesh格式。一次操作同种类型的材质贴图
    vector<Texture> loadMaterialTextures(aiMaterial* aimat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
   
        // std::cout << "Nunber of Property:: " << aimat->mNumProperties << endl;
        // cout << aimat->mProperties[0]->mKey.C_Str() << aimat->mProperties[1]->mKey.C_Str() << endl;
        //std::cout << "Type : " << type <<" Num:" << aimat->GetTextureCount(type) << endl;
        // 这里做一个优化，开辟一个数组用于保存已经加载过的纹理，可以直接从内存/显存中取，而不用IO读取
        
        // cout << "Type: " << type << " = " << aimat->GetTextureCount(type);
        for (unsigned int i = 0; i < aimat->GetTextureCount(type); i++)
        {

            aiString str;
            aimat->GetTexture(type, i, &str);
            bool skip = false;
            std::cout << "String:" << str.C_Str() << endl;
            // 每次加载时遍历一下已加载的纹理名称，如有重复则跳过
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                // string_compare
                if (std::strcmp(textures_loaded[j].path.data, str.C_Str()) == 0)
                {
                    std::cout << "SKIP" << endl;
                    // 对于重复的，则直接从数组中读取
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {   
                cout << "No SKIP" << endl;
                // 如果纹理还没有被加载，则加载它
                Texture texture;

                // id是纹理于显存中的ID
                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // 添加到已加载的纹理中
            }
        
        }
        
        return textures;

    }
    */
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
            std::string absolutePath = this->directory + "/" + textPath.C_Str();
            map<string, Texture>::const_iterator it = this->loadedTextureMap.find(absolutePath);
            if (it == this->loadedTextureMap.end()) // 检查是否已经加载过了
            {
                //GLuint textId = TextureHelper::load2DTexture(absolutePath.c_str());

                //text.id = TextureFromFile(absolutePath.c_str(), directory);
                text.id = TextureFromFile(textPath.C_Str(), directory);
                text.path = absolutePath;
                text.type = textureType;
                textures.push_back(text);
                loadedTextureMap[absolutePath] = text;
            }
            else
            {
                textures.push_back(it->second);
            }
        }
        return true;
    }


public:
    
    // 构造函数，加载模型
    Model(const char* path)
    {
        loadModel(path);
    }
    Model(char* path)
    {
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