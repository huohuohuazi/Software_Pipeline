#version 330 core
uniform float time;
in vec3 Color;
out vec4 FragColor;
void main(){
FragColor = vec4(
(sin(time*-0.5 + Color.x) + 1)/2,
(sin(time*1.5+Color.y * 2+sin(time - 7.2)* sin(time / 1.8) + 2) + 1)/2,
(sin(time*1.23+Color.z * 2.5) + 1)/2,
    1.0f);
}
