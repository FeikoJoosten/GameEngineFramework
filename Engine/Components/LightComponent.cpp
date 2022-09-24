#include "Engine/Components/LightComponent.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Renderer/Renderer.hpp"

#ifdef USING_OPENGL
#include "Engine/Renderer/OpenGLRenderer.hpp"
#endif
#ifdef USING_VULKAN
#include "Engine/Renderer/VulkanRenderer.hpp"
#endif

namespace Engine {

	void LightComponent::SetLightName(std::string name) {
		std::string oldName = lightName;
		lightName = name;
#ifdef USING_VULKAN
		renderer.lock()->SetLightType(oldName, LightType::LIGHT_NONEXISTENT);
		renderer.lock()->CreateLight(lightName, lightType,
			glm::vec3(lightInfo.position.x, lightInfo.position.y, lightInfo.position.z),
			glm::vec3(lightInfo.direction.x, lightInfo.direction.y, lightInfo.direction.z),
			glm::vec3(lightInfo.color.x, lightInfo.color.y, lightInfo.color.z),
			lightInfo.radius,
			lightInfo.attunuation,
			lightInfo.coneInnerAngle,
			lightInfo.coneOuterAngle);
#endif
	}

	std::string LightComponent::GetLightName() const {
		return lightName;
	}

	void LightComponent::SetLightType(LightType type) {
#ifdef USING_VULKAN
		if (vulkanEnabled && type != lightType && GetIsEnabled()) {
			renderer.lock()->SetLightType(lightName, type);
			switch (type) {
			case LightType::LIGHT_DIRECTIONAL_LIGHT:
				renderer.lock()->SetLightDirection(lightName,
					glm::vec3(lightInfo.direction.x, lightInfo.direction.y, lightInfo.direction.z));
				break;
			case LightType::LIGHT_POINT_LIGHT:
				renderer.lock()->SetLightPosition(lightName,
					glm::vec3(lightInfo.position.x, lightInfo.position.y, lightInfo.position.z));
				break;
			case LightType::LIGHT_SPOT_LIGHT:
				renderer.lock()->SetLightDirection(lightName,
					glm::vec3(lightInfo.direction.x, lightInfo.direction.y, lightInfo.direction.z));
				renderer.lock()->SetLightConeInnerAngle(lightName, lightInfo.coneInnerAngle);
				renderer.lock()->SetLightConeOuterAngle(lightName, lightInfo.coneOuterAngle);
				break;
			}
			lightType = type;
		}
#endif
		if (GetIsEnabled() == false)
			lightType = type;
	}

	LightType LightComponent::GetLightType() const {
		return lightType;
	}

	void LightComponent::SetLightPosition(glm::vec3 position) {
#ifdef USING_VULKAN
		renderer.lock()->SetLightPosition(lightName, position);
#endif
		lightInfo.position = glm::vec4(position.x, position.y, position.z, 1.f);
	}

	glm::vec3 LightComponent::GetLightPosition() {
		return glm::vec3(lightInfo.position.x, lightInfo.position.y, lightInfo.position.z);
	}

	void LightComponent::SetLightDirection(glm::vec3 direction) {
#ifdef USING_VULKAN
		renderer.lock()->SetLightDirection(lightName, direction);
#endif
		lightInfo.direction = glm::vec4(direction.x, direction.y, direction.z, 0.f);
	}

	glm::vec3 LightComponent::GetLightDirection() {
		return glm::vec3(lightInfo.direction.x, lightInfo.direction.y, lightInfo.direction.z);
	}

	void LightComponent::SetLightColor(glm::vec3 color) {
#ifdef USING_VULKAN
		renderer.lock()->SetLightColor(lightName, color);
#endif
		lightInfo.color = glm::vec4(color.x, color.y, color.z, 1.f);
	}

	glm::vec3 LightComponent::GetLightColor() {
		return glm::vec3(lightInfo.color.x, lightInfo.color.y, lightInfo.color.z);
	}

