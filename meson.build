project('rid3', 'c')

src = files('id3.c')
inc = include_directories('.')

rid3_lib = library(meson.project_name(), src, include_directories: inc)
rid3_dep = declare_dependency(link_with: rid3_lib, include_directories: inc)
