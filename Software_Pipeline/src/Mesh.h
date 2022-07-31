#ifndef MESH_H
#define MESH_H

// 包含glad来获取所有的必须OpenGL头文件
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// 每个顶点最多受到多少骨骼影响
#define MAX_BONE_INFLUENCE 4

// 顶点结结构
struct Vertex {
    glm::vec3 Position;// aPos
    glm::vec3 Normal;// aNormal
    glm::vec2 TexCoords;// aTexCoords
    // 可以有多个uv映射关系，这里只放了一个
    // TexCoords是映射关系，不是纹理

    //glm::vec3 Tangent;
    // bitangent
    //glm::vec3 Bitangent;

    //int m_BoneIDs[MAX_BONE_INFLUENCE];// 影响顶点的骨骼ID
    //float m_Weights[MAX_BONE_INFLUENCE];// 骨骼权重
};

struct Texture {
    unsigned int id;// 于显存中的ID
    string type;// 图片类型有GL_RGB,GL_RGBA,GL_RED
    aiString path;// 每次加载纹理都保存一下路径，后续加载相同纹理时直接从显存取用即可
};

class Mesh {
     
private:
    unsigned int VAO, VBO, EBO;

    // 设置VBO，批量将顶点、图片等数据写入显存
    void setupMesh()
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
     

        /*--------------------------------------------------------------------------------------------*/
        
        // VBO是数据的集合，包括了顶点、索引、uv坐标等信息，整合发送给GPU显存
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        // 因为是static类型，因此只写入一次
        
        /*--------------------------------------------------------------------------------------------*/
       
        // EBO为顶点索引
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);
        
        /*--------------------------------------------------------------------------------------------*/
        
        // VAO描述了如何将整合的信息分配给顶点着色器的各个属性
        glBindVertexArray(VAO);

        // 顶点位置
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        /*   对于index = i(0, 1, 2...属性的索引), 属性长度 = alloc[i](vec2 / vec3)
             type=float,是否标准化
             步长=maxLength*sizeof(float)，一个顶点所包含数据所占的byte大小
             该属性在步长里的偏移量为sum*sizeof(float)   */ 

        // 顶点法线
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        
        // 顶点纹理坐标
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        
        /*--------------------------------------------------------------------------------------------*/

        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        
        /*--------------------------------------------------------------------------------------------*/

        // vertex bitangent
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        
        /*--------------------------------------------------------------------------------------------*/

        
        // 影响的骨骼
        //glEnableVertexAttribArray(5);
        //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // 权重
        //glEnableVertexAttribArray(6);
        //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        /*--------------------------------------------------------------------------------------------*/


        glBindVertexArray(0);
    }


public:
    // 网格
    vector<Vertex> vertices;// aPos
    vector<unsigned int> indices;// aNormal
    vector<Texture> textures;// aTexCoords
    
    // 从Model中获取数据
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }
    
    void Draw(Shader shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        // 对于该mesh的所有纹理
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // 激活相应的纹理单元

            string number;// 获取纹理在该纹理种类(diffuse/specular/normal)中的编号
            string name = textures[i].type;

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            /*else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); 
            else if (name == "texture_height")
                number = std::to_string(heightNr++);*/

            // shader.setInt(("material." + name + number).c_str(), i);
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }


        // 绘制网格
        glBindVertexArray(VAO);// 调用显存中的网格数据
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    void Release()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &EBO);
        glDeleteBuffers(1, &VBO);
    }

};



#endif
