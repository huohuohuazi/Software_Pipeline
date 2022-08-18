#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

void main()
{   

    FragColor = texture(texture_diffuse1, TexCoords);
    
     //FragColor = vec4(1.0,1.0,1.0,1.0);
}