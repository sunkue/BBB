#include "stdafx.h"
#include "Renderer.h"
#include "Mesh.h"

Mesh::Mesh
(
	vector<Vertex> vertices,
	vector<GLuint> indices,
	vector<TexturePtr> textures
)
	: vertices_{ move(vertices) }
	, indices_{ move(indices) }
	, textures_{ move(textures) }
{
	setup_mesh();
	//vertices_.clear(); // ..
}

void Mesh::draw(const ShaderPtr& shader)const
{
	GLuint albedo_n = 1;
	GLuint specular_n = 1;
	for (const auto& t : textures_)
	{
		if ("albedo" == t->type)
		{
			shader->set("u_material." + t->type + to_string(albedo_n++), t);
		}
		else if ("specular" == t->type)
		{
			shader->set("u_material." + t->type + to_string(specular_n++), t);
		}
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setup_mesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), indices_.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, nor));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, tex));

	glBindVertexArray(0);
}


/// ////////////////////////////////////////////

CubeMap::CubeMap(const ShaderPtr& shader, const vector<string_view>& textures, const string_view dir)
	: cube_shader_{ shader }
	, cube_texture_{ load_cube_texture_file(textures, dir) }
	, vao_{ setup_mesh() }
{
}
void CubeMap::draw() const
{
	cube_shader_->use();
	auto& renderer = Renderer::get();
	glm::mat4 p = renderer.proj_mat();
	glm::mat4 v = glm::mat4(glm::mat3(renderer.get_main_camera()->view_mat()));
	glm::mat4 vp = p * v;
	cube_shader_->set("u_vp_mat", vp);
	glUniform1i(glGetUniformLocation(cube_shader_->get_shader_id(), 
		"u_cubemap"), cube_texture_);
	glActiveTexture(GL_TEXTURE0 + cube_texture_);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture_);
	glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

GLuint CubeMap::setup_mesh()
{
	static GLuint cached_vao = GLuint(-1);
	if (cached_vao != GLuint(-1))
	{
		return cached_vao;
	}

	GLuint vbo;
	glGenVertexArrays(1, &cached_vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(cached_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verticle), cube_verticle, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	return cached_vao;
}
