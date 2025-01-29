cmake_minimum_required(VERSION 3.10)


add_executable(iPerfer iPerfer.cpp)


target_link_libraries(iPerfer
    PRIVATE
        spdlog::spdlog
        cxxopts::cxxopts
)
