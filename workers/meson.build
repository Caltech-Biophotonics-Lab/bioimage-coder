boost_fiber_dep = dependency('boost', modules: ['fiber', 'context'])

workers_lib = static_library('workers',
    sources: [
        'src/image_capture_worker.cpp',
        'src/file_write_worker.cpp',
    ],
    cpp_args: [
        # Enable auto-vectorization
        '-march=native',
    ],
    include_directories: [
        'inc',
        messages_inc,
    ],
    dependencies: [
        fmt_dep,
        boost_fiber_dep,
        mock_usb_dep,
        message_router_dep,
    ],
)

workers_dep = declare_dependency(
    link_with: workers_lib,
    include_directories: 'inc',
)

test_coop_frame_capture_exe = executable('test-coop-frame-capture',
    sources: 'tests/test-coop-frame-capture.cpp',
    include_directories: [
        common_inc,
        messages_inc,
    ],
    dependencies: [
        frame_capture_card_dep,
        catch2_dep,
        mock_usb_dep,
        boost_fiber_dep,
        threads_dep,
    ],
)

test('Capture frames 96 cameras from 4x Mock USB cooperatively',
    test_coop_frame_capture_exe,
    args: [
        '-r', 'tap',
    ],
    protocol: 'tap',
)