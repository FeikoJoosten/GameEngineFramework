#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Material/Material.hpp"
#include "Engine/Texture/Texture.hpp"

#include <glm/glm.hpp>

namespace Engine {

	class ENGINE_API Particle
	{
	public:
		Particle();
		~Particle();

		enum class ParticleType {
			LINE = 0,
			BILLBOARD,
			MESH
		};

		enum class InterpolateType {
			START = 0,
			END,
			LINEAR,
			QUADRATIC
		};

		enum class BlendMode {
			ALPHA = 0,
			ADDATIVE,
			SOLID
		};

		enum class LineEndAction {
			STOP = 0,
			REMOVE_FIRST,
			REMOVE_SECOND,
			REMOVE_LAST,
			DESTROY
		};

		/// <summary>
		/// Sets the type of the particle.
		/// </summary>
		/// <param name="type">The new type.</param>
		void SetParticleType(ParticleType type);

		/// <summary>
		/// Returns the type of the particle.
		/// </summary>
		/// <returns>The particle type.</returns>
		ParticleType GetParticleType() const;

		/// <summary>
		/// Sets the blend mode of the particle.
		/// </summary>
		/// <param name="mode">The new blend mode.</param>
		void SetBlendMode(BlendMode mode);

		/// <summary>
		/// Returns the blend mode of the particle.
		/// </summary>
		/// <returns>The particle blend mode.</returns>
		BlendMode GetParticleBlendMode() const;

		/// <summary>
		/// Sets the range of possible start colors of the particle.
		/// </summary>
		/// <param name="colorMin">The lowest possible color in the range.</param>
		/// <param name="colorMax">The highest possible color in the range.</param>
		void SetStartColor(glm::vec4 colorMin, glm::vec4 colorMax);
		
		/// <summary>
		/// Returns a pair containing the range of possible start colors of the particle.
		/// </summary>
		/// <returns>An eastl pair containing the range of colors.</returns>
		std::pair<glm::vec4, glm::vec4> GetStartColor();

		/// <summary>
		/// Sets the range of possible end colors of the particle.
		/// </summary>
		/// <param name="colorMin">The lowest possible color in the range.</param>
		/// <param name="colorMax">The highest possible color in the range.</param>
		void SetEndColor(glm::vec4 colorMin, glm::vec4 colorMax);

		/// <summary>
		/// Returns a pair containing the range of possible end colors of the particle.
		/// </summary>
		/// <returns>An eastl pair containing the range of colors.</returns>
		std::pair<glm::vec4, glm::vec4> GetEndColor();

		/// <summary>
		/// Sets the interpolation type used for the color.
		/// </summary>
		/// <param name="type">The interpolation type used for the color.</param>
		void SetColorInterpolateType(InterpolateType type);

		/// <summary>
		/// Returns the interpolation type used for the color.
		/// </summary>
		/// <returns>The interpolation type used for the color.</returns>
		InterpolateType GetColorInterpolateType() const;

		/// <summary>
		/// Sets the min and max for the range of forces that can be applied on the particle.
		/// </summary>
		/// <param name="forceMin">The lowest possible force the particle can be created with.</param>
		/// <param name="forceMax">The hightest possible force the particle can be created with.</param>
		void SetForce(glm::vec3 forceMin, glm::vec3 forceMax);

		/// <summary>
		/// Returns a pair containing the range of possible forces that can be applied on the particle.
		/// </summary>
		/// <returns>A pair containing a range of forces.</returns>
		std::pair<glm::vec3, glm::vec3> GetForce();

		/// <summary>
		/// Sets the range of possible lifetimes the particle can have.
		/// </summary>
		/// <param name="lifetimeMin">The lowest possible lifetime.</param>
		/// <param name="lifetimeMax">The highest possible lifetime.</param>
		void SetLifetime(float lifetimeMin, float lifetimeMax);

		/// <summary>
		/// Returns the range of possible lifetimes the particle can have.
		/// </summary>
		/// <returns>A pair containing a range of lifetimes.</returns>
		std::pair<float, float> GetLifetime();

		/// <summary>
		/// Sets the range of possible speeds the particle can be created with.
		/// </summary>
		/// <param name="speedMin">The lowest possible speed.</param>
		/// <param name="speedMax">The highest possible speed.</param>
		void SetSpeed(float speedMin, float speedMax);

