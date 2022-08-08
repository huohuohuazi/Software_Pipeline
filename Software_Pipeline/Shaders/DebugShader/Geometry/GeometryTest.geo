#version 330 core

// 传递集合着色器
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in v2g_Struct
{
    vec3 color;
}v2g_in[];

out vec3 fcolor;// 传递给片元着色器

void build_house(vec4 position)
{    
    fcolor=v2g_in[0].color;
    // 生成一个（三角形）新顶点？
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();

    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();

    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();

    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();

    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
    fcolor=vec3(1.0,1.0,1.0);
    EmitVertex();
    
    EndPrimitive();
}


void main() {    
    build_house(gl_in[0].gl_Position);
}