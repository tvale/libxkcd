# libxkcd
[![Build Status](https://travis-ci.com/tvale/libxkcd.svg?branch=master)](https://travis-ci.com/tvale/libxkcd)
[![codecov](https://codecov.io/gh/tvale/libxkcd/branch/master/graph/badge.svg)](https://codecov.io/gh/tvale/libxkcd)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/tvale/libxkcd.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/tvale/libxkcd/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/tvale/libxkcd.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/tvale/libxkcd/context:cpp)

Library to navigate xkcd comics.

## Dependencies
* `libcurl`
* `libxml2`

## Build
```shell
% meson build
% cd build
% ninja
```

## Test
```shell
% ninja test