		/// <summary>
		/// Returns the range of possible speeds the particle can be created with.
		/// </summary>
		/// <returns>A pair containing a range of speeds.</returns>
		std::pair<float, float> GetSpeed();

		/// <summary>
		/// Sets the range of masses a particle can be created with. 
		/// NOTE: If the mass is zero forces won't interact with the particle, 
		/// to prevent particles from gaining infinite speed as soon as they encounter a force.
		/// </summary>
		/// <param name="massMin">The lowest possible mass.</param>
		/// <param name="massMax">The highest possible mass.</param>
		void SetMass(float massMin, float massMax);

		/// <summary>
		/// Returns the range of possible masses the particle can be created with.
		/// </summary>
		/// <returns>A pair containing a range of masses.</returns>
		std::pair<float, float> GetMass();

		/// <summary>
		/// Turns gravity on and off for the particle. Gravitic constant will be 9.81. If you want different gravities, turn this off and instead specify a force for the particle.
		/// </summary>
		/// <param name="gravity">Whether or not gravity should be used.</param>
		void SetGravityEnabled(bool gravity);

		/// <summary>
		/// Returns whether or not gravity is enabled for the particle.
		/// </summary>
		/// <returns></returns>
		bool IsGravityEnabled() const;

		/// <summary>
		/// Sets the texture of the particle. Only applies to lines and billboards. Can be left empty to use just the particle color.
		/// </summary>
		/// <param name="texture">A weak pointer to the texture.</param>
		void SetTexture(std::weak_ptr<Texture> texture);

		/// <summary>
		/// Returns the texture used by the particle.
		/// </summary>
		/// <returns>A weak pointer to a texture.</returns>
		std::weak_ptr<Texture> GetTexture() const;

		/// <summary>
		/// Returns whether or not the particle has a texture.
		/// </summary>
		/// <returns>A bool indicating whether or not the particle has a texture.</returns>
		bool HasTexture() const;

		/// <summary>
		/// Sets the maximum amount of segments a line particle can have.
		/// </summary>
		/// <param name="segments">The maximum number of segments.</param>
		void SetLineMaxSegments(int segments);

		/// <summary>
		/// Returns the maximum number of segments a line particle can have.
		/// </summary>
		/// <returns>The maximum number of segments.</returns>
		int GetLineMaxSegments() const;

		/// <summary>
		/// Sets the amount of new segments that get created each frame.
		/// </summary>
		/// <param name="segments">The amount of new segments.</param>
		void SetNewSegmentsPerFrame(int segments);

		/// <summary>
		/// Returns the ammount of new segments that get created each frame.
		/// </summary>
		/// <returns>The amount of new segments.</returns>
		int GetNewSegmentsPerFrame() const;

		/// <summary>
		/// Sets the action that should be taken when trying to create segments after having reached the maximum amount of segments.
		/// </summary>
		/// <param name="action">The action that should be taken.</param>
		void SetLineEndAction(LineEndAction action);

		/// <summary>
		/// Returns the action that should be taken when trying to create segments after having reached the maximum amount of segments.
		/// </summary>
		/// <returns>The action that should be taken.</returns>
		LineEndAction GetLineEndAction() const;

		/// <summary>
		/// Sets the range of thicknesses a line can start with (interpolation is based on lifetime, not segment count).
		/// </summary>
		/// <param name="thicknessMin">The minimum thickness.</param>
		/// <param name="thicknessMax">The maximum thickness.</param>
		void SetLineStartThickness(float thicknessMin, float thicknessMax);

		/// <summary>
		/// Returns the range of thicknesses a line can start with.
		/// </summary>
		/// <returns>A pair of floats containing the range.</returns>
		std::pair<float, float> GetLineStartThickness();

		/// <summary>
		/// Sets the range of thicknesses a line can end with (interpolation is based on lifetime, not segment count).
		/// </summary>
		/// <param name="thicknessMin">The minimum thickness.</param>
		/// <param name="thicknessMax">The maximum thickness.</param>
		void SetLineEndthickness(float thicknessMin, float thicknessMax);
		
