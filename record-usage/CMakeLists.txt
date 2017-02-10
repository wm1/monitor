set(TARGET record-usage)

add_definitions(-DPROJ=${TARGET})

set(SOURCES
    findapp.cpp
    logger.cpp
    mainframe.cpp
    timer.cpp
    win32helper.cpp)

add_executable(
    ${TARGET}
    WIN32
    ${SOURCES}
    version.rc
    )

target_link_libraries(${TARGET} kernel32)
target_link_libraries(${TARGET} WtsApi32)
