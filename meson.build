project('ft_vox', 'cpp',
  version : '1.0',
  default_options : ['warning_level=1', 'cpp_std=c++17'])

# TODO: REMOVE FOR PERF
add_global_arguments('-g', language : 'cpp')

srcs = [
  'srcs/engine/vulkan/VulkanCommandPool.cpp',
  'srcs/engine/vulkan/VulkanContext.cpp',
  'srcs/engine/vulkan/VulkanUtils.cpp',
  'srcs/engine/camera/Camera.cpp',
  'srcs/engine/mesh/Mesh.cpp',
  'srcs/engine/mesh/Vertex.cpp',
  'srcs/engine/window/Window.cpp',
  'srcs/engine/engine.cpp',
  'srcs/engine/shader/Shader.cpp',
  'srcs/engine/textures/TextureManager.cpp',
  'srcs/engine/inputs/InputManager.cpp',
  'srcs/engine/inputs/Key.cpp',
  'srcs/engine/inputs/Mouse.cpp',
  'srcs/program/parsing/string.cpp',
  'srcs/program/main.cpp',
  'srcs/program/loop/init.cpp',
  'srcs/program/loop/computation.cpp',
  'srcs/program/loop/draw.cpp',
  'srcs/program/loop/events.cpp',
  'srcs/program/map/Chunk.cpp',
  'srcs/program/map/Map.cpp',
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
