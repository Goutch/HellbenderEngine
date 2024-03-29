#version 450
//-----------------------------inputs---------------------------------
//vertices
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

//instanced
layout (location = 2) in mat4 inInstanceTransforms;

//----------------------------------outputs--------------------------
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out flat int fragInstanceID;

layout (binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

void main() {
    gl_Position = ubo.projection * ubo.view * inInstanceTransforms * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragInstanceID = gl_InstanceIndex;
}