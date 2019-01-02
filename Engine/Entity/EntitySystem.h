#ifndef ENTITY_SYSTEM_H
#define ENTITY_SYSTEM_H
#include "engine.hpp"

#include <vector>
#include "Entity.h"


/*<summary>
*	The entity system is a storage and manager for all of the entities in the game. *
*</summary>
*/


class RTS_COMMON_API EntitySystem
{
public:

	static Entity* createEntity();
	static void addEntity(Entity* ent);
	static std::vector<Entity*> findEntitiesByComponentType(enum ComponentTypes::ComponentType type);
	static std::vector<Entity*> entities;

	/*
	* TODO Add a clean up procedure for game over that cleans all entities.
	*/
};

#endif
