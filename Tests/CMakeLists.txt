add_executable (Tests
	${Logger}
	
    ArgsToStringTest.cpp
    AsyncAppenderTest.cpp
    ConsoleAppendersTest.cpp
    FileAppenderTest.cpp
    LevelsTest.cpp
    MultipleThreadsTest.cpp
    TestsRunner.cpp
    main.cpp

    TestsRunner.h
)

target_include_directories(Tests PRIVATE ../Logger)