#include "Container.h"
#include "Registration.h"
#include "Module.h"

#include <stdexcept>
#include <string>


using namespace compiler::util::di;
using namespace compiler::util::di::registration;
using namespace std;

Container::Container()
: parentContainer(NULL), isRoot(true)
{
}

Container::Container(Container* pc)
: parentContainer(pc), isRoot(false)
{
}

Container::~Container()
{
}


registration::RegistrationBase* Container::StoreRegistrationData(const std::shared_ptr<RegistrationBase> reg)
{
	auto typeIndex = reg->typeIndex;
	auto exist = registeredTypes.find(typeIndex);
	if (exist != registeredTypes.end())
	{
		throw runtime_error(typeIndex.name() + string(" type already registered"));
	}
	else
	{
		registeredTypes.insert(make_pair(typeIndex, std::move(reg)));
		return registeredTypes.at(typeIndex).get();
	}
}


registration::RegistrationBase* Container::TryResolve(const std::type_index& id)
{
	auto exists = registeredTypes.find(id);
	if (exists != registeredTypes.end())
	{
		return exists->second.get();
	}
	else if (parentContainer != NULL)
	{
		// walk up the tree to find a scope that has registered the type
		return parentContainer->TryResolve(id);
	}
	else
	{
		throw std::runtime_error(id.name() + string(" type not registered. cannot resolve"));
	}
}

const Container* Container::GetRootContainer() const
{
	if (isRoot)
	{
		return this;
	}
	else
	{
		// walk up the tree to root
		return parentContainer->GetRootContainer();
	}
}

std::unordered_map<std::type_index, std::shared_ptr<registration::RegistrationBase>>* Container::GetGlobalInstances()
{
	if (isRoot)
	{
		return &(this->globalSingletons);
	}
	else
	{
		// walk up the tree to root
		return parentContainer->GetGlobalInstances();
	}
}


Container Container::Create()
{
	return Container();
}

Container Container::NewScope()
{
	return Container(this);
}

Container* Container::NewScopeAlloc()
{
	return new Container(this);
}

void Container::RegisterModule(Module& mod)
{
	mod.Build(this);
}
