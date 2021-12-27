#version 450

uniform sampler2D depthmap; 

in vec2 texcoord; 

out vec4 o_flagcolor;

void main() 
{ 
    float depthValue = texture(depthmap, texcoord).r;
    o_flagcolor = vec4(vec3(depthValue), 1.0);
} 

