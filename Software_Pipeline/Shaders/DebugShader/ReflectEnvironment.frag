#version 430 core

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    // �ӽǷ���
    vec3 view_diretion = normalize(Position - cameraPos);
    // ���䷽��
    vec3 relfect_dirction = reflect(view_diretion, normalize(Normal));
    // ����պв���
    FragColor = vec4(texture(skybox, relfect_dirction).rgb, 1.0);
}