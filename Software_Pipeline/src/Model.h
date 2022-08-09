#ifndef MODEL_H
#define MODEL_H

// ����glad����ȡ���еı���OpenGLͷ�ļ�
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


// ��ȡ�������ص��Դ��У��������Դ��е�ID
unsigned int TextureFromFile(const char* path, const string& directory);




class Model
{
private:
    
    string directory;// ģ�͵�Ŀ¼������obj��mtl��������ͼ
    map<string, Texture> loadedTextureMap;// ����������ӳ���ϵ
    Texture default_texture;


     // Ĭ�ϲ��ʣ��Ժ��õĶ�
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

    
    // ��ʼ����ģ��
    void loadModel(string path)
    {
        std::cout << "Start Loading Model..." << endl;

        // �ȼ���deltatime
        float startTime = glfwGetTime();

        Assimp::Importer importer;
        // ����Importer�ķ���
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        // aiProcess_Triangulate:���е�ͼԪ��״ǿ�Ʊ任Ϊ�����Ρ�aiProcess_FlipUVs���Զ���תUV��y�ᣬ��opengl��Ӧ
        // aiProcess_SplitLargeMeshes/aiProcess_OptimizeMeshes���ָ�/����������Ӧ���������/����drawcall������

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            // ������ݣ���������������ǵ�AI_SCENE_FLAGS_INCOMPLETE��
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }
        // ģ�ʹ�ŵ��ļ�Ŀ¼
        directory = path.substr(0, path.find_last_of('/'));
        std::cout << "Read Model File Success�� "<< directory << endl;

        //loadDefaultTexture();

        // �ݹ鴦�����нڵ�
        processNode(scene->mRootNode, scene);
        
        float endTime = glfwGetTime();
        cout << "ģ�͵��뻨��ʱ�䣺" << endTime-startTime << endl;

    }

    
    // ������ģ��ϸ��Ϊ����mesh�����ڵ��assimp��aiMesh��ʽ�����ΪopenGL��Mesh��ʽ
    void processNode(aiNode* node, const aiScene* scene)
    {
        // std::cout << "����ת��Mesh���ڵ�����" << node->mNumMeshes << endl;
        // ���ڵ��assimp��aiMesh��ʽ�����ΪopenGL��Mesh��ʽ����д��vector
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        // ���������ӽڵ�
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }


    // ��ÿ��assimp��aiMeshת��ΪMesh��ʽ
    Mesh processMesh(aiMesh* aimesh, const aiScene* scene)
    {
        vector<Vertex> vertices;// ���㼯��
        vector<unsigned int> indices;// ��������
        vector<Texture> textures;// ���ʼ���

    #pragma region ����

        // ����aiMesh�Ķ��㣬������λ�á����ߺ���������uv
        for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
        {

            // Mesh�Ķ������
            Vertex vertex;


            // ����λ�� .Position
            glm::vec3 vector;
            vector.x = aimesh->mVertices[i].x;
            vector.y = aimesh->mVertices[i].y;
            vector.z = aimesh->mVertices[i].z;
            vertex.Position = vector;


            // ���㷨�� .Normal
            if (aimesh->HasNormals())
            {
                vector.x = aimesh->mNormals[i].x;
                vector.y = aimesh->mNormals[i].y;
                vector.z = aimesh->mNormals[i].z;
                vertex.Normal = vector;
            }


            // ����ӳ�� .TexCoord���Լ�.Tangent �� .Bitangent
            if (aimesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
            {
                // cout << "The Mesh has TexCoord" << endl;
                // Assimp���֧��8��uv����8��uvӳ���ϵ������8�����ʣ����������ò�����ô��
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


    #pragma region ����

       // ��������(���붥���ϵ)
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
        {
            aiFace face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

    #pragma endregion


    #pragma region ����

        // cout << *aimesh->mNumUVComponents << endl;
        // �������
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
        // ���û�в��ʣ���ָ��ΪĬ�ϲ���                                                               
        //if (textures.size() == 0)
        else
        {
            cout << "Use Defualt Texture..." << endl;
            // textures.push_back(default_texture);
        }
        
        cout << "Texture Number:" << textures.size() << endl;

    #pragma endregion

        //����ΪMesh����
        // cout << "Size of vertices=" << vertices.size() << " Size of indices=" << indices.size() << " Size of textures=" << textures.size() << endl;
        return Mesh(vertices, indices,textures);
    
}
    
    // �·�����
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

            // ����·������ʵ�ӳ���ϵ
            std::string absolutePath = this->directory + "/" + textPath.C_Str();
            map<string, Texture>::const_iterator it = this->loadedTextureMap.find(absolutePath);

            if (it == this->loadedTextureMap.end()) // ����Ƿ��Ѿ����ع���
            {
                // �������û�б����أ��������
                //GLuint textId = TextureHelper::load2DTexture(absolutePath.c_str());
                text.id = TextureFromFile(textPath.C_Str(), directory);// id���������Դ��е�ID
                text.path = absolutePath;// ·�������·��
                text.type = textureType;
                textures.push_back(text);
                textures_loaded.push_back(text);
                loadedTextureMap[absolutePath] = text;
            }
            else
            {
                // ���Ѿ����ص��ڴ棬��ֱ�Ӵ��ڴ��ȡ
                textures.push_back(it->second);
            }
        }
        return true;
    }


   
public:
    vector<Mesh> meshes;// һ��ģ���ж��Mesh
    vector<Texture> textures_loaded;// �Ѽ��ص��������ظ�����ʱ����ֱ��ȡ��

    int VertNum;
    int FaceNum;


    // ���캯��������ģ��
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

    // ʹ��shader����model����������mesh��
    void Draw(Shader shader)
    {
        // meshes��vector���飬������Ⱦ����
        // cout << "Number of Meshes:" << meshes.size() << endl;
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    // �ͷŻ���
    void Release()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Release();
    }

};

// ��ȡ�������ص��Դ��У��������Դ��е�ID
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