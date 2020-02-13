IF(NOT InMMEX)
    MESSAGE(FATAL_ERROR "Use the top-level CMake script!")
ENDIF(NOT InMMEX)

ADD_CUSTOM_TARGET(Translations ALL)
FILE(GLOB POFiles *.po)

FOREACH(POFile ${POFiles})

    GET_FILENAME_COMPONENT(CurrentFile ${POFile} NAME_WE)
    SET(MOFile ${CurrentFile}.mo)

    ADD_CUSTOM_COMMAND(TARGET Translations
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${MOFile} ${POFile}
            DEPENDS ${POFile})
    INSTALL(FILES ${MOFile} DESTINATION ${INSTALL_MO_DIR})
ENDFOREACH()