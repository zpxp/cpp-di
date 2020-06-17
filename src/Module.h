#pragma once

#include "Container.h"

namespace cpp_di
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


} // namespace cpp_di