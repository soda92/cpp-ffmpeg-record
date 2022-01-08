cmake_minimum_required(VERSION "3.21")
project(basic-ffmpeg)


# if (MSVC)
#     add_compile_options(/std:c++latest)
#     set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# endif()

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED true)

# To find and use ffmpeg avcodec library
# set(AVCODEC_INCLUDE_DIR "D:/src/vcpkg/packages/ffmpeg_x64-windows-static/include" )
find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

set(srcs
"main2.cpp"
)

set(additional_dependencies
dxva2.lib
evr.lib
mf.lib
mfplat.lib
mfplay.lib
mfreadwrite.lib
mfuuid.lib
bcrypt.lib
ws2_32.lib
Secur32.lib
Strmiids.lib)

# link_directories("D:/src/vcpkg/packages/ffmpeg_x64-windows-static/lib/")
set(ffmpeg_libs avcodec.lib avutil.lib avformat.lib swscale.lib swresample.lib)

add_executable(main ${srcs})

target_include_directories(main PRIVATE ${AVCODEC_INCLUDE_DIR})

if (MSVC)
    set_property(TARGET main APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:MSVCRT")
endif()

target_link_libraries(main PRIVATE ${ffmpeg_libs} ${additional_dependencies})
target_link_libraries(main PRIVATE fmt::fmt)
target_link_libraries(main PRIVATE Boost::boost Boost::filesystem)

add_executable(decoding_video decoding_video.c)
target_link_libraries(decoding_video PRIVATE ${ffmpeg_libs} ${additional_dependencies})
