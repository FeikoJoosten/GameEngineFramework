#pragma once

#include "Engine/Utility/Defines.hpp"
#include "Engine/Texture/Texture.hpp"

#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

#ifdef USING_OPENGL
	#include "Engine/Texture/OpenGLTexture.hpp"
#endif
#ifdef USING_VULKAN
	#include "Engine/Texture/VulkanTexture.hpp"
#endif

namespace Engine {

	class Material
	{
	public:
		Material() = default;

		/// <summary>
		/// Creates a new material from the material data stored in the scene at the given index. 
		/// Doesn't load any data if the index is larger than the amount of materials contained in.
		/// Use this function in the mesh loading code to create the meshes with the default materials for that mesh.
		/// </summary>
		/// <param name="scene">The scene (model) to load the material from.</param>
		/// <param name="materialIndex">The specific material from the material array to load.</param>
		/// <param name="modelName">The name of the model. This is used for naming any embedded textures found in the file.</param>
		Material(const aiScene* scene, uint32_t materialIndex, eastl::string modelName);
		virtual ~Material();

		/// <summary>
		/// Changes the diffuse texture of the material. NOTE: Texture will be loaded in, if it doesn't excist yet.
		/// </summary>
		/// <param name="diffuseTextureName">The texture you want to create. NOTE: The texture needs to be located under 'Resources/Textures/'. You then give the texture name + extension.
		/// The texture can be in a subfolder of of 'Resources/Textures/'.</param>
		virtual void SetDiffuseTexture(eastl::string diffuseTextureName);

		/// <summary>
		/// Changes the diffuse texture of the material. Pass a nullptr to clear the current texture.
		/// </summary>
		/// <param name="diffuseTexture">The new diffuse texture.</param>
		virtual void SetDiffuseTexture(eastl::shared_ptr<Texture> diffuseTexture);

		/// <summary>
		/// Returns the diffuse texture of the material.
		/// Returns nullptr if the current material doesn't have a diffuse texture.
		/// </summary>
		/// <returns>The diffuse texture used by the material. NOTE: Will return a temporary texture in case the default diffuse texture hasn't been assigned.</returns>
		eastl::shared_ptr<Texture> GetDiffuseTexture();

		/// <summary>
		/// Returns the diffuse texture that was defined by the model this material was created from. 
		/// Returns nullptr if no diffuse texture was specified for the material.
		/// </summary>
		/// <returns>The default diffuse texture for the material. NOTE: Will return a temporary texture in case the default diffuse texture hasn't been assigned.</returns>
		eastl::shared_ptr<Texture> GetDefaultDiffuseTexture();

		/// <summary>
		/// Indicates if the material has a diffuse texture loaded or not.
		/// </summary>
		/// <returns>Whether or not the material has a diffuse texture.</returns>
		bool IsDiffuseLoaded() const;

		/// <summary>
		/// Changes the bump map texture of the material. NOTE: Texture will be loaded in, if it doesn't excist yet.
		/// </summary>
		/// <param name="bumpMapTextureName">The texture you want to create. NOTE: The texture needs to be located under 'Resources/Textures/'. You then give the texture name + extension.
		/// The texture can be in a subfolder of of 'Resources/Textures/'.</param>
		virtual void SetBumpMapTexture(eastl::string bumpMapTextureName);

		/// <summary>
		/// Changes the bump map texture of the material. Pass a nullptr to clear the current texture.
		/// </summary>
		/// <param name="bumpMapTexture">The new bump map texture.</param>
		virtual void SetBumpMapTexture(eastl::shared_ptr<Texture> bumpMapTexture);

		/// <summary>
		/// Returns the bump map texture of the material.
		/// Returns nullptr if the current material doesn't have a bump map texture.
		/// </summary>
		/// <returns>The bump map texture used by the material. NOTE: Will return a temporary texture in case the default bump map texture hasn't been assigned.</returns>
		eastl::shared_ptr<Texture> GetBumpMapTexture() const;

		/// <summary>
		/// Returns the bump map texture that was defined by the model this material was created from. 
		/// Returns nullptr if no bump map texture was specified for the material.
		/// </summary>
		/// <returns>The default bump map texture for the material. NOTE: Will return a temporary texture in case the default bump map texture hasn't been assigned.</returns>
		eastl::shared_ptr<Texture> GetDefaultBumpMapTexture() const;

		/// <summary>
		/// Indicates if the material has a bump map texture loaded or not.
		/// </summary>
		/// <returns>Whether or not the material has a bump map texture.</returns>
		bool IsBumpMapLoaded() const;