		/// <summary>
		/// Returns the range of thicknesses a line can end with.
		/// </summary>
		/// <returns>A pair of floats containing the range.</returns>
		std::pair<float, float> GetLineEndThickness();

		/// <summary>
		/// Sets the interpolation type of the line thickness.
		/// </summary>
		/// <param name="type">The interpolation type.</param>
		void SetLineThicknessInterpolateType(InterpolateType type);

		/// <summary>
		/// Returns the interpolation type of the line thickness.
		/// </summary>
		/// <returns>The interpolation type.</returns>
		InterpolateType GetLineThicknessInterpolateType() const;

		/// <summary>
		/// Sets the range for start size of the billboard.
		/// </summary>
		/// <param name="sizeMin">The minimum size.</param>
		/// <param name="sizeMax">The maximum size.</param>
		void SetBillboardSizeStart(glm::vec2 sizeMin, glm::vec2 sizeMax);

		/// <summary>
		/// Returns the range of start sizes for the billboard.
		/// </summary>
		/// <returns>A pair of vec2s containing the size.</returns>
		std::pair<glm::vec2, glm::vec2> GetBillboardSizeStart();

		/// <summary>
		/// Sets the range for the end size of the billboard.
		/// </summary>
		/// <param name="sizeMin">The minimum size.</param>
		/// <param name="sizeMax">The maximum size.</param>
		void SetBillboardSizeEnd(glm::vec2 sizeMin, glm::vec2 sizeMax);

		/// <summary>
		/// Returns the range of end sizes for the billboard.
		/// </summary>
		/// <returns>A pair of vec2s containing the range of sizes.</returns>
		std::pair<glm::vec2, glm::vec2> GetBillboardSizeEnd();

		/// <summary>
		/// Sets the interpolation type for the billboard size.
		/// </summary>
		/// <param name="type">The interpolation type.</param>
		void SetBillboardSizeInterpolateType(InterpolateType type);

		/// <summary>
		/// Returns the interpolation type for the billboard size.
		/// </summary>
		/// <returns>The interpolation type.</returns>
		InterpolateType GetBillboardSizeInterpolateType() const;

		/// <summary>
		/// Sets the range of roll rotation speeds the billboard can start with.
		/// </summary>
		/// <param name="speedMin">The minimum speed.</param>
		/// <param name="speedMax">The maximum speed.</param>
		void SetBillboardRollSpeedStart(float speedMin, float speedMax);

		/// <summary>
		/// Returns the angle of roll rotation speeds the billboard can start with.
		/// </summary>
		/// <returns>A pair of floats containing the range of speeds.</returns>
		std::pair<float, float> GetBillboardRollSpeedStart();

		/// <summary>
		/// Sets the range of roll rotation speeds the billboard can end with.
		/// </summary>
		/// <param name="speedMin">The minimum speed.</param>
		/// <param name="speedMax">The maximum speed.</param>
		void SetBillboardRollSpeedEnd(float speedMin, float speedMax);

		/// <summary>
		/// Returns the angle of roll rotation speeds the billboard can end with.
		/// </summary>
		/// <returns>A pair of floats containing the range of speeds.</returns>
		std::pair<float, float> GetBillboardRollSpeedEnd();

		/// <summary>
		/// Sets the range of roll rotations the billboard can start with.
		/// </summary>
		/// <param name="rollMin">The minimum rotation.</param>
		/// <param name="rollMax">The maximum rotation.</param>
		void SetBillboardRoll(float rollMin, float rollMax);

		/// <summary>
		/// Returns the range of roll rotations the billboard can start with.
		/// </summary>
		/// <returns>A pair of floats containing the range.</returns>
		std::pair<float, float> GetBillboardRoll();

		/// <summary>
		/// Sets the interpolation type for the roll speed.
		/// </summary>
		/// <param name="type">The interpolation type.</param>
		void SetBillboardRollSpeedInterpolateType(InterpolateType type);

		/// <summary>
		/// Returns the interpolation type for the roll speed.
		/// </summary>
		/// <returns>The interpolation type.</returns>
		InterpolateType GetBillboardRollSpeedInterpolateType() const;

		/// <summary>
		/// Sets the mesh used for mesh particles.
		/// </summary>
		/// <param name="mesh">The new mesh.</param>
		void SetMesh(std::weak_ptr<Mesh> mesh);

