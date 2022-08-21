#version 430 core
out vec4 FragColor;

in VS_OUT{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
}frag_in;


// Textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;// White

// lights
uniform vec3 lightPositions;
uniform vec3 lightColors;

uniform vec3 cameraPos;

const float PI = 3.14159265359;


// Normal Map : Tangent Space -> World Space
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, frag_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(frag_in.WorldPos);
    vec3 Q2  = dFdy(frag_in.WorldPos);
    vec2 st1 = dFdx(frag_in.TexCoords);
    vec2 st2 = dFdy(frag_in.TexCoords);

    vec3 N   = normalize(frag_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


// NDF normal distribution function
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    // n * h
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);

    // NDF = a^2 / ( pi * (n * h)^2 * (a^2 -1) +1 )^2
    return a2 / (PI * denom * denom);
    // roughness->0   ==>   NDF->0   ==>   highlight area focus on a point 
}

// G geometry funcion
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    // Remapping
    float k = (r * r) / 8.0;
    // In IBL , k = r^2 /2
    float becovered = NdotV / ( NdotV * (1.0 - k) + k);

    return becovered;
    // roughness->MAX  ==>  becovered->MAX   ==>   shadow / obstruction
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    // 微平面间相互遮蔽的比率,这种相互遮蔽会损耗光线的能量。
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);// normal * viewDirection -->  Geometry Obstruction
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);// normal * lightDirection -->  Geometry Shadowing

    return ggx1 * ggx2;
}


// Fresnel
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    // clamp=(1-(h*v)^5)
}



// L0(p,w0)=Sigma( (Kdiffuse * c  /pi  +  Kspecular * D F G / (w0 * n)*(Wi * n)  ) * Li(p , wi) * n * wi  )dwi
// c = albedo , Ks - specular , Kd - diffuse , Li - LightPower (radiance)
// n = normal , wi = L = lightDirection , w0 = V = viewDirection
void main()
{		
    vec3 albedo     = pow(texture(albedoMap, frag_in.TexCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, frag_in.TexCoords).r;
    float roughness = texture(roughnessMap, frag_in.TexCoords).r;
    float ao        = texture(aoMap, frag_in.TexCoords).r;

    vec3 N = getNormalFromMap(); // normal
    vec3 V = normalize(cameraPos - frag_in.WorldPos);// w0 = viewDirection
 
    vec3 F0 = vec3(0.04); // Basic Reclection rate of plane
    F0 = mix(F0, albedo, metallic);// F0 = F0 * (1-matallic) + albedo * matallic;

    vec3 Lo = vec3(0.0);
    //for(int i = 0; i < 4; ++i) 
    //{

        // Radiance
        vec3 L = normalize(lightPositions - frag_in.WorldPos);// wi = LightDirection
        vec3 H = normalize(V + L);
        float distance = length(lightPositions - frag_in.WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors * attenuation;

        // Specular = Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness); // Normal Distribution Function --> D
        float G   = GeometrySmith(N, V, L, roughness); // --:>G (Geometry Function)
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);// --> F (Fresnel Rquation)
        // DFG / 4 * ( viewDirection * normal) * ( lightDirection * normal)
        vec3 specular = NDF * G * F / ( 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);
        
        
        vec3 Ks = F;// Ks = Fresnel
        vec3 Kd = vec3(1.0) - Ks;// only non-metals  have diffuse lighting
        Kd *= 1.0 - metallic; 

        // L0(p,w0)=Sigma( (Kd * c  /pi  +  Ks * D F G / (w0 * n)*(Wi * n)  ) * Li(p,wi) * n * wi  )dwi
        Lo += (Kd * albedo / PI + specular) * radiance * max(dot(N, L), 0.0);
    //}   
    
    // Ambient
    vec3 ambient = vec3(0.03) * albedo * ao;
    ambient =vec3(0.03) * albedo ;// 记得加上一张全白色的ao Map
    vec3 color = ambient + Lo;

    // HDR tone mapping & Gamma correct
    color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
    //FragColor=vec4(vec3(metallic),1.0);
}