#pragma once

#include "Engine/Api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Utility/Light.hpp"
#include "Engine/Utility/Defines.hpp"

#include <glm/detail/type_vec3.hpp>

namespace Engine {

	class Renderer;
#ifdef USING_VULKAN
	class VulkanRenderer;
#endif

	class ENGINE_API LightComponent : public Component
	{
	public:
		virtual ~LightComponent() override = default;

		/// <summary>
		/// Sets the name of the light. This is used internally 
		/// by the engine to reference the light. If two lights
		/// have the same name, any changes to one of them will alter both.
		/// The default name used will be the entity name.
		/// WARNING: Any lights that already have the new name will be reset to the values held by this component.
		/// </summary>
		/// <param name="name">The new name of the light. 
		/// NOTE: Unless you want to have multiple references to the same light,
		/// prefix this with the entity name for safety.</param>
		void SetLightName(std::string name);

		/// <summary>
		/// Returns the name of the light. The default name is the entity name.
		/// </summary>
		/// <returns>The name of the light.</returns>
		std::string GetLightName() const;

		/// <summary>
		/// Sets the type of the light. NOTE: the type non-existent (-1) disables the light.
		/// </summary>
		/// <param name="type">The light's new type.</param>
		void SetLightType(LightType type);

		/// <summary>
		/// Returns the type of the light.
		/// </summary>
		/// <returns>The light's type.</returns>
		LightType GetLightType() const;

		/// <summary>
		/// Sets the position of the light. 
		/// NOTE: In case of directional lights and ambient lights this value will take effect if the light type is changed to a point or spot light.
		/// </summary>
		/// <param name="position">A vec3 containing the position.</param>
		void SetLightPosition(glm::vec3 position);

		/// <summary>
		/// Returns the position of the light. 
		/// </summary>
		/// <returns>A vec3 containing the position.</returns>
		glm::vec3 GetLightPosition();

		/// <summary>
		/// Sets the direction of the light.
		/// NOTE: In case of point and ambient lights this value will take effect if the light type is changed to a directional or spot light.
		/// </summary>
		/// <param name="direction">A vec3 containing the new direction.</param>
		void SetLightDirection(glm::vec3 direction);

		/// <summary>
		/// Returns the direction of the light.
		/// </summary>
		/// <returns>A vec3 containing the direction.</returns>
		glm::vec3 GetLightDirection();

		/// <summary>
		/// Sets the color of the light.
		/// </summary>
		/// <param name="color">A vec3 containing the color in rgb format.</param>
		void SetLightColor(glm::vec3 color);

		/// <summary>
		/// Returns the color of the light.
		/// </summary>
		/// <returns>A vec3 containing the color in rgb format.</returns>
		glm::vec3 GetLightColor();

		/// <summary>
		/// Sets the new radius of the light.
		/// NOTE: In case of directional lights and ambient lights this value will take effect if the light type is changed to a point or spot light.
		/// </summary>
		/// <param name="radius">A float containing the new radius of the light.</param>
		void SetLightRadius(float radius);

		/// <summary>
		/// Returns the radius of the light.
		/// </summary>
		/// <returns>A float containing the radius of the light.</returns>
		float GetLightRadius() const;

		/// <summary>
		/// Sets the inner angle of the light cone.
		/// NOTE: If the light isn't a spot light this value will take effect if the light type is changed to spot light.
		/// </summary>
		/// <param name="angle">The new angle in radians.</param>
		void SetLightConeInnerAngle(float angle);

		/// <summary>
		/// Returns the inner angle of the light cone.
		/// </summary>
		/// <returns>The angle in radians.</returns>
		float GetLightConeInnerAngle();

		/// <summary>
		/// Sets the outer angle of the light cone.
		/// NOTE: If the light isn't a spot light this value will take effect if the light type is changed to spot light.
		/// </summary>
		/// <param name="angle">The new angle in radians.</param>
		void SetLightConeOuterAngle(float angle);

		/// <summary>
		/// Returns the outer angle of the light cone.
		/// </summary>
		/// <returns>The angle in radians.</returns>
		float GetLightConeOuterAngle();

		/// <summary>
		/// Creates a new light component with the given name and type, and initializes it with the given variables.
		/// </summary>
		/// <param name="name">The name of the light. Use this when modifying the light in other functions.</param>
		/// <param name="type">The type of light that's being created.</param>
		/// <param name="position">The position of the light. Unused for directional lights.</param>
		/// <param name="direction">The direction the light points at. Unused for point lights.</param>
		/// <param name="color">The color of the light.</param>
		/// <param name="radius">The radius over which a light has effect.</param>
		/// <param name="attunuation">Attunuation of the light. Currently not used.</param>
		/// <param name="coneInnerAngle">The angle of a spot light in which a pixel recieves the incoming light at full strength.
		/// Only used for spot lights.</param>
		/// <param name="coneOuterAngle">The angle of spot light that is the outer most angle a pixel can still recieve incoming light.
		/// If a pixel is between the inner and outer angle the strength of the light will slowly fade depending how close to the
		/// outer angle the pixel is. Only used for spot lights.</param>
		explicit LightComponent(std::string name, LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color,
			float radius, float attunuation, float coneInnerAngle, float coneOuterAngle) noexcept;

		/// <summary>
		/// Creates a new light component with the given name, but doesn't set the variables for the light.
		/// </summary>
		/// <param name="name">The name to use for the light.</param>
		explicit LightComponent(std::string name) noexcept;

	private:
		virtual void Update() override;

		std::string lightName;

		bool vulkanEnabled;
		bool active;

#ifdef USING_OPENGL
		std::weak_ptr<Renderer> renderer;
#endif
#ifdef USING_VULKAN
		std::weak_ptr<VulkanRenderer> renderer;
#endif

		Light lightInfo;
		LightType lightType;
	};

}
