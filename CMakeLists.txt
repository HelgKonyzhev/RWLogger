cmake_minimum_required (VERSION 3.8)

project(RWLogger)

set(Logger
	"../Logger/FIFOMutex.cpp"
    "../Logger/Logger.cpp"
    "../Logger/Appender.cpp"
    "../Logger/ConsoleAppender.cpp"
    "../Logger/FileAppender.cpp"
    "../Logger/AsyncAppender.cpp"

	"../Logger/FIFOMutex.h"
    "../Logger/Logger.h"
    "../Logger/Appender.h"
    "../Logger/Commons.h"
    "../Logger/ConsoleAppender.h"
    "../Logger/FileAppender.h"
    "../Logger/AsyncAppender.h"
)

add_subdirectory(Example)
add_subdirectory(Tests)
