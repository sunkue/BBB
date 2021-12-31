#version 450

nouse!!!!!!!!!!!!!!!!!

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 normal;
    vec2 texcoord;
} vs_in[];

out FS_IN {
	vec3 normal;
    vec2 texcoord;
} gs_out;

uniform float time;
uniform int explosion;

vec4 explode(vec4 position, vec3 normal)
{
    return position;
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + explosion * vec4(direction, 0.0);
} 

vec3 get_normal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
} 

void main() {    
    vec3 normal = get_normal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    gs_out.normal = vs_in[0].normal;
    gs_out.texcoord = vs_in[0].texcoord;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    gs_out.normal = vs_in[1].normal;
    gs_out.texcoord = vs_in[1].texcoord;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    gs_out.normal = vs_in[2].normal;
    gs_out.texcoord = vs_in[2].texcoord;
    EmitVertex();
    EndPrimitive();
}  
