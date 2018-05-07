#--------------------------------------------------------------------------
# projects
#--------------------------------------------------------------------------
FILE(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*)
FOREACH(child ${children})
	IF(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${child})
		IF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${child}/CMakeLists.txt)
			ADD_SUBDIRECTORY(${child})
		ENDIF()
	ENDIF()
ENDFOREACH()
