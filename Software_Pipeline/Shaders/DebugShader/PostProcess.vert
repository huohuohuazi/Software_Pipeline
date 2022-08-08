#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

//out vec2 TexCoords;

// �ýṹ�����
out Texcoords_Stuct{
    vec2 TexCoords;
} texcoords_out;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    texcoords_out.TexCoords = aTexCoords;
}