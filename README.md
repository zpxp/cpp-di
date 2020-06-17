# cpp-di
A super lightweight C++ Dependency Injection utilizing templates.

Use:

```cpp
#include "cpp_di/Injector.h"


// ILogger.h
class ILogger
{
public:
	virtual void Warn(const std::string& msg) = 0;
	virtual void Error(const std::string& msg, bool terminate = false) = 0;
	virtual void Message(const std::string& msg) = 0;
	virtual ~ILogger(){};
};


// Logger.h
class Logger : public ILogger
{
private:
int ic:
	virtual void Warn(const std::string& msg) override;
	virtual void Error(const std::string& msg ,bool terminate = false) override;
	virtual void Message(const std::string& msg) override;

	Logger();
	~Logger() override;
};


// di module
class LoggerModule : public util::di::Module
{
public:
	void Load() override;
};


// Logger.cpp

/// ... implementation

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

ILogger logger = root_container.Resolve<ILogger>();

```


More complicated types can be constructed in the `Module::Load` implentation
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
				 false)
		.As<ICompilation>()
		.InstancePerScope();
}
```