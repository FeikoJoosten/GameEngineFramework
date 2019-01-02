#pragma once
#include "../engine.hpp"


/*
*	Base component interface, keeps component type enum
*	Use the component type constructor when initializing derived components
*/
namespace ComponentTypes
{
	enum ComponentType
	{
		TRANSFORM_COMPONENT,
		DAMAGE_COMPONENT,
		MESH_COMPONENT
	};
}


class RTS_COMMON_API Component
{
public:
	Component(); //we don't want to have components without a type
	Component(ComponentTypes::ComponentType componentType);
	virtual ~Component();

	ComponentTypes::ComponentType componentType;
private:
	
};

