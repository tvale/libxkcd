libxkcd = static_library(meson.project_name(),
    sources: files('dynamic_buffer.c', 'optional.c', 'xkcd.c'),
    include_directories: public_headers,
    dependencies: [libcurl, libxml2],
)
