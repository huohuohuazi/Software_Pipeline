#version 430 core

layout (std140,binding = 2) uniform PointLightinfo
{
    float K_ambient;   //4  0
    vec3 lightPos;     //16 16
    vec3 lightColor;   //16 32
    bool IsBlinnPhong; //4  48
};

out vec4 FragColor;

in vert_OUT {
    vec3 FragPos;
    vec3 Normal;
} frag_in;

 
void main()
{
    vec3 color=lightColor;
    FragColor = vec4(color, 1.0);
} 