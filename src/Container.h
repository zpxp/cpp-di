#pragma once

#include "RegistrationBase.h"
#include "Registration.h"

#include <unordered_map>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <memory>

namespace cpp_di
{

	class Module;

	class Container
	{
		template <class TYPE, class... ARGS>
		friend class registration::Registration;

		template <class TYPE, class... ARGS>
		friend class registration::RegistrationInstance;

	private:
		std::unordered_map<std::type_index, std::shared_ptr<registration::RegistrationBase>> registeredTypes;
		std::unordered_map<std::type_index, std::shared_ptr<registration::RegistrationBase>> globalSingletons;
		std::unordered_map<std::type_index, std::shared_ptr<registration::RegistrationBase>> scopeInstances;
		std::vector<std::shared_ptr<registration::RegistrationBase>> requestInstances;

		Container* parentContainer;
		const bool isRoot;

		Container();
		Container(Container* parentContainer);
		registration::RegistrationBase* StoreRegistrationData(const std::shared_ptr<registration::RegistrationBase> reg);


		registration::RegistrationBase* TryResolve(const std::type_index& id);
		std::unordered_map<std::type_index, std::shared_ptr<registration::RegistrationBase>>* GetGlobalInstances();


	public:
		~Container();

		void RegisterModule(Module& mod);

		template <class TYPE, class... ARGS>
		/* register a type and provide a construction factory. manage = take ownership of pointer and delete on scope exit */
		registration::Registration<TYPE, ARGS...> Register(TYPE* factory(Container&, ARGS...), bool manage = true)
		{
			return Register<TYPE, ARGS...>(std::function<TYPE*(Container&, ARGS...)>(factory), manage);
		}


		template <class TYPE, class... ARGS>
		/* register a type and provide a construction factory. manage = take ownership of pointer and delete on scope exit */
		registration::Registration<TYPE, ARGS...> Register(std::function<TYPE*(Container&, ARGS...)> const& factory, bool manage = true)
		{
			return registration::Registration<TYPE, ARGS...>(this, std::type_index(typeid(TYPE)), factory, manage);
		}

		template <class TYPE, class... ARGS>
		/* register a type with the default ctor. manage = take ownership of pointer and delete on scope exit */
		registration::Registration<TYPE, ARGS...> Register()
		{
			return Register<TYPE, ARGS...>(
				+[](Container&, ARGS... args) {
					return new TYPE(args...);
				},
				true);
		}

		template <class IFace, class... ARGS>
		IFace* Resolve(ARGS... args)
		{
			auto typeIndex = std::type_index(typeid(IFace));
			auto item = this->TryResolve(typeIndex);
			registration::Registration<IFace, ARGS...>* casted = static_cast<registration::Registration<IFace, ARGS...>*>(item);
			auto global = this->GetGlobalInstances();
			return casted->TryGetInstance(*this, &this->scopeInstances, global, &requestInstances, args...);
		}


		Container NewScope();

		/* allocate new scope on heap. remember to `delete` */
		Container* NewScopeAlloc();

		const Container* GetRootContainer() const;

		static Container Create();
	};


} // namespace cpp_di