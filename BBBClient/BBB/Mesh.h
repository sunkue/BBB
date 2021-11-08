#pragma once


struct Vertex
{
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

/// //////////////////////////////////////


/// //////////////////////////////////////

class Mesh
{
private:
	vector<Vertex> vertices_;
	vector<GLuint> indices_;
	vector<TexturePtr> textures_;
	
public:
	Mesh( vector<Vertex> vertices
		, vector<GLuint> indices
		, vector<TexturePtr> textures);
	
	void draw(const ShaderPtr& shader)const;

private:
	void setup_mesh();

private:
	GLuint vao, vbo, ebo;
};

/// //////////////////////////////////
using CubeMapPtr = unique_ptr<class CubeMap>;
class CubeMap
{
public:
	void draw()const;

	CREATE_UNIQUE(CubeMapPtr, CubeMap);
private:
	CubeMap(const ShaderPtr& shader
		, const vector<string_view>& textures, string_view dir);

private:
	static GLuint setup_mesh();

private:
	GLuint vao_;
	GLuint cube_texture_;
	ShaderPtr cube_shader_;
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


const float cube_verticle[108] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
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
