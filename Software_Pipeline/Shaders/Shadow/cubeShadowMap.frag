#version 330 core
in vec4 FragPos;
// 采样一次深度
// 没有out
uniform vec3 lightPos;
uniform float far_plane;


void main()
{
    // 
    float lightDistance = length(FragPos.xyz - lightPos);
    // 归一化距离，在渲染时需要乘上far_plane进行比较
    lightDistance = lightDistance / far_plane;

    // 人为设置深度
    gl_FragDepth = lightDistance;
   // gl_FragDepth=1.0f;
    // 在单一方向的shadowMap中，默认是这么做的
    // gl_FragDepth = gl_FragCoord.z;
}