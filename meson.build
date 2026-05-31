project(
  'clife',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1.0',
  default_options : ['warning_level=3', 'c_std=c23'],
)

cc = meson.get_compiler('c')

dependencies = [
  dependency('raylib'),
  cc.find_library('m'),
]

sources = [
  'src/coord_set.c',
  'src/life.c',
  'src/main.c',
  'src/naive_life.c',
  'src/util.c',
]

exe = executable(
  'clife',
  sources,
  include_directories : include_directories('src'),
  dependencies : dependencies,
  install : true,
)