		/// <summary>
		/// Changes the specular texture of the material. Pass a nullptr to clear the current texture.
		/// </summary>
		/// <param name="specularTexture">The new specular texture.</param>
		virtual void SetSpecularTexture(eastl::shared_ptr<Texture> specularTexture);

		/// <summary>
		/// Returns the specular texture of the material.
		/// Returns nullptr if the current material doesn't have a specular texture.
		/// </summary>
		/// <returns>The specular texture used by the material.</returns>
		eastl::shared_ptr<Texture> GetSpecularTexture() const;

		/// <summary>
		/// Returns the specular texture that was defined by the model this material was created from. 
		/// Returns nullptr if no specular texture was specified for the material.
		/// </summary>
		/// <returns>The default specular texture for the material.</returns>
		eastl::shared_ptr<Texture> GetDefaultSpecularTexture() const;

		/// <summary>
		/// Indicates if the material has a specular texture loaded or not.
		/// </summary>
		/// <returns>Whether or not the material has a specular texture.</returns>
		bool IsSpecularLoaded() const;

		/// <summary>
		/// Sets the material's diffuse color.
		/// </summary>
		/// <param name="diffuseColor">The new color.</param>
		virtual void SetDiffuseColor(glm::vec3 diffuseColor);

		/// <summary>
		/// Returns the material's diffuse color.
		/// </summary>
		/// <returns>The diffuse color.</returns>
		glm::vec3 GetDiffuseColor() const;

		/// <summary>
		/// Returns the diffuse color that was defined by the model this material was created from.
		/// </summary>
		/// <returns>The default diffuse color.</returns>
		glm::vec3 GetDefaultDiffuseColor() const;


		/// <summary>
		/// Sets the material's specular color.
		/// </summary>
		/// <param name="specularColor">The new color.</param>
		virtual void SetSpecularColor(glm::vec3 specularColor);

		/// <summary>
		/// Returns the material's specular color.
		/// </summary>
		/// <returns>The specular color.</returns>
		glm::vec3 GetSpecularColor() const;

		/// <summary>
		/// Returns the specular color that was defined by the model this material was created from.
		/// </summary>
		/// <returns>The default specular color.</returns>
		glm::vec3 GetDefaultSpecularColor() const;

		/// <summary>
		/// Sets the scale of the specular highlights (strength) of the material.
		/// </summary>
		/// <param name="scale">The new scale.</param>
		void SetSpecularScale(float scale);

		/// <summary>
		/// Returns the scale of the specular highlights (strength).
		/// </summary>
		/// <returns>The scale as a float.</returns>
		float GetSpecularScale() const;

		/// <summary>
		/// Returns the scale of the specular highlights (strength) 
		/// that was defined by the model this material was created from.
		/// </summary>
		/// <returns>The default scale as a float.</returns>
		float GetDefaultSpecularScale() const;

		/// <summary>
		/// Sets the specular exponent of the material (shinyness).
		/// </summary>
		/// <param name="exponent">The new exponent.</param>
		void SetSpecularExponent(float exponent);

		/// <summary>
		/// Returns the specular exponent of the material.
		/// </summary>
		/// <returns>The exponent as a float.</returns>
		float GetSpecularExponent() const;

		/// <summary>
		/// Returns the specular exponent of the material 
		/// that was defined by the model this material was created from.
		/// </summary>
		/// <returns>The default exponent as a float.</returns>
		float GetDefaultSpecularExponent() const;

		/// <summary>
		/// Returns whether or not two materials are the same.
		/// </summary>
		/// <param name="other">The material to compare to.</param>
		/// <returns>Whether or not the materials are equal.</returns>
		bool operator==(const Material& other) const;

	protected:
		typedef struct MaterialData
		{
			int32_t diffuseTextureLoaded;
			glm::vec4 diffuseColor;
			int32_t specularTextureLoaded;
			glm::vec4 specularColor;
			int32_t bumpMapLoaded;
			float specularScale;
			float specularExponent;
		} MaterialData_t;

		virtual void UpdateMaterialData();

		MaterialData_t materialData_;
		MaterialData_t defaultMaterialData_;

		eastl::string modelName;

		eastl::shared_ptr<Texture> diffuseTexture;
		eastl::shared_ptr<Texture> defaultDiffuseTexture;
		eastl::shared_ptr<Texture> bumpMapTexture;
		eastl::shared_ptr<Texture> defaultBumpMapTexture;
		eastl::shared_ptr<Texture> specularTexture;
		eastl::shared_ptr<Texture> defaultSpecularTexture;

		eastl::shared_ptr<Texture> missingTexture;

		eastl::shared_ptr<Texture> LoadTexture(const aiScene* scene, aiMaterial* material, aiTextureType textureType);
	};

} // namespace Engine