#include "Module.h"

using namespace compiler::util::di;


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
