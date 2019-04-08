ctest_header = include_directories('../ctest')

private_headers = include_directories('../src')
dynamic_buffer_object = libxkcd.extract_objects('dynamic_buffer.c')
dynamic_buffer_test = executable('dynamic_buffer_test',
    sources: files('test_dynamic_buffer.c'),
    objects: dynamic_buffer_object,
    include_directories: [ctest_header, private_headers]
)

xkcd_test = executable('xkcd_test',
    sources: files('test_xkcd.c'),
    include_directories: [ctest_header, private_headers, public_headers],
    dependencies: [libcurl, libxml2],
    link_with: libxkcd
)
if not meson.is_subproject()
    test('dynamic buffer', dynamic_buffer_test)
    test('xkcd', xkcd_test)
endif
