#pragma once

#include "Mesh.h"
#include "Shader.h"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

using ModelPtr = shared_ptr<class Model>;

class Model
{
public:
	CREATE_SHARED(ModelPtr, Model);

	void draw(const ShaderPtr& shader)const;
private:
	Model(string_view path)
	{
		load_model(path);
	}


private:
	vector<Mesh> meshes;
	string directory;
	vector<TexturePtr> textures_loaded;

private:
	void load_model(string_view path);
	void process_node(aiNode* node, const aiScene* scene);
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	vector<TexturePtr> load_material_textures(aiMaterial* mat, aiTextureType type, string typeName);

};