		/// <summary>
		/// Returns the mesh used for mesh particles.
		/// </summary>
		/// <returns>The mesh.</returns>
		std::weak_ptr<Mesh> GetMesh() const;

		/// <summary>
		/// Sets the material used for the mesh. Material color is overriden by the particle color.
		/// </summary>
		/// <param name="material">The mesh material.</param>
		void SetMaterial(std::weak_ptr<Material> material);

		/// <summary>
		/// Returns the material used for the mesh.
		/// </summary>
		/// <returns>The mesh material.</returns>
		std::weak_ptr<Material> GetMaterial() const;

		/// <summary>
		/// Sets the range for start scale of the mesh.
		/// </summary>
		/// <param name="scaleMin">The minimum scale.</param>
		/// <param name="scaleMax">The maximum scale.</param>
		void SetMeshScaleStart(glm::vec3 scaleMin, glm::vec3 scaleMax);

		/// <summary>
		/// Returns the range of start scales for the mesh.
		/// </summary>
		/// <returns>A pair of vec3s containing the scale.</returns>
		std::pair<glm::vec3, glm::vec3> GetMeshScaleStart();

		/// <summary>
		/// Sets the range for the end scale of the mesh.
		/// </summary>
		/// <param name="scaleMin">The minimum scale.</param>
		/// <param name="scaleMax">The maximum scale.</param>
		void SetMeshScaleEnd(glm::vec3 scaleMin, glm::vec3 scaleMax);

		/// <summary>
		/// Returns the range of end scales for the mesh.
		/// </summary>
		/// <returns>A pair of vec3s containing the range of scales.</returns>
		std::pair<glm::vec3, glm::vec3> GetMeshScaleEnd();

		/// <summary>
		/// Sets the interpolation type for the mesh scale.
		/// </summary>
		/// <param name="type">The interpolation type.</param>
		void SetMeshScaleInterpolateType(InterpolateType type);

		/// <summary>
		/// Returns the interpolation type for the mesh scale.
		/// </summary>
		/// <returns>The interpolation type.</returns>
		InterpolateType GetMeshScaleInterpolateType() const;

		/// <summary>
		/// Sets the range of rotation speeds in euler angles the mesh can start with.
		/// </summary>
		/// <param name="speedMin">The minimum speed.</param>
		/// <param name="speedMax">The maximum speed.</param>
		void SetMeshRotationSpeedStart(glm::vec3 speedMin, glm::vec3 speedMax);

		/// <summary>
		/// Returns the rotation speeds in euler angles the mesh can start with.
		/// </summary>
		/// <returns>A pair of glm::vec3s containing the range of speeds.</returns>
		std::pair<glm::vec3, glm::vec3> GetMeshRotationSpeedStart();

		/// <summary>
		/// Sets the range of rotation speeds in euler angles the mesh can end with.
		/// </summary>
		/// <param name="speedMin">The minimum speed.</param>
		/// <param name="speedMax">The maximum speed.</param>
		void SetMeshRotationSpeedEnd(glm::vec3 speedMin, glm::vec3 speedMax);

		/// <summary>
		/// Returns the rotation speeds in euler angles the mesh can end with.
		/// </summary>
		/// <returns>A pair of vec3s containing the range of speeds.</returns>
		std::pair<glm::vec3, glm::vec3> GetMeshRotationSpeedEnd();

		/// <summary>
		/// Sets the range of rotations in euler angles the mesh can start with.
		/// </summary>
		/// <param name="rotationMin">The minimum rotation.</param>
		/// <param name="rotationMax">The maximum rotation.</param>
		void SetMeshRotation(glm::vec3 rotationMin, glm::vec3 rotationMax);

		/// <summary>
		/// Returns the range of rotations in euler angles the mesh can start with.
		/// </summary>
		/// <returns>A pair of vec3s containing the range.</returns>
		std::pair<glm::vec3, glm::vec3> GetMeshRotation();

		/// <summary>
		/// Sets the interpolation type for the rotation speed.
		/// </summary>
		/// <param name="type">The interpolation type.</param>
		void SetMeshRotationSpeedInterpolateType(InterpolateType type);

