#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} frag_in;

uniform sampler2D texture_diffuse1;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;// 是否绘制阴影

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // 片元到光源向量
    vec3 fragToLight = fragPos - lightPos; 

    // 待比较的片元的深度
    float currentDepth = length(fragToLight);


    // PCF
    //float bias = 0.05; 
   // float shadow = 0.0;//阴影值
    // float samples = 4.0;
    //float offset = 0.1;

    // 三个维度递增角度
    //for(float x = -offset; x < offset; x += offset / (samples * 0.5)){
        ///for(float y = -offset; y < offset; y += offset / (samples * 0.5)){
            //for(float z = -offset; z < offset; z += offset / (samples * 0.5)){
                // 用方向向量 采样 立方体深度贴图
                //float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
                // 由于方向是归一化的，因此要统一
                //closestDepth *= far_plane; 
                //if(currentDepth - bias > closestDepth)
                    //shadow += 1.0;
                //} } }
    //shadow /= (samples * samples * samples);

    // PCF优化：使用20个偏移量代替64次采样
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;

    float viewDistance = length(viewPos - fragPos);
    // 偏移半径
    // float diskRadius = 0.05;
    //根据观察者的距离来增加偏移半径了
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane; 
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    
    return shadow;
}

// Debug，用于查看shadwmap有没有问题
vec4 DebugShowDepth(vec3 fragPos)
{
    // 片元到光源向量
    vec3 fragToLight = fragPos - lightPos; 

    // 待比较的片元的深度
    float closestDepth = texture(depthMap, fragToLight).r; 
    float currentDepth = length(fragToLight);

    //vec3 color=texture(texture_diffuse1, frag_in.TexCoords).rgb;

    vec3 color=vec3(closestDepth);
    return  vec4(color, 1.0);
}

void main()
{           
    // 底色/纹理
    //vec3 color  =vec3(0.4);
    vec3 color = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightColor = vec3(0.4);
    // 环境光
    vec3 ambient = 0.2 * color;
    // 漫反射
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // 高光反射(Blinn_Phong光照模型)
    vec3 viewDir = normalize(viewPos - frag_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // 阴影
    float shadow = shadows ? ShadowCalculation(frag_in.FragPos) : 0.0;// 是否开启阴影                  
    shadow = min(shadow, 0.75);

    // 环境光不受阴影影响
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    FragColor = vec4(lighting, 1.0f);

    // Debug用 阴影二值化
    //lighting=(1.0 - shadow)*vec3(1.0);
    //FragColor = DebugShowDepth(frag_in.FragPos);

    
}