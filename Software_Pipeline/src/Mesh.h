#ifndef MESH_H
#define MESH_H

// ����glad����ȡ���еı���OpenGLͷ�ļ�
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// ÿ����������ܵ����ٹ���Ӱ��
#define MAX_BONE_INFLUENCE 4

// �����ṹ
struct Vertex {
    glm::vec3 Position;// aPos
    glm::vec3 Normal;// aNormal
    glm::vec2 TexCoords;// aTexCoords
    // �����ж��uvӳ���ϵ������ֻ����һ��
    // TexCoords��ӳ���ϵ����������

    //glm::vec3 Tangent;
    // bitangent
    //glm::vec3 Bitangent;

    //int m_BoneIDs[MAX_BONE_INFLUENCE];// Ӱ�춥��Ĺ���ID
    //float m_Weights[MAX_BONE_INFLUENCE];// ����Ȩ��
};

struct Texture {
    unsigned int id;// ���Դ��е�ID
    string type;// ͼƬ������GL_RGB,GL_RGBA,GL_RED
    aiString path;// ÿ�μ�����������һ��·��������������ͬ����ʱֱ�Ӵ��Դ�ȡ�ü���
};

class Mesh {
     
private:
    unsigned int VAO, VBO, EBO;

    // ����VBO�����������㡢ͼƬ������д���Դ�
    void setupMesh()
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
     

        /*--------------------------------------------------------------------------------------------*/
        
        // VBO�����ݵļ��ϣ������˶��㡢������uv�������Ϣ�����Ϸ��͸�GPU�Դ�
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        // ��Ϊ��static���ͣ����ֻд��һ��
        
        /*--------------------------------------------------------------------------------------------*/
       
        // EBOΪ��������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);
        
        /*--------------------------------------------------------------------------------------------*/
        
        // VAO��������ν����ϵ���Ϣ�����������ɫ���ĸ�������
        glBindVertexArray(VAO);

        // ����λ��
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        /*   ����index = i(0, 1, 2...���Ե�����), ���Գ��� = alloc[i](vec2 / vec3)
             type=float,�Ƿ��׼��
             ����=maxLength*sizeof(float)��һ������������������ռ��byte��С
             �������ڲ������ƫ����Ϊsum*sizeof(float)   */ 

        // ���㷨��
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        
        // ������������
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

        
        // Ӱ��Ĺ���
        //glEnableVertexAttribArray(5);
        //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // Ȩ��
        //glEnableVertexAttribArray(6);
        //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        /*--------------------------------------------------------------------------------------------*/


        glBindVertexArray(0);
    }


public:
    // ����
    vector<Vertex> vertices;// aPos
    vector<unsigned int> indices;// aNormal
    vector<Texture> textures;// aTexCoords
    
    // ��Model�л�ȡ����
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

        // ���ڸ�mesh����������
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // ������Ӧ������Ԫ

            string number;// ��ȡ�����ڸ���������(diffuse/specular/normal)�еı��
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


        // ��������
        glBindVertexArray(VAO);// �����Դ��е���������
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
