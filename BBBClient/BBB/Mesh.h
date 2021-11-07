#pragma once

/* max = 256 */

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

/// //////////////////////////////////////

using MaterialPtr = shared_ptr<struct Material>;

struct Material
{
	Texture albedo;
	Texture specular;
	float shininess;

	CREATE_SHARED(MaterialPtr, Material);
private:
	Material(Texture albedoTex, Texture specularTex, float shininess = 32.f) noexcept
		: albedo{ albedoTex }, specular{ specularTex }, shininess{ shininess }{};
};

/// //////////////////////////////////////

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	
public:
	Mesh( vector<Vertex> vertices
		, vector<GLuint> indices
		, vector<Texture> textures);
	
	void draw(const ShaderPtr& shader)const;

private:
	void setup_mesh();

private:
	GLuint vao, vbo, ebo;
};



const Vertex box[36]
{
	/* +Y */
	{ {-1,  1, -1}, Y_DEFAULT, {0, 0}},
	{ {-1,  1,  1}, Y_DEFAULT, {1, 0}},
	{ { 1,  1, -1}, Y_DEFAULT, {0, 1}},

	{ { 1,  1, -1}, Y_DEFAULT, {0, 1}},
	{ {-1,  1,  1}, Y_DEFAULT, {1, 0}},
	{ { 1,  1,  1}, Y_DEFAULT, {1, 1}},
	/* +X */
	{ { 1,  1,  1}, X_DEFAULT, {0, 1}},
	{ { 1, -1,  1}, X_DEFAULT, {0, 0}},
	{ { 1, -1, -1}, X_DEFAULT, {1, 0}},

	{ { 1, -1, -1}, X_DEFAULT, {1, 0}},
	{ { 1,  1, -1}, X_DEFAULT, {1, 1}},
	{ { 1,  1,  1}, X_DEFAULT, {0, 1}},
	/* +Z */
	{ { 1,  1,  1}, Z_DEFAULT, {1, 1}},
	{ {-1,  1,  1}, Z_DEFAULT, {0, 1}},
	{ { 1, -1,  1}, Z_DEFAULT, {1, 0}},

	{ { 1, -1,  1}, Z_DEFAULT, {1, 0}},
	{ {-1,  1,  1}, Z_DEFAULT, {0, 1}},
	{ {-1, -1,  1}, Z_DEFAULT, {0, 0}},
	/* -X */
	{ {-1, -1,  1}, -X_DEFAULT, {1, 0}},
	{ {-1,  1,  1}, -X_DEFAULT, {1, 1}},
	{ {-1, -1, -1}, -X_DEFAULT, {0, 0}},

	{ {-1, -1, -1}, -X_DEFAULT, {0, 0}},
	{ {-1,  1,  1}, -X_DEFAULT, {1, 1}},
	{ {-1,  1, -1}, -X_DEFAULT, {0, 1}},
	/* -Z */
	{ {-1,  1, -1}, -Z_DEFAULT, {1, 1}},
	{ { 1,  1, -1}, -Z_DEFAULT, {0, 1}},
	{ {-1, -1, -1}, -Z_DEFAULT, {1, 0}},

	{ {-1, -1, -1}, -Z_DEFAULT, {1, 0}},
	{ { 1,  1, -1}, -Z_DEFAULT, {0, 1}},
	{ { 1, -1, -1}, -Z_DEFAULT, {0, 0}},
	/* -Y */
	{ { 1, -1, -1}, -Y_DEFAULT, {0, 1}},
	{ { 1, -1,  1}, -Y_DEFAULT, {1, 1}},
	{ {-1, -1,  1}, -Y_DEFAULT, {1, 0}},

	{ {-1, -1,  1}, -Y_DEFAULT, {1, 0}},
	{ {-1, -1, -1}, -Y_DEFAULT, {0, 0}},
	{ { 1, -1, -1}, -Y_DEFAULT, {0, 1}}
};



const Vertex cross_billboard_3[18]
{
	/* - */
	{ {HALF_ROOT3,  2, 0}, Y_DEFAULT, {0, 1}},
	{ {HALF_ROOT3, 0,  2}, Y_DEFAULT, {1, 0}},
	{ {HALF_ROOT3,  2,  2}, Y_DEFAULT, {1, 1}},

	{ {HALF_ROOT3,  2, 0}, Y_DEFAULT, {0, 1}},
	{ {HALF_ROOT3, 0, 0}, Y_DEFAULT, {0, 0}},
	{ {HALF_ROOT3, 0,  2}, Y_DEFAULT, {1, 0}},

	/* / */
	{ {0,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ { ROOT3, 0,  1.5}, Y_DEFAULT, {1, 0}},
	{ { ROOT3,  2,  1.5}, Y_DEFAULT, {1, 1}},

	{ {0,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ {0, 0, 0.5}, Y_DEFAULT, {0, 0}},
	{ { ROOT3, 0,  1.5}, Y_DEFAULT, {1, 0}},

	/* \ */
	{ { ROOT3,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ {0, 0,  1.5}, Y_DEFAULT, {1, 0}},
	{ {0,  2,  1.5}, Y_DEFAULT, {1, 1}},

	{ { ROOT3,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ { ROOT3, 0, 0.5}, Y_DEFAULT, {0, 0}},
	{ {0, 0,  1.5}, Y_DEFAULT, {1, 0}}
};
