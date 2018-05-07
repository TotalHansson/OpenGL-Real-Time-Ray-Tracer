#--------------------------------------------------------------------------
# engine
#--------------------------------------------------------------------------

ADD_LIBRARY(engine INTERFACE)
TARGET_INCLUDE_DIRECTORIES(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#TARGET_LINK_LIBRARIES(engine INTERFACE ${OPENGL_LIBS})
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(render)

SET_TARGET_PROPERTIES(core PROPERTIES FOLDER "gscept/lab_env")
SET_TARGET_PROPERTIES(render PROPERTIES FOLDER "gscept/lab_env")
