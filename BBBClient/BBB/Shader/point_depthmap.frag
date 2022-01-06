#version 450

in vec4 position;

uniform vec3 u_lightpos;
uniform float u_far_plane;

void main()
{
	// get distance between fragment and light source 
	float lightDistance = length(position.xyz - u_lightpos);
	
	// map to [0;1] range by dividing by far_plane 
	lightDistance = lightDistance / u_far_plane;
	
	// write this as modified depth 
	gl_FragDepth = lightDistance;
}
