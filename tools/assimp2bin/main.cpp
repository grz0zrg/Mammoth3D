/**
 * This program load any formats supported by the assimp library
 * and export it to a binary format usable directly within the Mammoth3D engine.
 * 
 * Usage: assimb2bin input_file output_file
 * 
 * Note: only a small subset of data is exported, indices/vertices/normals/uvs/colors
 *       and the geometry is always assumed to be made of triangles (assimp triangulate anyway)
 * */


#include "../../include/assimp/Importer.hpp"
#include "../../include/assimp/scene.h"
#include "../../include/assimp/postprocess.h"
	
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

std::string doExport(const aiScene* scene, const std::string& output_filename);

int main(int argc, char **argv)
{
	std::cout << "assimp2bin.exe - Mammoth3D tool\n\n";

	if (argc != 3) {
		std::cout << "Invalid usage.\n\n";
		return 0;
	}
	
	Assimp::Importer importer;  
	const aiScene* scene = importer.ReadFile(argv[1], 
												aiProcess_CalcTangentSpace|
												aiProcess_Triangulate|
												aiProcess_JoinIdenticalVertices|
												aiProcess_SortByPType);   
	if(!scene) {
		std::cout << importer.GetErrorString() << std::endl;
		return 0;
	}
	
	std::cout << doExport(scene, std::string(argv[2]));

	return 1;
}

std::string doExport(const aiScene* scene, const std::string& output_filename) {
	if (scene->HasMeshes()) {
		std::string filename(output_filename);
		if (filename.empty()) {
			filename = "mesh";
		}

		filename += ".mm";
			
		std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
		if (!file.good() || !file.is_open()) {
			return std::string("Failed to export Mesh: " + filename);
		}
		
		const aiNode *root = scene->mRootNode;
		
		unsigned int numNodes = root->mNumChildren;
		file.write((char*)&numNodes, sizeof(numNodes));
		
		for (unsigned int i = 0; i < root->mNumChildren; i++) {
			const aiNode *child = root->mChildren[i];
			
			// ignore non meshs stuff for now
			unsigned int numMeshes = child->mNumMeshes;
			/*if (numMeshes == 0) {
				continue;
			}*/
			
			file.write((char*)&numMeshes, sizeof(numMeshes));
			
			aiMatrix4x4 tmatrix = root->mTransformation*child->mTransformation;
			file.write((char*)&tmatrix.a1, sizeof(float));
			file.write((char*)&tmatrix.a2, sizeof(float));
			file.write((char*)&tmatrix.a3, sizeof(float));
			file.write((char*)&tmatrix.a4, sizeof(float));
			file.write((char*)&tmatrix.b1, sizeof(float));
			file.write((char*)&tmatrix.b2, sizeof(float));
			file.write((char*)&tmatrix.b3, sizeof(float));
			file.write((char*)&tmatrix.b4, sizeof(float));
			file.write((char*)&tmatrix.c1, sizeof(float));
			file.write((char*)&tmatrix.c2, sizeof(float));
			file.write((char*)&tmatrix.c3, sizeof(float));
			file.write((char*)&tmatrix.c4, sizeof(float));
			file.write((char*)&tmatrix.d1, sizeof(float));
			file.write((char*)&tmatrix.d2, sizeof(float));
			file.write((char*)&tmatrix.d3, sizeof(float));
			file.write((char*)&tmatrix.d4, sizeof(float));
			
			for (unsigned int i = 0; i < child->mNumMeshes; i++) {
				const aiMesh *aimesh = scene->mMeshes[child->mMeshes[i]];

				unsigned int indicesCount = 0;
				unsigned int verticesCount = 0;
				unsigned int normalsCount = 0;
				unsigned int texcoordsCount = 0;
				unsigned int vColorsCount = 0;
			
				unsigned int numNormals = 0;
				unsigned int numUvs = 0;
				unsigned int numVColors = 0;
				
				if (aimesh->HasFaces()) {
					indicesCount = aimesh->mNumFaces * 3; // only triangles
				}

				if (aimesh->HasPositions()) {
					verticesCount = aimesh->mNumVertices * 3;
				}
				
				if (aimesh->HasNormals() && aimesh->mNormals != NULL) {
					numNormals = aimesh->mNumVertices;
					normalsCount = verticesCount;
				}
				
				if (aimesh->HasTextureCoords(0) && aimesh->mTextureCoords[0] != NULL) {
					numUvs = aimesh->mNumVertices;
					texcoordsCount = numUvs * 2;
				}
				
				if (aimesh->HasVertexColors(0) && aimesh->mColors[0] != NULL) {
					numVColors = aimesh->mNumVertices;
					vColorsCount = numVColors * 4;
				}

				file.write((char*)&indicesCount, sizeof(indicesCount));
				file.write((char*)&verticesCount, sizeof(verticesCount));
				file.write((char*)&normalsCount, sizeof(normalsCount));
				file.write((char*)&texcoordsCount, sizeof(texcoordsCount));
				file.write((char*)&vColorsCount, sizeof(vColorsCount));

				for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
					const aiFace *face = &aimesh->mFaces[i];
					
					for (unsigned int j = 0; j < 3; j++) {
						file.write((char*)&face->mIndices[j], sizeof(unsigned int));
					}
				}

				for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
					const aiVector3D *vertice = &aimesh->mVertices[i];
					
					file.write((char*)&vertice->x, sizeof(float));
					file.write((char*)&vertice->y, sizeof(float));
					file.write((char*)&vertice->z, sizeof(float));
				}

				for (unsigned int i = 0; i < numNormals; i++) {
					const aiVector3D *normal = &aimesh->mNormals[i];
					
					file.write((char*)&normal->x, sizeof(float));
					file.write((char*)&normal->y, sizeof(float));
					file.write((char*)&normal->z, sizeof(float));
				}

				for (unsigned int i = 0; i < numVColors; i++) {
					const aiColor4D *color = &aimesh->mColors[0][i];
					
					file.write((char*)&color->r, sizeof(float));
					file.write((char*)&color->g, sizeof(float));
					file.write((char*)&color->b, sizeof(float));
					file.write((char*)&color->a, sizeof(float));
				}

				for (unsigned int i = 0; i < numUvs; i++) {
					const aiVector3D *uv = &aimesh->mTextureCoords[0][i];
					
					file.write((char*)&uv->x, sizeof(float));
					file.write((char*)&uv->y, sizeof(float));
				}

				if (scene->HasMaterials()) {
					const aiMaterial *material = scene->mMaterials[aimesh->mMaterialIndex];
					
					aiString diffuse_filename;
					unsigned int diffuse_filename_length = 0;
					if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuse_filename) == AI_SUCCESS) {
						diffuse_filename_length = diffuse_filename.length+1;
						file.write((char*)&diffuse_filename_length, sizeof(unsigned int));
						file.write((char*)diffuse_filename.C_Str(), diffuse_filename_length);
					} else {
						file.write((char*)&diffuse_filename_length, sizeof(unsigned int));
					}
				}
			}
		}
		
		file.close();
	}
	
	return std::string("success");
}
