#version 450

//----------------------inputs----------------------
layout (location = 0) in vec4 fragPos;
//layout(location = 1) in vec2 fragTexCoord;
layout(location = 1) in vec3 normal;


//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout(binding = 1) uniform MaterialUniform {
    vec4 color;
    bool hasTexture;
} material;



void main() {
    /* if (material.hasTexture) {

     } else {
         vec3 lightColor = vec3(1, 1, 1);
         float ambientStrength = 0.1;

         vec3 ambient = ambientStrength * vec3(1, 1, 1);
         vec3 lightDir = normalize(vec3(0.5,-0.5,0.0));
         float diff = max(dot(normal, lightDir), 0.0);
         vec3 diffuse = diff * lightColor;
         vec3 result = (ambient + diffuse) * material.color.rgb;
         outColor =vec4(result, 1.0);*
     }*/
    outColor = vec4(1,0,0,1);
}