#version 430 core
layout (triangles) in;// ����ÿ��ƬԪ���ֱ𱣴�������ͼ
layout (triangle_strip, max_vertices=18) out;

// �����VP����
uniform mat4 lightSpaceMatrix[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    // ÿ�������β������Σ���������
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        // ���������ε�ÿ�����㣬����������������λ����Ϣ����ƬԪ��ɫ���н����һ��Ϊ��ȣ�
        for(int i = 0; i < 3; ++i)
        {
            // FragPos������ռ�����(M*aPos)
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightSpaceMatrix[face] * FragPos;
            EmitVertex();// Emitһ������Ϊgl_Position�Ķ��㣬���͸�frag_shader
        }    
        EndPrimitive();
    }
}