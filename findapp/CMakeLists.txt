set(TARGET findapp)

add_definitions(-DPROJ=${TARGET})

set(SOURCES
    findapp.cpp
    timer.cpp
    win32helper.cpp)

add_executable(
    ${TARGET}
    ${SOURCES}
    )

target_link_libraries(${TARGET} kernel32)
