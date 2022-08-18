#version 430 core

// ���ݼ�����ɫ��
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in v2g_Struct
{
    vec3 color;
}v2g_in[];

out vec3 fcolor;// ���ݸ�ƬԪ��ɫ��

void build_house(vec4 position)
{    
    fcolor=v2g_in[0].color;
    // ����һ���������Σ��¶��㣿
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:����
    EmitVertex();

    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:����
    EmitVertex();

    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:����
    EmitVertex();

    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:����
    EmitVertex();

    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:����
    fcolor=vec3(1.0,1.0,1.0);
    EmitVertex();
    
    EndPrimitive();
}


void main() {    
    build_house(gl_in[0].gl_Position);
}