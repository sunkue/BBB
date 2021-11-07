#include "stdafx.h"
/*
	assimp 라이브러리를 사용하고 있음,, 5.0 버전임.
*/
#pragma comment(lib,"./Dependencies/assimp/assimp-vc140-mt.lib")

#include "Dependencies/assimp/assimp/Importer.hpp";
#include "Dependencies/assimp/assimp/scene.h";
#include "Dependencies/assimp/assimp/postprocess.h";

#include "Model.h"


void Model::draw(const ShaderPtr& shader) const
{
	for (auto m : meshes)
	{
		m.draw(shader);
	}
}

void Model::load_model(string_view path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.data()
		, aiProcess_Triangulate);


	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode)
	{
		cerr << "ERROR::[Model::load_model]::ASSIMP::"
			<< importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(process_mesh(mesh, scene));
	}
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices; vertices.reserve(mesh->mNumVertices);
	vector<GLuint> indices; indices.reserve(mesh->mNumVertices);
	vector<TexturePtr> textures; textures.reserve(10);

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.pos = { mesh->mVertices[i].x , mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.nor = { mesh->mNormals[i].x , mesh->mNormals[i].y, mesh->mNormals[i].z };
		if (mesh->mTextureCoords[0])
		{
			vertex.tex = { mesh->mTextureCoords[0][i].x , mesh->mTextureCoords[0][i].y };
		}
		else
		{
			vertex.tex = { 0, 0 };
		}

		vertices.emplace_back(move(vertex));
	}

	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (0 <= mesh->mMaterialIndex)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<TexturePtr> albedo_maps = load_material_textures(material
			, aiTextureType_DIFFUSE, "albedo");
		textures.insert(textures.end(), albedo_maps.begin(), albedo_maps.end());
		vector<TexturePtr> specular_maps = load_material_textures(material
			, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}
	return Mesh(move(vertices), move(indices), move(textures));
}


vector<TexturePtr> Model::load_material_textures(
	aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<TexturePtr> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (const auto& t : textures_loaded)
		{
			if (t->path == str.C_Str())
			{
				textures.push_back(t);
				skip = true;
				break;
			}
		}
		if (false == skip)
		{
			auto texture = Texture::create();
			//texture.id = CreatePngTexture(Dir.append(str.C_Str()).c_str());
			texture->id = load_texture_file(str.C_Str(), directory);
			texture->type = typeName;
			texture->path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

