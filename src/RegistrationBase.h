#pragma once

#include <typeinfo>
#include <typeindex>
#include <vector>

namespace compiler::util::di
{
	class Container;

	namespace registration
	{

		enum InjectTypes
		{
			/*	 1 instance for the root container and all children containers */
			GlobalSingleton,
			/*	 1 instance for this container context */
			InstancePerScope,
			/*   make a new instance on every resolve*/
			InstancePerRequest
		};


		/* Base class to allow templates of diff types to be stored in same lists/maps */
		class RegistrationBase
		{
			friend class ::compiler::util::di::Container;

			template <class TYPE2, class... ARG2S>
			friend class Registration;

		protected:
			Container* container;
			InjectTypes type;
			const std::type_index typeIndex;


			RegistrationBase(Container* _container, const std::type_index& type)
			: container(_container), typeIndex(type)
			{
			}

			virtual size_t InstanceCount() = 0;
		};


	} // namespace registration
} // namespace compiler::util::di