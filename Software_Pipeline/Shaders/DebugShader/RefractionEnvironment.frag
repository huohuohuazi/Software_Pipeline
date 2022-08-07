#version 330 core

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    float ratio = 1.00 / 1.52;
    // 视角方向
    vec3 view_diretion = normalize(Position - cameraPos);
    // 折射方向
    vec3 refract_dirction = refract(view_diretion, normalize(Normal),ratio);
    // 像天空盒采样
    FragColor = vec4(texture(skybox, refract_dirction).rgb, 1.0);
}