		/// <summary>
		/// Returns the interpolation type for the rotation speed.
		/// </summary>
		/// <returns>The interpolation type.</returns>
		InterpolateType GetMeshRotationSpeedInterpolateType() const;

		/// <summary>
		/// Sets whether or not the lightning calculations will be done when rendering the mesh. NOTE: Only works for mesh particles.
		/// </summary>
		/// <param name="lit">Bool indicating whether or not the lightning calculations will be done.</param>
		void SetMeshLit(bool lit);

		/// <summary>
		/// Returns whether or not the lightning calculations will be done when rendering the mesh.
		/// </summary>
		/// <returns>Bool indicating whether or not the lightning calculations will be done.</returns>
		bool IsMeshLit() const;

		/// <summary>
		/// Sets whether or not the particle can cast shadows. NOTE: This only works for mesh particles.
		/// WARNING: This will cause a lot of additional calculations, especially when using higher complexity meshes. Only use this when absolutely neseccary.
		/// </summary>
		/// <param name="castShadows">Bool indicating whether or not the mesh should cast shadows.</param>
		void SetCastsShadows(bool castShadows);

		/// <summary>
		/// Returns whether or not the particle can cast shadows.
		/// </summary>
		/// <returns>Whether or not the particle can cast shadows.</returns>
		bool CanCastShadows() const;

	protected:

		friend class Emitter;

		struct ParticleData {
			glm::vec4 position;
			glm::vec4 direction;
			glm::vec4 force;
			glm::vec4 startColor;
			glm::vec4 endColor;
			glm::vec4 currentColor;
			float lifetime;
			float currentLifetime;
			float speed;
			float mass;
		};

		struct LineData {
			int segmentCount;
			float startThickness;
			float endThickness;
			float currentThickness;
		};

		struct BillboardData {
			glm::vec2 startSize;
			glm::vec2 endSize;
			glm::vec2 currentSize;
			glm::vec2 padding;
			float startRotationSpeed;
			float endRotationSpeed;
			float currentRotationSpeed;
			float currentRotation;
		};

		struct MeshData {
			glm::vec4 startScale;
			glm::vec4 endScale;
			glm::vec4 currentScale;
			glm::vec4 startRotationSpeed;
			glm::vec4 endRotationSpeed;
			glm::vec4 currentRotationSpeed;
			glm::vec4 currentRotation;
		};

		ParticleData CreateParticle(glm::vec4 position, glm::vec4 direction) const;

		LineData CreateParticleLineData() const;

		BillboardData CreateParticleBillboardData() const;

		MeshData CreateParticleMeshData() const;

		float RandomFloatLinear(float min, float max) const;

		ParticleType particleType;
		BlendMode particleBlendMode;

		glm::vec4 startColorMin, startColorMax;
		glm::vec4 endColorMin, endColorMax;
		InterpolateType colorInterpolateType;

		glm::vec4 forceMin, forceMax;

		float lifetimeMin, lifetimeMax;
		float speedMin, speedMax;
		float massMin, massMax;

		bool gravity = false;

		std::weak_ptr<Texture> texture;

		bool textured = false;

		int maxSegments;
		int newSegmentsPerFrame;

		LineEndAction lineEndAction;

		float startLineThicknessMin, startLineThicknessMax;
		float endLineThicknessMin, endLineThicknessMax;
		InterpolateType lineThicknessInterpolateType;

		glm::vec2 startSizeMin, startSizeMax;
		glm::vec2 endSizeMin, endSizeMax;
		InterpolateType sizeInterpolateType;

		float startRollSpeedMin, startRollSpeedMax;
		float endRollSpeedMin, endRollSpeedMax;
		InterpolateType rollSpeedInterpolateType;

		float rollMin, rollMax;

		std::weak_ptr<Mesh> mesh;
		std::weak_ptr<Material> material;

		glm::vec4 startScaleMin, startScaleMax;
		glm::vec4 endScaleMin, endScaleMax;
		InterpolateType scaleInterpolateType;

		glm::vec4 startRotationSpeedMin, startRotationSpeedMax;
		glm::vec4 endRotationSpeedMin, endRotationSpeedMax;
		InterpolateType rotationSpeedInterpolateType;

		glm::vec4 rotationMin, rotationMax;

		bool meshLit;
		bool castShadows;
	};

}