	void LightComponent::SetLightRadius(float radius) {
#ifdef USING_VULKAN
		renderer.lock()->SetLightRadius(lightName, radius);
#endif
		lightInfo.radius = radius;
	}

	float LightComponent::GetLightRadius() const {
		return lightInfo.radius;
	}

	void LightComponent::SetLightConeInnerAngle(float angle) {
#ifdef USING_VULKAN
		renderer.lock()->SetLightConeInnerAngle(lightName, angle);
#endif
		lightInfo.coneInnerAngle = angle;
	}

	float LightComponent::GetLightConeInnerAngle() {
		return lightInfo.coneInnerAngle;
	}

	void LightComponent::SetLightConeOuterAngle(float angle) {
#ifdef USING_VULKAN
		renderer.lock()->SetLightConeOuterAngle(lightName, angle);
#endif
		lightInfo.coneOuterAngle = angle;
	}

	float LightComponent::GetLightConeOuterAngle() {
		return lightInfo.coneOuterAngle;
	}

	LightComponent::LightComponent(std::string name, LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float radius, float attunuation, float coneInnerAngle, float coneOuterAngle) noexcept {
#ifdef USING_VULKAN
		vulkanEnabled = true;
		renderer = Renderer::Get<VulkanRenderer>();
		SetIsEnabled(true);
#else
		vulkanEnabled = false;
		SetIsEnabled(false);
#endif// USING_VULKAN

		lightName = name;

		if (name.empty())
			lightName = GetOwner()->GetName();

#ifdef USING_VULKAN
		renderer.lock()->CreateLight(lightName,
			type,
			glm::vec3(position.x, position.y, position.z),
			glm::vec3(direction.x, direction.y, direction.z),
			glm::vec3(color.x, color.y, color.z),
			radius, attunuation, coneInnerAngle, coneOuterAngle);
#endif
		lightType = type;
		lightInfo.position = glm::vec4(position.x, position.y, position.z, 1.f);
		lightInfo.direction = glm::vec4(direction.x, direction.y, direction.z, 0.f);
		lightInfo.color = glm::vec4(color.x, color.y, color.z, 1.f);
		lightInfo.radius = radius;
		lightInfo.attunuation = attunuation;
		lightInfo.coneInnerAngle = coneInnerAngle;
		lightInfo.coneOuterAngle = coneOuterAngle;
	}

	LightComponent::LightComponent(std::string name) noexcept {
		lightName = name;

		if (name.empty())
			lightName = GetOwner()->GetName();

#ifdef USING_VULKAN
		vulkanEnabled = true;
		renderer = Renderer::Get<VulkanRenderer>();
		SetIsEnabled(true);
#else
		vulkanEnabled = false;
		SetIsEnabled(false);
#endif

		if (lightName.empty())
			lightName = GetOwner()->GetName();
#ifdef USING_VULKAN
		renderer.lock()->CreateLight(lightName,
			LightType::LIGHT_NONEXISTENT,
			glm::vec3(),
			glm::vec3(),
			glm::vec3(),
			0.f, 0.f, 0.f, 0.f);
#endif
		lightType = LightType::LIGHT_NONEXISTENT;
		lightInfo.position = glm::vec4();
		lightInfo.direction = glm::vec4();
		lightInfo.color = glm::vec4();
		lightInfo.radius = 0.f;
		lightInfo.attunuation = 0.f;
		lightInfo.coneInnerAngle = 0.f;
		lightInfo.coneOuterAngle = 0.f;
	}

	void LightComponent::Update() {
#ifdef USING_VULKAN
		if (GetIsEnabled() != active) {
			active = GetIsEnabled();

			if (active == false)
				renderer.lock()->SetLightType(lightName, LightType::LIGHT_NONEXISTENT);
			else
				renderer.lock()->SetLightType(lightName, lightType);
		}
#endif
	}
}