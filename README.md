# cpp-di
A super lightweight C++ Dependency Injection utilizing templates. Requires C++ 17 or greater.

### Install
```
git clone https://github.com/zpxp/cpp-di.git
cd cpp-di
mkdir build
cd build
cmake ..
sudo make install
```


### Use:

*ILogger.h*
```cpp

class ILogger
{
public:
	virtual void Warn(const std::string& msg) = 0;
	virtual void Error(const std::string& msg, bool terminate = false) = 0;
	virtual void Message(const std::string& msg) = 0;
	virtual ~ILogger(){};
};
```

*Logger.h*
```cpp
#include "ILogger.h"
#include <cpp_di/di.h>

class Logger : public ILogger
{
private:
	int member:

	virtual void Warn(const std::string& msg) override;
	virtual void Error(const std::string& msg ,bool terminate = false) override;
	virtual void Message(const std::string& msg) override;

	Logger();
	~Logger() override;
};


// di module
class LoggerModule : public cpp_di::Module
{
public:
	void Load() override;
};
```

*Logger.cpp*
``` cpp
#include "Logger.h"

/// Logger implementation here ...

// module implementation
void LoggerModule::Load()
{
	container->Register<Logger>()
		.As<ILogger>()
		.GlobalSingleton();
}
```

Now add the logger to the container
``` cpp
auto root_container = cpp_di::Container::Create();
auto loggermod = LoggerModule();
root_container.RegisterModule(loggermod);
```

The container instance can now be used to resolve an instance with `Resolve<>`
```cpp
ILogger logger = root_container.Resolve<ILogger>();
```

Use `Container.NewScope` to create a sub scope that owns all instances created
with the sub scope. Whenever a container is destructed, it cleans up all `InstancePerRequest`
and `InstancePerScope` resolved items. Resolved items registered with `GlobalSingleton` are
only cleaned up when the root scope is destructed.

| Module Load Option | Use|
|--------|----|
|`InstancePerRequest`|A new instance is created on every call to `Resolve<>`|
|`InstancePerScope`|A new instance is created once per container scope instance and lasts for the duration of that scope|
|`GlobalSingleton`|One instance is created for all containers under the single root container scope|
|`As<>`|Used to register a concrete type as a virtual interface type |

### Flags
Add `-lcpp_di` to your compiler flags

### Advanced
More complicated types can be constructed in the `Module::Load` implementation by passing a factory lambda to the `Register<>` function. Use this lambda along with the `cpp_di::Container& scope` scope argument to construct the required type.
```cpp
void CompilationModule::Load()
{
	container->Register<compilation::Compilation>(+[](cpp_di::Container& scope) {
				// use scope to resolve a channel and a logger
				 return new compilation::Compilation(&scope, scope.Resolve<channel::Channel>(), scope.Resolve<logger::ILogger>());
			 })
		.InstancePerScope();

	// register same instance as ICompilation for external use
	container->Register<compilation::Compilation>(
				 +[](cpp_di::Container& scope) {
					 return scope.Resolve<Compilation>();
				 },
				 // pass false to not take ownership of this pointer
				 false)
		.As<ICompilation>()
		.InstancePerScope();
}
```