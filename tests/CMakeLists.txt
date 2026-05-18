cmake_minimum_required(VERSION 3.15)

# Unit tests for libmoon_assets decompressors
add_executable(test_decompressors test_decompressors.c)
target_link_libraries(test_decompressors PRIVATE moon_assets)

add_test(NAME test_decompressors COMMAND test_decompressors)

add_executable(test_cache test_cache.c)
target_link_libraries(test_cache PRIVATE moon_assets)

add_test(NAME test_cache COMMAND test_cache)
