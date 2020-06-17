#pragma once

#include "RegistrationBase.h"
#include "Container.h"

#include <typeinfo>
#include <typeindex>
#include <functional>
#include <utility>
#include <vector>
#include <memory>

namespace cpp_di
{
	class Container;

	namespace registration
	{


		template <class TYPE, class IFace, class... ARGS>
		class RegistrationAs;

		template <class TYPE, class... ARGS>
		class RegistrationInstance;

		using regMap = std::unordered_map<std::type_index, std::shared_ptr<registration::RegistrationBase>>;


		template <class TYPE, class... ARGS>
		/* A class that holds registration meta data. the actual instances are stored in  `RegistrationInstance`*/
		class Registration : public RegistrationBase
		{
			friend class ::cpp_di::Container;

			template <class TYPE2, class... ARG2S>
			friend class Registration;


		protected:
			std::function<TYPE*(Container& scope, ARGS... args)> const typeFactory;
			const bool manageOwnInstances;

			Registration(Container* _container, const std::type_index& type, std::function<TYPE*(Container&, ARGS...)> const& factory, const bool manage)
			: RegistrationBase(_container, type), typeFactory(factory), manageOwnInstances(manage)
			{
			}

			size_t InstanceCount() override
			{
				return 0;
			}


			/* handler to resolve instances. if the instance does not exits, it creates a new instance registration and stores it in the correct container context */
			TYPE* TryGetInstance(Container& scope,
				regMap* scopeInstances,
				regMap* globalInstances,
				std::vector<std::shared_ptr<registration::RegistrationBase>>* requestInstances,
				ARGS... args)
			{
				using typedMap = const std::unordered_map<std::type_index, std::shared_ptr<RegistrationInstance<TYPE*, ARGS...>>>;

				switch (this->type)
				{
					case InjectTypes::GlobalSingleton:
					{
						// 1 instance for the root container and all children containers
						auto exists = globalInstances->find(this->typeIndex);
						if (exists != globalInstances->end())
						{
							std::shared_ptr<RegistrationInstance<TYPE, ARGS...>> casted = std::static_pointer_cast<RegistrationInstance<TYPE, ARGS...>>(exists->second);
							if (casted->instances.size() > 0)
							{
								TYPE* inst = casted->instances[0];
								return inst;
							}
							else
							{
								return casted->GenerateInstance(scope, args...);
							}
						}
						else
						{
							auto inst = std::make_shared<RegistrationInstance<TYPE, ARGS...>>(this);
							globalInstances->insert(std::make_pair(this->typeIndex, inst));
							return inst->GenerateInstance(scope, args...);
						}
					}
					break;

					case InjectTypes::InstancePerScope:
					{
						// 1 instance for this container context
						auto exists = scopeInstances->find(this->typeIndex);
						if (exists != scopeInstances->end())
						{
							std::shared_ptr<RegistrationInstance<TYPE, ARGS...>> casted = std::static_pointer_cast<RegistrationInstance<TYPE, ARGS...>>(exists->second);
							if (casted->instances.size() > 0)
							{
								TYPE* inst = casted->instances[0];
								return inst;
							}
							else
							{
								return casted->GenerateInstance(scope, args...);
							}
						}
						else
						{
							auto inst = std::make_shared<RegistrationInstance<TYPE, ARGS...>>(this);
							scopeInstances->insert(std::make_pair(this->typeIndex, inst));
							return inst->GenerateInstance(scope, args...);
						}
					}
					break;

					case InjectTypes::InstancePerRequest:
					{
						// make a new instance on every resolve
						auto inst = std::make_shared<RegistrationInstance<TYPE, ARGS...>>(this);
						requestInstances->push_back(inst);
						return inst->GenerateInstance(scope, args...);
					}
					break;
				}
			}

		public:
			~Registration() = default;

			template <class IFace>
			/* Resolve registered type as `IFace` */
			Registration As() const
			{
				return RegistrationAs<TYPE, IFace, ARGS...>(this->container, std::type_index(typeid(IFace)), this->typeFactory, this->manageOwnInstances);
			}

			/*   make a new instance on every resolve*/
			void InstancePerRequest()
			{
				auto stored = std::make_shared<Registration<TYPE, ARGS...>>(*this);
				this->container->StoreRegistrationData(stored);
				stored->type = InjectTypes::InstancePerRequest;
			}
			/*	 1 instance for this container context */
			void InstancePerScope()
			{
				auto stored = std::make_shared<Registration<TYPE, ARGS...>>(*this);
				this->container->StoreRegistrationData(stored);
				stored->type = InjectTypes::InstancePerScope;
			}

			/*	 1 instance for the root container and all children containers */
			void GlobalSingleton()
			{
				auto stored = std::make_shared<Registration<TYPE, ARGS...>>(*this);
				this->container->StoreRegistrationData(stored);
				stored->type = InjectTypes::GlobalSingleton;
			}
		};


		template <class TYPE, class... ARGS>
		/* a registration that actually holds its generated instances */
		class RegistrationInstance : public Registration<TYPE, ARGS...>
		{
			friend class ::cpp_di::Container;

			template <class TYPE2, class... ARGS2>
			friend class Registration;

			template <class TYPE2, class IFace, class... ARGS2>
			friend class RegistrationAs;

		protected:
			std::vector<TYPE*> instances;

			RegistrationInstance(Container* _container, const std::type_index& type, std::function<TYPE*(Container&, ARGS...)> const& factory, const bool manage)
			: Registration<TYPE, ARGS...>(_container, type, factory, manage)
			{
			}


			RegistrationInstance(RegistrationInstance&&) = default;
			RegistrationInstance(const RegistrationInstance&) = default;
			RegistrationInstance& operator=(RegistrationInstance&&) = default;
			RegistrationInstance& operator=(const RegistrationInstance&) = default;

			size_t InstanceCount() override
			{
				return instances.size();
			}

			TYPE* GenerateInstance(Container& scope, ARGS... args)
			{
				TYPE* inst = this->typeFactory(scope, args...);
				auto exists = std::find(instances.begin(), instances.end(), inst);
				if (exists == instances.end())
				{
					instances.push_back(inst);
				}
				return inst;
			}


		public:
			RegistrationInstance(const Registration<TYPE, ARGS...>* other)
			: Registration<TYPE, ARGS...>(*other)
			{
			}

			~RegistrationInstance()
			{
				if (this->manageOwnInstances)
				{
					for (auto inst : instances)
						if (inst != NULL)
							delete inst;
				}
			}
		};


		template <class TYPE, class IFace, class... ARGS>
		/* override the GenerateInstance func to return as the Interface type */
		class RegistrationAs : public RegistrationInstance<TYPE, ARGS...>
		{
			template <class TYPE2, class... ARGS2>
			friend class Registration;

		private:
		protected:
			RegistrationAs(Container* _container, const std::type_index& type, std::function<TYPE*(Container&, ARGS...)> const& factory, const bool manage)
			: RegistrationInstance<TYPE, ARGS...>(_container, type, factory, manage)
			{
			}

		public:
			~RegistrationAs() = default;

			IFace* GenerateInstance(const Container& scope, ARGS... args)
			{
				TYPE* inst = this->typeFactory(scope, args...);
				auto exists = std::find(this->instances.begin(), this->instances.end(), inst);
				if (exists == this->instances.end())
				{
					this->instances.push_back(inst);
				}
				return static_cast<IFace*>(inst);
			}
		};


	} // namespace registration

} // namespace cpp_di
