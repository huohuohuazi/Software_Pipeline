#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    vec4 texColor = texture(texture1, TexCoords);
    //if(texColor.a<0.1)
        //discard;//舍弃该片元，保证片段不进行下一步处理（不进入颜色缓冲）
    FragColor = texColor;
}