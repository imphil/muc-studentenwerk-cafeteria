project(cafeteria_plasmoid)

# Find the required libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
# only required for KDE 4.1
#find_package(Plasma REQUIRED)

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
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS plasma_applet_cafeteria
        DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-cafeteria.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

# and finally, we do some translations
FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

IF(NOT GETTEXT_MSGFMT_EXECUTABLE OR NOT LOCALE_INSTALL_DIR)
	IF (NOT LOCALE_INSTALL_DIR)
		MESSAGE("NOTE: Use -DLOCALE_INSTALL_DIR=/some/path to define your locale
	                install directory. Locales will not be installed")
	ENDIF(NOT LOCALE_INSTALL_DIR)
	IF (NOT GETTEXT_MSGFMT_EXECUTABLE)
	        MESSAGE("NOTE: msgfmt not found. Translations will *not* be installed")
	ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)
ELSE(NOT GETTEXT_MSGFMT_EXECUTABLE OR NOT LOCALE_INSTALL_DIR)

        SET(catalogname plasma_applet_cafeteria)

        ADD_CUSTOM_TARGET(translations ALL)

        FILE(GLOB PO_FILES po/*.po)

        FOREACH(_poFile ${PO_FILES})
                GET_FILENAME_COMPONENT(_poFileName ${_poFile} NAME)
                STRING(REGEX REPLACE "^${catalogname}_?" "" _langCode ${_poFileName} )
                STRING(REGEX REPLACE "\\.po$" "" _langCode ${_langCode} )

                IF( _langCode )
                        MESSAGE (" " ${LOCALE_INSTALL_DIR})
                        GET_FILENAME_COMPONENT(_lang ${_poFile} NAME_WE)
                        SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

                        ADD_CUSTOM_COMMAND(TARGET translations
                                COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
                                DEPENDS ${_poFile})
                        INSTALL(FILES ${_gmoFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_langCode}/LC_MESSAGES/ RENAME ${catalogname}.mo)
                ENDIF( _langCode )

        ENDFOREACH(_poFile ${PO_FILES})

ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE OR NOT LOCALE_INSTALL_DIR)
