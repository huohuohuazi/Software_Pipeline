#version 330 core
layout (triangles) in;// 对于每个片元，分别保存六张贴图
layout (triangle_strip, max_vertices=18) out;

// 六面的VP矩阵
uniform mat4 lightSpaceMatrix[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    // 每个三角形采样六次（六个方向）
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        // 对于三角形的每个顶点，保存其对于六个面的位置信息（在片元着色器中将会归一化为深度）
        for(int i = 0; i < 3; ++i)
        {
            // FragPos是世界空间坐标(M*aPos)
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightSpaceMatrix[face] * FragPos;
            EmitVertex();// Emit一个坐标为gl_Position的顶点，发送给frag_shader
        }    
        EndPrimitive();
    }
}