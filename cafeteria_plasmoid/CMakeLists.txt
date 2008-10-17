project(cafeteria_plasmoid)

# Find the required libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

# We add our source code here
set(cafeteria_plasmoid_SRCS
    infopanel.cpp
    cafeteriaplasmoid.cpp)
kde4_add_ui_files(cafeteria_plasmoid_SRCS configuration.ui)

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_cafeteria ${cafeteria_plasmoid_SRCS})
target_link_libraries(plasma_applet_cafeteria
                      ${PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS plasma_applet_cafeteria
        DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-cafeteria.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
