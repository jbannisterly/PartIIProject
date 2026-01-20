add_library(lib STATIC 
    "barcode_layout.cpp" "image_aux.cpp" "compressor.cpp" "colours.cpp" "split_error.cpp" "split_bytes.cpp" "barcode_writer.cpp" "colour_palletes.cpp" "error_layout.cpp" "header_data.cpp"
)

add_library(alignment_lib STATIC
    "image_aux.cpp" "vector_helper.cpp" "profiling.cpp" "image_debug.cpp" "finder_patterns.cpp" "finder_group.cpp" "image_processing.cpp" "pattern_valid.cpp" "bounding_box.cpp"
)

add_executable(Encoder
    "encoder.cpp"
)

add_executable(Decoder
    "decoder.cpp"
)

add_executable(Alignment
    "alignment.cpp"
)

target_link_libraries(Encoder PRIVATE lib schifra)
target_link_libraries(Decoder PRIVATE lib schifra)
target_link_libraries(Alignment PRIVATE alignment_lib)

target_link_libraries(
    lib PUBLIC
    PkgConfig::PKG_ZLIB
    ${OpenCV_LIBRARIES} PRIVATE
    schifra
)

target_link_libraries(
    alignment_lib PUBLIC ${OpenCV_LIBRARIES}
)