#version 430 core

struct Light {
    // 平行光不管位置，只看方向
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

