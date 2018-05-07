#--------------------------------------------------------------------------
# OpenGL Ray Tracer
#--------------------------------------------------------------------------

PROJECT(opengl_ray_tracer)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("example" FILES ${files_example})

ADD_EXECUTABLE(opengl_ray_tracer ${files_example})
TARGET_LINK_LIBRARIES(opengl_ray_tracer core render)
ADD_DEPENDENCIES(opengl_ray_tracer core render)
