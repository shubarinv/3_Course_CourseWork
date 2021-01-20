#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "texture.hpp"

class ObjLoader {
 public:
  struct loadedOBJ {
	std::vector<uint> indices;
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::string diffuse_texname;
  };
 private:
  static std::vector<loadedOBJ> doTheSceneProcessing(const aiScene* scene) {
	std::vector<loadedOBJ> loadedMeshes;
	std::vector<Texture> textures;
	LOG_S(INFO) << "Meshes: " << scene->mNumMeshes;
	for (int i = 0; i < scene->mNumMeshes; ++i) {
	  LOG_S(INFO) << "Mesh(" << i << ")";
	  auto mesh = scene->mMeshes[i];
	  uint num_faces = mesh->mNumFaces;

	  std::vector<uint> indices;
	  std::vector<float> vertices;
	  std::vector<float> texCoords;
	  std::vector<float> normals;

	  // trying to extract indices
	  for (int j = 0; j < num_faces; ++j) {
		auto face = mesh->mFaces[j];
		for (int k = 0; k < face.mNumIndices; ++k) {
		  auto uv = mesh->HasTextureCoords(0) ?mesh->mTextureCoords[0][face.mIndices[k]]:aiVector3D(0.0f, 0.0f, 0.0f);
		  texCoords.push_back(uv.x);
		  texCoords.push_back(uv.y);

		  auto vertex = mesh->mVertices[face.mIndices[k]];

		  vertices.push_back(vertex.x);
		  vertices.push_back(vertex.y);
		  vertices.push_back(vertex.z);

		  auto normal = mesh->mNormals[face.mIndices[k]];

		  normals.push_back(normal.x);
		  normals.push_back(normal.y);
		  normals.push_back(normal.z);
		}
	  }

	  LOG_S(INFO) << "vertices: " << vertices.size();
	  LOG_S(INFO) << "texCoords: " << texCoords.size();
	  //LOG_S(INFO) << "indices: " << indices.size();
	  LOG_S(INFO) << "normals: " << normals.size();
	  LOG_S(INFO) << "---------------";
	  loadedMeshes.push_back({indices, vertices, texCoords, normals});
	}
	/*
	// loading textures
	for (int i = 0; i < scene->mNumMaterials; ++i) {
	  auto material = scene->mMaterials[i];
	  int texIndex = 0;
	  aiString path;// filename
	  if(material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS){
		LOG_S(INFO) << path.C_Str();
	  }
	}
	return{};*/
	// todo load materials
	return loadedMeshes;
  }

  // C++ importer interface
  // Output data structure
  // Post processing flags
  std::vector<loadedOBJ> doTheImportThing(const std::string& pFile) {
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
											 //aiProcess_CalcTangentSpace |
											 aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords | aiProcess_GenNormals);
	// If the import failed, report it
	if (!scene) {
	  LOG_S(ERROR) << "Failed to load file: " << importer.GetErrorString();
	  return {};
	}
	// Now we can access the file's contents.
	return doTheSceneProcessing(scene);
  }

 public:
  std::vector<loadedOBJ> loadObj(const std::string& filename) {
	return doTheImportThing(filename);
  }

};