#pragma once
#include "Commons.h"
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <type_traits>

namespace RWLogger
{
	class Appender;
	using AppenderPtr = std::shared_ptr<Appender>;

	class Logger
	{
	public:
		Logger(const std::string& name, Level lvl = Trace);

		Level level() const { return m_level; }
		void addAppender(AppenderPtr appender);

		template<typename T, typename... Args>
		AppenderPtr addAppender(Args&&... args)
		{
			auto appender = std::make_shared<T>(std::forward<Args>(args)...);
			addAppender(appender);
			return appender;
		}

		void removeAppender(AppenderPtr appender);

		template<typename T>
		void log(Level lvl, T&& val)
		{
			logImpl(lvl, toString(std::forward<T>(val)));
		}

		template<typename Arg, typename... Args>
		void log(Level lvl, Arg&& arg, Args&&... args)
		{
			std::string argsStr;
			argsFolder(argsStr, std::forward<Arg>(arg));
			argsFolder(argsStr, std::forward<Args>(args)...);
			logImpl(lvl, argsStr);
		}

		template<typename Arg>
		void trace(Arg&& arg) { return log(Trace, std::forward<Arg>(arg)); }

		template<typename Arg, typename... Args>
		void trace(Arg&& arg, Args&&... args) { return log(Trace, std::forward<Arg>(arg), std::forward<Args>(args)...); }

		template<typename Arg>
		void debug(Arg&& arg) { return log(Debug, std::forward<Arg>(arg)); }

		template<typename Arg, typename... Args>
		void debug(Arg&& arg, Args&&... args) { return log(Debug, std::forward<Arg>(arg), std::forward<Args>(args)...); }

		template<typename Arg>
		void info(Arg&& arg) { return log(Info, std::forward<Arg>(arg)); }

		template<typename Arg, typename... Args>
		void info(Arg&& arg, Args&&... args) { return log(Info, std::forward<Arg>(arg), std::forward<Args>(args)...); }

		template<typename Arg>
		void warn(Arg&& arg) { return log(Warn, std::forward<Arg>(arg)); }

		template<typename Arg, typename... Args>
		void warn(Arg&& arg, Args&&... args) { return log(Warn, std::forward<Arg>(arg), std::forward<Args>(args)...); }

		template<typename Arg>
		void error(Arg&& arg) { return log(Error, std::forward<Arg>(arg)); }

		template<typename Arg, typename... Args>
		void error(Arg&& arg, Args&&... args) { return log(Error, std::forward<Arg>(arg), std::forward<Args>(args)...); }

		template<typename Arg>
		void fatal(Arg&& arg) { return log(Fatal, std::forward<Arg>(arg)); }

		template<typename Arg, typename... Args>
		void fatal(Arg&& arg, Args&&... args) { return log(Fatal, std::forward<Arg>(arg), std::forward<Args>(args)...); }

	private:
		template <typename T>
		class HasToLogMessageMethod
		{
			template <typename U>
			static std::true_type testSignature(std::string(U::*)());

			template <typename U>
			static decltype(testSignature(&U::toLogMessage)) test(std::nullptr_t);

			template <typename U>
			static std::false_type test(...);

		public:
			using type = decltype(test<T>(nullptr));
			static const bool value = type::value;
		};

		template<typename T>
		typename std::enable_if<
			std::is_same<char, typename std::decay<T>::type>::value
		, std::string>::type
		static toString(T val)
		{
			return { val };
		}

		template<typename T>
		typename std::enable_if<
			std::is_integral<typename std::decay<T>::type>::value
			&& !std::is_same<char, typename std::decay<T>::type>::value
		, std::string>::type
		static toString(T val)
		{
			return std::to_string(val);
		}

		template<typename T>
		typename std::enable_if<
			std::is_floating_point<typename std::decay<T>::type>::value
			&& !std::is_same<char, typename std::decay<T>::type>::value
		, std::string>::type
		static toString(T val)
		{
			auto str = std::to_string(val);
			auto lastSignificantPos = str.find_last_not_of('0');
			if(lastSignificantPos != std::string::npos && lastSignificantPos < str.size() - 1)
				str.erase(lastSignificantPos + 1);
			return str;
		}

		template<typename T>
		typename std::enable_if<
			HasToLogMessageMethod<typename std::decay<T>::type>::value
		, std::string>::type
		static toString(T&& val)
		{
			return val.toLogMessage();
		}

		template<typename T>
		typename std::enable_if<
			std::is_constructible<std::string, typename std::decay<T>::type>::value
			&& !std::is_same<char, typename std::decay<T>::type>::value
			&& !HasToLogMessageMethod<typename std::decay<T>::type>::value
		, std::string>::type
		static toString(T&& val)
		{
			return std::forward<T>(val);
		}

		template<typename T>
		typename std::enable_if<
			!std::is_integral<typename std::decay<T>::type>::value
			&& !std::is_floating_point<typename std::decay<T>::type>::value
			&& !std::is_constructible<std::string, typename std::decay<T>::type>::value
			&& !std::is_same<char, typename std::decay<T>::type>::value
			&& !HasToLogMessageMethod<typename std::decay<T>::type>::value
		, std::string>::type
		static toString(T)
		{
			static_assert (std::is_integral<typename std::decay<T>::type>::value
					|| std::is_floating_point<typename std::decay<T>::type>::value
					|| std::is_constructible<std::string, typename std::decay<T>::type>::value
					|| std::is_same<char, typename std::decay<T>::type>::value
					|| HasToLogMessageMethod<typename std::decay<T>::type>::value
					, "Logger::log accepts integral, floating, constructible of std::string types, "
					  "or custom types that has toLogMessage() method.");
			return {};
		}

		template<typename Arg, typename... Args>
		static void argsFolder(std::string& argsStr, Arg&& arg, Args&&... args)
		{
			argsStr += toString(arg);
			argsFolder(argsStr, std::forward<Args>(args)...);
		}

		static void argsFolder(std::string&) {}

		void logImpl(Level lvl, const std::string& message);

	private:
		std::string m_name;
		std::vector<AppenderPtr> m_appenders;
		std::atomic<Level> m_level;
		std::mutex m_appendersMtx;
	};
}
