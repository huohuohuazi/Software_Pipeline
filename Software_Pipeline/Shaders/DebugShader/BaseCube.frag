#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
uniform sampler2D texture1;

void main()
{    
    FragColor = texture(texture1, TexCoords);
    //FragColor=vec4(0,1,1,1);
}