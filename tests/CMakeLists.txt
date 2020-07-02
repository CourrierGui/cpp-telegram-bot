add_executable(Cppbot.test-buffer test-buffer.cpp)
target_link_libraries(Cppbot.test-buffer
  PRIVATE
    gtest gtest_main
		cppbot-utils
)

gtest_add_tests(Cppbot.test-buffer "" AUTO)
