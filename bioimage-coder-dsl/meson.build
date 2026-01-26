bioimage_coder_dsl_dep = declare_dependency(
    include_directories: [
        'inc',
        messages_inc,
        common_inc,
    ],
)

bioimage_coder_export_plantuml_dep = declare_dependency(
    sources: 'src/export-plantuml.cpp',
    compile_args: [
        '-Wno-unused-function',
    ],
    include_directories: [
        'inc',
        common_inc,
    ],
)