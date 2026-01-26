project('fpm-96eyes-instrument-control', 'cpp',
    license: 'BSD-3-Clause',
    license_files: ['LICENSE'],
    default_options: [
        'cpp_std=c++17',
        'buildtype=debugoptimized',
    ],
)

catch2_dep = subproject('catch2').get_variable('catch2_with_main_dep')
threads_dep = dependency('threads')

subdir('common')
subdir('messages')
subdir('bioimage-coder-dsl')

subdir('hardware_drivers')
subdir('message_router')
subdir('workers')
subdir('apps')