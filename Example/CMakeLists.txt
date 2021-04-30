add_executable (Example
	${Logger}
	main.cpp
)

target_include_directories(Example PRIVATE ../Logger)