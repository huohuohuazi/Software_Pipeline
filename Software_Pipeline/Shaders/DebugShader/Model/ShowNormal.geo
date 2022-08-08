#version 330 core


layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
// 传递集合着色器

uniform mat4 projection;

in v2g_Struct{
    vec3 Normal;
}v2g_in[];


const float MAGNITUDE = 0.02;// 线段长度？


void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = projection * (gl_in[index].gl_Position + vec4(v2g_in[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}


void main() {    
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}