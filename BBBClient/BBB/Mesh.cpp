#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh
(
	vector<Vertex> vertices, 
	vector<GLuint> indices,
	vector<Texture> textures
)
	: vertices{ move(vertices) }
	, indices{ move(indices) }
	, textures{ move(textures) }
{
	setup_mesh();
}

void Mesh::draw(const ShaderPtr& shader)const
{
	GLuint albedo_n = 1;
	GLuint specular_n = 1;
	for (const auto& t : textures)
	{
		if ("albedo" == t.type)
		{
			shader->set("u_material." + t.type + to_string(albedo_n++), t);
		}
		else if ("specular" == t.type)
		{
			shader->set("u_material." + t.type + to_string(specular_n++), t);
		}
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setup_mesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, nor));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, tex));

	glBindVertexArray(0);
}
