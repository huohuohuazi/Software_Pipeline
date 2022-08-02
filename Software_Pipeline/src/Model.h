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
#include "Mesh.h"
#include "stb_image.h"

// �������ܲ���һ��ģ�Ͷ��еģ�����Ϊ˽�б�����
vector<Texture> textures_loaded;// �Ѽ��ص����������ظ�����ʱ����ֱ��ȡ��
Texture default_texture;

// ��ȡ���������ص��Դ��У��������Դ��е�ID
unsigned int TextureFromFile(const char* path, const string& directory);


class Model
{
private:
    vector<Mesh> meshes;
    string directory;// ģ�͵�Ŀ¼������obj��mtl��������ͼ
    

    // ����ģ��
    void loadModel(string path)
    {
        std::cout << "Start Loading Model..." << endl;

        Assimp::Importer importer;
        // ����Importer�ķ���
        const aiScene* scene = importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
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

        loadDefaultTexture();

        // �ݹ鴦�����нڵ�
        processNode(scene->mRootNode, scene);
        
    }

    // Ĭ�ϲ��ʣ��Ժ��õĶ�
    void loadDefaultTexture()
    {
        
        string pa = "1001_albedo.jpg";
        //string dir = pa.substr(0, pa.find_last_of('/'));
        string dir = "resources/objects/survival-guitar-backpack/textures";
        default_texture.id = TextureFromFile(pa.c_str(), dir);
        default_texture.type = "texture_diffuse";
        default_texture.path = "resources/objects/survival-guitar-backpack/textures/1001_albedo.jpg";

        textures_loaded.push_back(default_texture);
    }

    // ����ģ�����нڵ㣬���ڵ��assimp��aiMesh��ʽ������ΪopenGL��Mesh��ʽ����д��vector
    void processNode(aiNode* node, const aiScene* scene)
    {
        std::cout << "����ת��Mesh���ڵ�����" << node->mNumMeshes << endl;
        // ���ڵ��assimp��aiMesh��ʽ������ΪopenGL��Mesh��ʽ����д��vector
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



        // ����aiMesh�Ķ��㣬��������λ�á����ߺ���������uv
        //std::cout << "aimesh->mNumVertices = " << aimesh->mNumVertices << endl;
        //std::cout << " x= " << aimesh->mVertices[0].x << " y= " << aimesh->mVertices[0].y << " z= " << aimesh->mVertices[0].z << endl;
        std::cout << "Process Vertices:" << endl;
        for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
        {
            // Mesh�Ķ�����
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
            

            // �������� .TexCoord
            if (aimesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
            {
                // Assimp���֧��8��uv����8��uvӳ���ϵ������8�����ʣ����������ò�����ô��
                glm::vec2 vec;
                vec.x = aimesh->mTextureCoords[0][i].x;
                vec.y = aimesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                // tangent
                vector.x = aimesh->mTangents[i].x;
                vector.y = aimesh->mTangents[i].y;
                vector.z = aimesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = aimesh->mBitangents[i].x;
                vector.y = aimesh->mBitangents[i].y;
                vector.z = aimesh->mBitangents[i].z;
                vertex.Bitangent = vector;

            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                
            vertices.push_back(vertex);
        }


        
        std::cout << "Process indices:" << endl;
        // ��������(���붥���ϵ)
        // std::cout << "aimesh->mNumFaces = " << aimesh->mNumFaces << endl;
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
        {
            aiFace face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }



        // cout << *aimesh->mNumUVComponents << endl;
        // ��������
        std::cout << "Process materials:" << aimesh->mMaterialIndex << endl;
        if (aimesh->mMaterialIndex >= 0)
        {
            // cout<<"Processing Material" << endl;
            // assimp��material
            aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];
            
            // ������ͼ
            vector<Texture> diffuseMaps = loadMaterialTextures(aimaterial,
                aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            // �߹���ͼ
            vector<Texture> specularMaps = loadMaterialTextures(aimaterial,
                aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // ������ͼ
            std::vector<Texture> normalMaps = loadMaterialTextures(aimaterial,
                 aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            
            // �߶���ͼ
            std::vector<Texture> heightMaps = loadMaterialTextures(aimaterial, 
                aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
   
        }
        /*
        
        else
        {
            Texture texture;

            // id���������Դ��е�ID
            texture.id = TextureFromFile("resources/textures/container2.png", directory);
            texture.type = "texture_diffuse";
            texture.path = "";
            textures.push_back(texture);
            //textures_loaded.push_back(texture); // ���ӵ��Ѽ��ص�������

        }
        */

        // Ĭ�ϲ���
        if (textures.size() == 0)
        {
            cout << "Use Defualt Texture..." << endl;
            textures.push_back(default_texture);
        }


        //cout <<"MeshTextureSize = "<< textures.size() << endl;
        cout << "Size of vertices=" << vertices.size() << " Size of indices=" << indices.size() << " Size of textures=" << textures.size() << endl;
        return Mesh(vertices, indices,textures);
    }
    

    // ��assimp�Ĳ���ת��Ϊmesh��ʽ��һ�β���ͬ�����͵Ĳ�����ͼ
    vector<Texture> loadMaterialTextures(aiMaterial* aimat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
  
        /* 
        // ������������
        for (unsigned int i = 0; i < aimat->GetTextureCount(type); i++)
        {
            aiString str;
            aimat->GetTexture(type, i, &str);

            cout << "String:" << str.C_Str() << endl;

            // ����������Ϣ
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
        }
       // return textures;
         */   
        
        // std::cout << "Nunber of Property:: " << aimat->mNumProperties << endl;
        // cout << aimat->mProperties[0]->mKey.C_Str() << aimat->mProperties[1]->mKey.C_Str() << endl;
        //std::cout << "Type : " << type <<" Num:" << aimat->GetTextureCount(type) << endl;
        // ������һ���Ż�������һ���������ڱ����Ѿ����ع�������������ֱ�Ӵ��ڴ�/�Դ���ȡ��������IO��ȡ
        
        cout << "Type: " << type << " = " << aimat->GetTextureCount(type);
        for (unsigned int i = 0; i < aimat->GetTextureCount(type); i++)
        {

            aiString str;
            aimat->GetTexture(type, i, &str);
            bool skip = false;
            std::cout << "String:" << str.C_Str() << endl;
            // ÿ�μ���ʱ����һ���Ѽ��ص��������ƣ������ظ�������
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                // string_compare
                if (std::strcmp(textures_loaded[j].path.data, str.C_Str()) == 0)
                {
                    std::cout << "SKIP" << endl;
                    // �����ظ��ģ���ֱ�Ӵ������ж�ȡ
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {   
                cout << "No SKIP" << endl;
                // ���������û�б����أ��������
                Texture texture;

                // id���������Դ��е�ID
                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // ���ӵ��Ѽ��ص�������
            }
        }
        
        return textures;

    }

public:
    
    // ���캯��������ģ��
    Model(const char* path)
    {
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

// ��ȡ���������ص��Դ��У��������Դ��е�ID
unsigned int TextureFromFile(const char* path, const string& directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
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