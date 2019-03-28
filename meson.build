project('xkcd', 'c',
    version: '1.0',
    default_options: [
        'c_std=c11',
        'warning_level=3',
        'werror=true'
    ]
)

libcurl = dependency('libcurl')
libxml2 = dependency('LibXml2')

public_headers = include_directories('include')

subdir('src')
subdir('tests')

libxkcd_dep = declare_dependency(
    include_directories: public_headers,
    link_with: libxkcd
)
