#include "Engine/Mesh/MeshAssetImporter.hpp"

namespace Engine {
	MeshAssetImporter::MeshAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Mesh>(assetRegistry) {}

	const char* MeshAssetImporter::GetDefaultAssetExtension() const {
		return ".mesh";
	}

	std::shared_ptr<Asset> MeshAssetImporter::CreateAsset() const {
		return std::shared_ptr<Mesh>(new Mesh());
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

	std::shared_ptr<Asset> MeshAssetImporter::LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const {
		std::shared_ptr<Asset> mesh;
		if (!AssetRegistry::TryLoadCustomDataFromAsset(assetGuid, GetDefaultAssetExtension(), mesh)) return {};

		return mesh;
	}

	void MeshAssetImporter::ProcessRawData(const std::shared_ptr<Mesh>& relatedMesh, const aiMesh* assimpMesh) {
		const std::vector uvMappings = {
			&relatedMesh->uvs,
			&relatedMesh->uv2,
			&relatedMesh->uv3,
			&relatedMesh->uv4,
			&relatedMesh->uv5,
			&relatedMesh->uv6,
			&relatedMesh->uv7,
			&relatedMesh->uv8
		};

		//unpack vertices
		for (unsigned i = 0; i < assimpMesh->mNumVertices; i++) {
			//position vertex coords
			const aiVector3D& assimpVertices = assimpMesh->mVertices[i];
			relatedMesh->vertices.emplace_back(glm::vec3(assimpVertices.x, assimpVertices.y, -assimpVertices.z));

			//check if we have normals
			if (assimpMesh->mNormals) {
				//normal vertex coordinates
				const aiVector3D& assimpNormals = assimpMesh->mNormals[i];
				relatedMesh->normals.emplace_back(glm::vec3(assimpNormals.x, assimpNormals.y, assimpNormals.z));
			}

			for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++) {
				if (!assimpMesh->HasTextureCoords(j)) continue;

				const aiVector3D& assimpTextureCoords = assimpMesh->mTextureCoords[j][i];
				uvMappings[j]->emplace_back(glm::vec2(assimpTextureCoords.x, assimpTextureCoords.y));
			}

			for (int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++) {
				if (!assimpMesh->HasVertexColors(j)) continue;

				const aiColor4D& vertexColors = assimpMesh->mColors[j][i];
				relatedMesh->colors.emplace_back(glm::vec4(vertexColors.r, vertexColors.g, vertexColors.b, vertexColors.a));
			}

			if (assimpMesh->HasTangentsAndBitangents()) {
				const aiVector3D& tangent = assimpMesh->mTangents[i];
				relatedMesh->tangents.emplace_back(glm::vec3(tangent.x, tangent.y, tangent.z));
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
				relatedMesh->indices.push_back(face->mIndices[j]);
		}
	}
}
