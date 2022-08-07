#version 330 core

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    // 视角方向
    vec3 view_diretion = normalize(Position - cameraPos);
    // 反射方向
    vec3 relfect_dirction = reflect(view_diretion, normalize(Normal));
    // 像天空盒采样
    FragColor = vec4(texture(skybox, relfect_dirction).rgb, 1.0);
}