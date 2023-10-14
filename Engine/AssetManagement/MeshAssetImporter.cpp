#include "Engine/AssetManagement/MeshAssetImporter.hpp"

namespace Engine {
	MeshAssetImporter::MeshAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Mesh>(assetRegistry) {}

	const char* MeshAssetImporter::GetDefaultAssetExtension() const {
		return ".mesh";
	}

	std::shared_ptr<Asset> MeshAssetImporter::ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) {
		const std::shared_ptr<Mesh> meshToImport = std::dynamic_pointer_cast<Mesh>(assetToImport);
		if (!meshToImport) {
			DEBUG_ERROR("The asset trying to import is not of type Mesh!");
			return {};
		}

		if (!AssetRegistry::TryWriteCustomDataToAsset(meshToImport->GetGuid(), meshToImport, GetDefaultAssetExtension())) {
			DEBUG_ERROR("Failed to save imported mesh to disk!");
			return {};
		}

		return meshToImport;
	}

	std::shared_ptr<Asset> MeshAssetImporter::ImportAsset(const std::string& fullPath, const char* assetName, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return {};
	}

	std::shared_ptr<Asset> MeshAssetImporter::LoadAsset(const xg::Guid& assetGuid) const {
		std::shared_ptr<Asset> mesh;
		if (!AssetRegistry::TryLoadCustomDataFromAsset(assetGuid, GetDefaultAssetExtension(), mesh)) return {};

		return mesh;
	}

	std::shared_ptr<Mesh> MeshAssetImporter::ProcessRawData(const aiMesh* assimpMesh) {
		std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh());
		mesh->SetName(assimpMesh->mName.C_Str());

		const std::vector uvMappings = {
			&mesh->uvs,
			&mesh->uv2,
			&mesh->uv3,
			&mesh->uv4,
			&mesh->uv5,
			&mesh->uv6,
			&mesh->uv7,
			&mesh->uv8
		};

		//unpack vertices
		for (unsigned i = 0; i < assimpMesh->mNumVertices; i++) {
			//position vertex coords
			const aiVector3D& assimpVertices = assimpMesh->mVertices[i];
			mesh->vertices.emplace_back(glm::vec3(assimpVertices.x, assimpVertices.y, -assimpVertices.z));

			//check if we have normals
			if (assimpMesh->mNormals) {
				//normal vertex coordinates
				const aiVector3D& assimpNormals = assimpMesh->mNormals[i];
				mesh->vertices.emplace_back(glm::vec3(assimpNormals.x, assimpNormals.y, assimpNormals.z));
			}

			for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++) {
				if (!assimpMesh->HasTextureCoords(j)) continue;

				const aiVector3D& assimpTextureCoords = assimpMesh->mTextureCoords[j][i];
				uvMappings[j]->emplace_back(glm::vec2(assimpTextureCoords.x, assimpTextureCoords.y));
			}

			for (int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++) {
				if (!assimpMesh->HasVertexColors(j)) continue;

				const aiColor4D& vertexColors = assimpMesh->mColors[j][i];
				mesh->colors.emplace_back(glm::vec4(vertexColors.r, vertexColors.g, vertexColors.b, vertexColors.a));
			}

			if (assimpMesh->HasTangentsAndBitangents()) {
				const aiVector3D& tangent = assimpMesh->mTangents[i];
				mesh->tangents.emplace_back(glm::vec3(tangent.x, tangent.y, tangent.z));
				//TODO: assimpMesh->mBitangents
			}

			// TODO: Add back in once OpenGL renderer supports skinned meshes
			//vertices.boneWeights[0] = 0.f;
			//vertices.boneWeights[1] = 0.f;
			//vertices.boneWeights[2] = 0.f;
			//vertices.boneWeights[3] = 0.f;
			//vertices.boneIds[0] = 0;
			//vertices.boneIds[1] = 0;
			//vertices.boneIds[2] = 0;
			//vertices.boneIds[3] = 0;
		}

		for (size_t i = 0; i < assimpMesh->mNumFaces; ++i) {
			const aiFace* face = &assimpMesh->mFaces[i];

			for (size_t j = 0; j < face->mNumIndices; j++)
				mesh->indices.push_back(face->mIndices[j]);
		}

		return mesh;
	}
}
