#=============================================================================
# Copyright 2014 Luís Pereira <luis.artur.pereira@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================
#
# funtion lxqt_translate_ts(qmFiles
#                           [UPDATE_TRANSLATIONS [Yes | No]]
#                           SOURCES <sources>
#                           [TEMPLATE] translation_template
#                           [TRANSLATION_DIR] translation_directory
#                           [INSTALL_DIR] install_directory
#                    )
#     Output:
#       qmFiles The generated compiled translations (.qm) files
#
#     Input:
#       UPDATE_TRANSLATIONS Optional flag. Setting it to Yes, extracts and
#                           compiles the translations. Setting it No, only
#                           compiles them.
#
#       TEMPLATE Optional translations files base name. Defaults to
#                ${PROJECT_NAME}. An .ts extensions is added.
#
#       TRANSLATION_DIR Optional path to the directory with the .ts files,
#                        relative to the CMakeList.txt. Defaults to
#                        "translations".
#
#       INSTALL_DIR Optional destination of the file compiled files (qmFiles).
#                    If not present no installation is performed



# CMake v2.8.3 needed to use the CMakeParseArguments module
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)


function(lxqt_translate_ts qmFiles)
    set(oneValueArgs UPDATE_TRANSLATIONS TEMPLATE TRANSLATION_DIR INSTALL_DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(TR "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED TR_UPDATE_TRANSLATIONS)
        set(TR_UPDATE_TRANSLATIONS "No")
    endif()

    if(NOT DEFINED TR_TEMPLATE)
        set(TR_TEMPLATE "${PROJECT_NAME}")
    endif()

    if (NOT DEFINED TR_TRANSLATION_DIR)
        set(TR_TRANSLATION_DIR "translations")
    endif()

    file(GLOB tsFiles "${TR_TRANSLATION_DIR}/${TR_TEMPLATE}_*.ts")
    set(templateFile "${TR_TRANSLATION_DIR}/${TR_TEMPLATE}.ts")

    if (TR_UPDATE_TRANSLATIONS)
        qt6_create_translation(QMS
            ${TR_SOURCES}
            ${templateFile}
            OPTIONS -locations absolute
        )
        qt6_create_translation(QM
            ${TR_SOURCES}
            ${tsFiles}
            OPTIONS -locations absolute
        )
    else()
        qt6_add_translation(QM ${tsFiles})
    endif()

    if(TR_UPDATE_TRANSLATIONS)
        add_custom_target("update_${TR_TEMPLATE}_ts" ALL DEPENDS ${QMS})
    endif()

    if(DEFINED TR_INSTALL_DIR)
        install(FILES ${QM} DESTINATION ${TR_INSTALL_DIR})
    endif()

    set(${qmFiles} ${QM} PARENT_SCOPE)
endfunction()
