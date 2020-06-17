#pragma once

#include "Container.h"

namespace compiler::util::di
{
	class Module
	{
		friend class Container;

	protected:
		Container* container;

	private:
		void Build(Container* cont);

	public:
		Module();
		~Module();
		virtual void Load() = 0;
	};


} // namespace compiler::util::di