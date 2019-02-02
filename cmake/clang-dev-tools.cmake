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

find_package(ClangFormat3.8)

if(CLANG_FORMAT_FOUND)
        message("clang-format-3.8 executable: ${CLANG_FORMAT_EXECUTABLE}")
        message("clang-format-3.8 version: ${CLANG_FORMAT_VERSION}")
else()
        message("clang-format-3.8 executable not found")
endif()

find_package(ClangTidy)

if(CLANG_FORMAT_FOUND)
	message("clang-tidy executable: ${CLANG_TIDY_EXECUTABLE}")
		message("clang-tidy version: ${CLANG_TIDY_VERSION}")
else()
        message("clang-tidy executable not found")
endif()

# Get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES ${MAIN_SRC_FILES} ${TEST_FILES})

list(REMOVE_DUPLICATES ALL_SOURCE_FILES)

add_custom_target(
        clang-format
		COMMAND ${PROJECT_SOURCE_DIR}/dev/clang-format-report.sh
		${CLANG_FORMAT_EXECUTABLE}
        ${ALL_SOURCE_FILES}
)

set(SYSINC "")
foreach(INC ${GOOGLE_TEST_INCLUDE_DIRS} ${URHO3D_INCLUDE_DIR})
	list(APPEND SYSINC "-isystem"  "${INC}" "")
endforeach(INC)

add_custom_target(
        clang-tidy
		COMMAND ${PROJECT_SOURCE_DIR}/dev/clang-tidy-report.sh
		${CLANG_TIDY_EXECUTABLE}
		${ALL_SOURCE_FILES}
		config=''
		-system-headers=0
		--
		-std=gnu++11
		-I${COMMON_INCLUDES}
		${SYSINC}
)
