set(TARGET limit-time)

add_definitions(-DPROJ=${TARGET})

set(SOURCES
    main.cpp
    )

add_executable(
    ${TARGET}
    WIN32
    ${SOURCES}
    version.rc
    )

target_link_libraries(${TARGET} kernel32)
