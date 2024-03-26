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
# These functions enables "automatic" translation loading in Qt6 apps
#   and libs. They generate a .cpp file that takes care of everything. The
#   user doesn't have to do anything in the source code.
#
# Qt6_translation_loader(<source_files> <translations_dir> <catalog_name>)
#
# Output:
#   <source_files> Appends the generated file to this variable.
#
# Input:
#   <translations_dir> Full path name to the translations dir.
#   <catalog_name> Translation catalog to be loaded.

set(__CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

function(qt6_translation_loader source_files translations_dir catalog_name)
    configure_file(
        ${__CURRENT_DIR}/Qt6TranslationLoader.cpp.in
        Qt6TranslationLoader.cpp @ONLY
    )
    set(${source_files} ${${source_files}} ${CMAKE_CURRENT_BINARY_DIR}/Qt6TranslationLoader.cpp PARENT_SCOPE)
endfunction()
