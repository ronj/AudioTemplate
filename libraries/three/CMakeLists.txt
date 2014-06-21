include_directories(${LIBRARIES_INCLUDE_DIR})

if (NOT STATIC_LIBRARIES)
  add_definitions(-DTHREE_DYN_LINK)
endif()

if (ANDROID)
  add_definitions(-DTHREE_GLES)
endif()

set(THREE_SOURCES "core/impl/buffer_geometry.cpp"
                  "core/impl/geometry.cpp"
                  "core/impl/geometry_buffer.cpp"
                  "core/impl/object3d.cpp"
                  "core/impl/projector.cpp"
                  "core/impl/raycaster.cpp"
                  "extras/geometries/impl/box_geometry.cpp"
                  "extras/geometries/impl/circle_geometry.cpp"
                  "extras/geometries/impl/cylinder_geometry.cpp"
                  "extras/geometries/impl/icosahedron_geometry.cpp"
                  "extras/geometries/impl/lathe_geometries.cpp"
                  "extras/geometries/impl/octahedron_geometries.cpp"
                  "extras/geometries/impl/polyhedron_geometry.cpp"
                  "extras/geometries/impl/ring_geometry.cpp"
                  "extras/geometries/impl/tetrahedron_geometry.cpp"
                  "extras/geometries/impl/text_2d_geometry.cpp"
                  "extras/geometries/impl/torus_knot_geometry.cpp"
                  "extras/geometries/impl/plane_geometry.cpp"
                  "extras/geometries/impl/sphere_geometry.cpp"
                  "extras/geometries/impl/sphere_geometry.cpp"
                  "extras/geometries/impl/torus_geometry.cpp"
                  "extras/helpers/impl/arrow_helper.cpp"
                  "extras/helpers/impl/axis_helper.cpp"
                  "extras/impl/geometry_utils.cpp"
                  "extras/impl/image_utils.cpp"
                  "extras/impl/scene_utils.cpp"
                  "extras/utils/impl/font.cpp"
                  "math/impl/box2.cpp"
                  "math/impl/box3.cpp"
                  "math/impl/color.cpp"
                  "math/impl/euler.cpp"
                  "math/impl/frustum.cpp"
                  "math/impl/line3.cpp"
                  "math/impl/math.cpp"
                  "math/impl/matrix3.cpp"
                  "math/impl/matrix4.cpp"
                  "math/impl/plane.cpp"
                  "math/impl/quaternion.cpp"
                  "math/impl/ray.cpp"
                  "math/impl/sphere.cpp"
                  "math/impl/spline.cpp"
                  "math/impl/triangle.cpp"
                  "math/impl/vector3.cpp"
                  "math/impl/vector4.cpp"
                  "renderers/impl/gl_renderer.cpp"
                  "renderers/shaders/impl/shader_chunk.cpp"
                  "renderers/shaders/impl/shader_lib.cpp"
                  "renderers/shaders/impl/uniforms_lib.cpp"
                  "impl/console.cpp"
                  "impl/gl.cpp"
                  "materials/impl/material.cpp"
                  "materials/impl/text_2d_material.cpp"
                  "materials/impl/uniform.cpp"
                  "objects/impl/bone.cpp"
                  "objects/impl/LOD.cpp"
                  "objects/impl/mesh.cpp"
                  "scenes/impl/scene.cpp")

set(THREE_LIB_SOURCE_FILES ${THREE_SOURCES})

set(THREE_LIB three CACHE INTERNAL "three library" FORCE)
invent_add_library(${THREE_LIB} ${THREE_LIB_SOURCE_FILES})

set_target_properties(${THREE_LIB} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter")

if (BUILD_TESTS)
  #_subdirs(tests)
endif()
