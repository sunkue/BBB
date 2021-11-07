#pragma once

#include "Mesh.h"


class Model
{
public:
	Model(string_view path)
	{
		load_model(path);
	}

	void draw(const ShaderPtr& shader)const;

private:
	vector<Mesh> meshes;
	string directory;

private:
	void load_model(string_view path);
	void process_node(aiNode* node, const aiScene* scene);
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, string typeName);

};

