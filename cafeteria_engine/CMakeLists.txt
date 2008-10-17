project(cafeteria_engine)

# Find the required libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_QTNETWORK_INCLUDES}
    ${QT_QTXML_INCLUDES}
    ${KDE4_INCLUDES})

# We add our source code here
set(cafeteria_engine_SRCS
    cafeteriaengine.cpp
    cafeteriajob.cpp
    locationsource.cpp
    cafeteriamenusource.cpp)

# Now make sure all files get to the right place
kde4_add_plugin(plasma_engine_cafeteria ${cafeteria_engine_SRCS})
target_link_libraries(plasma_engine_cafeteria
                      ${PLASMA_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_QTXML_LIBS})

install(TARGETS plasma_engine_cafeteria
        DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-engine-cafeteria.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
