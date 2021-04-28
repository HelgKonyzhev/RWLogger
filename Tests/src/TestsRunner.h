#pragma once
#include <Appender.h>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <cassert>

class Test
{
public:
	Test(const std::string& name)
		: m_name(name)
	{}

	const std::string& name() const { return m_name; }
	virtual void run() noexcept(false) = 0;

private:
	std::string m_name;
};


class TestsCollection
{
	using TestCreator = std::function<Test*()>;
	using TestsCreators = std::unordered_map<std::string, TestCreator>;

public:
	template<typename T>
	struct Registrator
	{
		Registrator(const std::string& name)
		{
			TestsCollection::instance().registerTest<T>(name);
		}
	};

	static TestsCollection& instance()
	{
		static TestsCollection inst;
		return inst;
	}

	template<typename T>
	void registerTest(const std::string& name)
	{
		m_creators[name] = [name]()->Test* { return new T(name); };
	}

	const TestsCreators& registeredTests() const { return m_creators; }

private:
	TestsCreators m_creators;
};


class TestsRunner
{
	using TestPtr = std::unique_ptr<Test>;

public:
	static void testFormat(std::ostream& ostream, const RWLogger::Appender::LoggingEvent& event);

	TestsRunner();

	void run() noexcept(true);

private:
	std::unordered_map<std::string, TestPtr> m_tests;
};


#define REGISTER_TEST(testClassName) \
	static TestsCollection::Registrator<testClassName> testClassName ## Registrator{ #testClassName };

#define ASSERT(statement) \
	do { \
		if(!(statement)) throw std::logic_error(std::string("\"") + #statement + "\" failed in " + __FILE__ + ": " + std::to_string(__LINE__)); \
	} while(false);
