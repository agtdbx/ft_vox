project('ft_vox', 'cpp',
  version : '1.0',
  default_options : ['warning_level=1', 'cpp_std=c++17'])

# TODO: REMOVE FOR PERF
add_global_arguments('-g', language : 'cpp')

srcs = [
  'srcs/program/perfLogger/PerfLogger.cpp',
  'srcs/program/skybox/Skybox.cpp',
  'srcs/program/bytes/uint256_t.cpp',
  'srcs/program/bytes/bitFunctions.cpp',
  'srcs/program/loop/computation.cpp',
  'srcs/program/loop/events.cpp',
  'srcs/program/loop/draw.cpp',
  'srcs/program/loop/init.cpp',
  'srcs/program/map/Chunk_generation.cpp',
  'srcs/program/map/Chunk.cpp',
  'srcs/program/map/Chunk_meshing.cpp',
  'srcs/program/map/PerlinNoise.cpp',
  'srcs/program/map/Map.cpp',
  'srcs/program/map/Cluster.cpp',
  'srcs/program/map/Map_update.cpp',
  'srcs/program/main.cpp',
  'srcs/program/parsing/string.cpp',
  'srcs/engine/mesh/VertexPos.cpp',
  'srcs/engine/mesh/VertexVoxel.cpp',
  'srcs/engine/mesh/VertexPosTex.cpp',
  'srcs/engine/mesh/VertexPosNrm.cpp',
  'srcs/engine/mesh/Vertex.cpp',
  'srcs/engine/inputs/Mouse.cpp',
  'srcs/engine/inputs/Key.cpp',
  'srcs/engine/inputs/InputManager.cpp',
  'srcs/engine/textures/TextureManager.cpp',
  'srcs/engine/window/Window.cpp',
  'srcs/engine/vulkan/VulkanUtils.cpp',
  'srcs/engine/vulkan/VulkanContext.cpp',
  'srcs/engine/vulkan/VulkanCommandPool.cpp',
  'srcs/engine/shader/Shader.cpp',
  'srcs/engine/shader/ShaderParam.cpp',
  'srcs/engine/camera/Camera.cpp',
  'srcs/engine/engine.cpp',
]

executable('ft_vox',
          srcs,
          build_rpath: '.',
          dependencies : [
            dependency('glfw3'),
            dependency('libgmath'),
            dependency('vulkan'),
          ],
          include_directories: [
            include_directories('srcs'),
            include_directories('lib'),
          ],
          link_args : ['/usr/lib/x86_64-linux-gnu/libOpenCL.so.1'],
          install : true)

install_subdir('shadersbin', install_dir:'.')
install_subdir('data', install_dir:'.')
install_data('vsupp', install_dir:'.')
