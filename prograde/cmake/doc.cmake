#    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# Additional target to perform clang-format/clang-tidy run
# Requires clang-format and clang-tidy

find_package(Doxygen)

if(DOXYGEN_FOUND)
		message("doxygen executable: ${DOXYGEN_EXECUTABLE}")
		message("doxygen version: ${DOXYGEN_VERSION}")
else()
        message("doxygen executable not found")
endif()


# Get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES ${MAIN_SRC_FILES} ${TEST_FILES})

list(REMOVE_DUPLICATES ALL_SOURCE_FILES)

set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_target(doc
        COMMAND ${PROJECT_SOURCE_DIR}/dev/doc-report.sh ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
