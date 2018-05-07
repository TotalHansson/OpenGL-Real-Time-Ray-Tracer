#--------------------------------------------------------------------------
# ImGuiExample project
#--------------------------------------------------------------------------

PROJECT(ImGuiExample)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("example" FILES ${files_example})

ADD_EXECUTABLE(ImGuiExample ${files_example})
TARGET_LINK_LIBRARIES(ImGuiExample core render)
ADD_DEPENDENCIES(ImGuiExample core render)