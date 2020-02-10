project(WotionX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(src SRC_LIST)
include_directories(inc)

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base adv net REQUIRED)
# Should make sqlite3 more generic
include(${wxWidgets_USE_FILE} /usr/include/sqlite3.h)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} sqlite3)
