#version 430 core
in vec4 FragPos;
// ����һ�����
// û��out
uniform vec3 lightPos;
uniform float far_plane;


void main()
{
    // 
    float lightDistance = length(FragPos.xyz - lightPos);
    // ��һ�����룬����Ⱦʱ��Ҫ����far_plane���бȽ�
    lightDistance = lightDistance / far_plane;

    // ��Ϊ�������
    gl_FragDepth = lightDistance;
    // gl_FragDepth=1.0f;
    // �ڵ�һ�����shadowMap�У�Ĭ������ô����
    // gl_FragDepth = gl_FragCoord.z;
}