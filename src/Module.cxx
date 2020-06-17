#include "Module.h"

using namespace cpp_di;


Module::Module()
{
}

Module::~Module()
{
}


void Module::Build(Container* cont)
{
	container = cont;
	Load();
}
