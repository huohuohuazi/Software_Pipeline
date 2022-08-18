#version 430 core


layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
// ���ݼ�����ɫ��

uniform mat4 projection;

in v2g_Struct{
    vec3 Normal;
}v2g_in[];


const float MAGNITUDE = 0.02;// �߶γ��ȣ�


void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = projection * (gl_in[index].gl_Position + vec4(v2g_in[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}


void main() {    
    GenerateLine(0); // ��һ�����㷨��
    GenerateLine(1); // �ڶ������㷨��
    GenerateLine(2); // ���������㷨��
}