#ifndef ENTITY_H
#define ENTITY_H

#include "../engine.hpp"
#include "../Components/Component.h"



class EntitySystem;
class RTS_COMMON_API Entity
{
public:
	Entity();
	~Entity();

	static const int MAX_NUM_COMPONENTS = 15;

	//store components here, init to nullptr
	Component* components[MAX_NUM_COMPONENTS] = {};

	//add a component - return true if success
	bool addComponent(Component* component);
	bool removeComponentByType(ComponentTypes::ComponentType componentType);

	//return nullptr if no component is found
	Component* getComponentByType(ComponentTypes::ComponentType componentType);

	void update();
private:

	//call this to initialize variables
	void initialize();
	static int id;
};

#endif
