#include "stdafx.h"
/*
	assimp 라이브러리를 사용하고 있음,, 5.0 버전임.
*/
#pragma comment(lib,"./Dependencies/assimp/assimp-vc142-mt.lib")
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
		, aiProcess_Triangulate | aiProcess_FlipUVs);

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
		meshes.push_back(process_mesh(mesh, scene));
	}
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

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
		vector<Texture> albedo_maps = load_material_textures(material
			, aiTextureType_DIFFUSE, "albedo");
		textures.insert(textures.end(), albedo_maps.begin(), albedo_maps.end());
		vector<Texture> specular_maps = load_material_textures(material
			, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh(move(vertices), move(indices), move(textures));
}

GLuint TextureFromFile(const char* path, const string& directory, bool gamma = false);

vector<Texture> Model::load_material_textures(
	aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	std::string Dir{ "./Resource/Texture" };
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		//texture.id = CreatePngTexture(Dir.append(str.C_Str()).c_str());
		texture.id = TextureFromFile(str.C_Str(), Dir.c_str());
		texture.type = typeName;
		textures.push_back(texture);
	}
	return textures;
}


GLuint TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}