Introduction
============
RWLogger is a set of C++11 classes for output application log messages to multiple outputs.

Features
========
-Supports multiple outputs (file, console).
-Supports output from multiple threads.
-Supports output of user types.
-Supports asynchronous output.
-Provides API to configure output format and expand outputs list.
-API validated with unit tests
-Hasn't 3rd party dependencies

Supported OS
============
* Linux
* Windows

Installation
============
There is source files only and don't require build or installation. They supposed to be included in target project.

Tests and Example compilation (for Windows)
===========================================
There is Tests and Example projects provided. They can be compiled by CMake.

Preconditions:
1. Build tools for Visual Studio are installed ( https://visualstudio.microsoft.com/ru/downloads/ ).
2. Path to MSBuild.exe added to evironment variable PATH.
3. CMake 3.8 or later is installed and added to evironment variable PATH too.

Build steps:
cd RWLogger
cmake .
msbuild RWLogger.sln /property:Configuration=Release

Output binaries:
Tests/Release/Tests.exe
Example/Release/Example.exe

Tests usage
===========
cd RWLogger/Tests/Release
Tests.exe

Example usage
=============
cd RWLoggerExample/Release
Example.exe [options]

Options:
-L <logging-level> = Specify logging level of the logger.
                     Possible values: Trace, Debug, Info, Warn, Error, Fatal.
-l <logging-level> = Specify logging level of the following message.
                     Possible values: Trace, Debug, Info, Warn, Error, Fatal.
                     By default Info level used.
-m <message>       = Appending message.
-c <out-type>      = Add console appender of <out-type> that can be either "cout" or "cerr",
                     that means stdout or stderr output respectively.
-f <filename>      = Add file appender that will append to file spicified by <filename>.
-F <filename>      = Add file appender that will first discard content and then append to file spicified by <filename>.
-t <threads-count> = Specify number of threads that will append specified messages. Each thread will append all messages.
                     By default one thread is used.
-h                   Print help.

Usage example:
Example.exe -L Debug -c cout -t 2 -l Debug -m "Debug message 1" -m "Debug message 2" -l Info -m "Info message" -l Trace -m "Trace message"

Output:
2021-04-30 13:06:09.096 DEBUG   0x00000001 Debug message 1
2021-04-30 13:06:09.096 DEBUG   0x00000002 Debug message 1
2021-04-30 13:06:09.096 DEBUG   0x00000001 Debug message 2
2021-04-30 13:06:09.096 DEBUG   0x00000002 Debug message 2
2021-04-30 13:06:09.097 INFO    0x00000001 Info message
2021-04-30 13:06:09.097 INFO    0x00000002 Info message

Known issues
============
1. Instaces of RWLogger::Logger are not synchronized with each other.
   So if more than one instance RWLogger::Logger owns same instance 
   of RWLogger::CoutAppender or RWLogger::CerrAppender or RWLogger::FileAppender,
   data races when logging from different threads are possible. 
   It doesn't apply to AsyncAppender because of it's underlying buffer synchronization.
2. RWLogger::Logger locks mutex that guards appenders list on each logging operation.
   But it's not necessary if user don't intending to add appenders while logging 
   or if all this operations processing in same thread. This can be made customizable.
   
Ideas for further development
=============================
1. Resolve known issues such way:
   1. Add RWLogger::FIFOMutex member to RWLogger::Appender class 
      and implement public interface to lock, unlock, try_lock it.
   2. Get rid of std::mutex member of RWLogger::AsyncAppender 
      and make it synchronize underlying buffer via lock, unlock, try_lock it methods.
   3. Add locking and unlocking of appender in RWLogger::Logger::logImpl before and after appending.
   4. Add boolean flag that will turn on/off usage of mutex that guards appenders list to RWLogger::Logger.
      And provide public mthod to set it. By default usage of mutex that guards appenders list should be enabled.
   5. Add boolean flag that will turn on/off usage of mutex that guards appending to RWLogger::Appender.
      And provide public mthod to set it. By default usage of mutex that guards appending should be enabled.
2. Implement logging of log context (source filename, line number, call stack).
3. Introduce logger name concept to make loggeng more customizable. Each instance of RWLogger::Logger should
   add it's name in each log record for example like this:
   2021-04-30 13:06:09.097 INFO    <Logger name> - 0x00000002 Info message.
4. Implement default sharing of appenders between loggers. Purpose is to allow user create one instance of RWLogger::Logger,
   setup appenders for it, and then each new instance RWLogger::Logger should have that appenders.
   For example it could be inheritace concept. Each appender can have parent and children appenders. 
   Children appenders have all appenders of the parent.
5. Implement configuration file that describes loggers hierarchy, log levels of loggers, appenders of loggers.