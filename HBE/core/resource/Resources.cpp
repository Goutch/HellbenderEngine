
#include "Resources.h"
#include "ResourceFactory.h"
namespace HBE{
	const ResourceFactory* Resources::factory= nullptr;
	std::unordered_map<std::string, Resource *> Resources::registry=std::unordered_map<std::string, Resource *>();
}
