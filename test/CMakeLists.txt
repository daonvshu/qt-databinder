project(mvvm-test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt5 COMPONENTS
            Core
            Gui
            Widgets
            REQUIRED)

add_executable(${PROJECT_NAME} WIN32
    main.cpp

    pagea.h
    pagea.cpp

    pageb.h
    pageb.cpp

    singleinstance.h
    singleinstance.cpp

    ${CMAKE_SOURCE_DIR}/src/propertybinder.h
    ${CMAKE_SOURCE_DIR}/src/asyncdatasetter.h
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)