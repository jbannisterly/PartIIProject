make:
	g++ -o output/bin/Decoder src/cpp/decoder.cpp src/cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	g++ -o output/bin/Encoder src/cpp/encoder.cpp src/cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`

unit:
	g++ -g -w -Ilibs/ -o output/bin/UnitTest src/cpp/unit_test.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp src/cpp/split_error.cpp src/cpp/split_bytes.cpp src/cpp/barcode_writer.cpp src/cpp/colour_palletes.cpp src/cpp/error_layout.cpp src/cpp/header_data.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/UnitTest

e2e:
	g++ -g -w -Ilibs/ -o output/bin/E2E src/cpp/e2e_test.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp src/cpp/split_error.cpp src/cpp/split_bytes.cpp src/cpp/barcode_writer.cpp src/cpp/colour_palletes.cpp src/cpp/error_layout.cpp src/cpp/header_data.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/E2E

decoder:
	g++ -g -w -Ilibs/ -o output/bin/Decoder src/cpp/decoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp src/cpp/split_error.cpp src/cpp/split_bytes.cpp src/cpp/barcode_writer.cpp src/cpp/colour_palletes.cpp src/cpp/error_layout.cpp src/cpp/header_data.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/Decoder

encoder:
	g++ -g -w -Ilibs/ -o output/bin/Encoder src/cpp/encoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp src/cpp/split_error.cpp src/cpp/split_bytes.cpp src/cpp/barcode_writer.cpp src/cpp/colour_palletes.cpp src/cpp/error_layout.cpp src/cpp/header_data.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/Encoder

alignment:
	g++ -g -o output/bin/Alignment src/cpp/alignment.cpp src/cpp/image_aux.cpp src/cpp/vector_helper.cpp src/cpp/profiling.cpp src/cpp/image_debug.cpp src/cpp/finder_patterns.cpp src/cpp/finder_group.cpp src/cpp/image_processing.cpp src/cpp/pattern_valid.cpp src/cpp/bounding_box.cpp `pkg-config --cflags --libs opencv4`
	./output/bin/Alignment

alignment_profile:
	g++ -Wall -pg -o output/bin/Alignment src/cpp/alignment.cpp src/cpp/image_aux.cpp src/cpp/vector_helper.cpp src/cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	./output/bin/Alignment
	gprof ./output/bin/Alignment gmon.out > output/debug/Alignment_Profile
	rm gmon.out

alignment_debug:
	g++ -g -o output/bin/Alignment src/cpp/alignment.cpp src/cpp/image_aux.cpp src/cpp/vector_helper.cpp src/cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	./output/bin/Alignment > output/debug/Alignment_Debug

profiling:
	g++ -Wall -o output/bin/profiling src/cpp/profiling.cpp

error:
	g++ -o output/bin/Error src/cpp/error_correction.cpp
	./output/bin/Error

error2:
	g++ -Ilibs/ -o output/bin/Error2 src/cpp/error_correction_schifra.cpp
	./output/bin/Error2

colours:
	g++ -o output/bin/Colours src/cpp/colours.cpp

full:
	g++ -g -w -Ilibs/ -o output/bin/Encoder src/cpp/encoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp src/cpp/split_error.cpp src/cpp/split_bytes.cpp src/cpp/barcode_writer.cpp src/cpp/colour_palletes.cpp src/cpp/error_layout.cpp src/cpp/header_data.cpp `pkg-config --cflags --libs opencv4 zlib`
	g++ -g -o output/bin/Alignment src/cpp/alignment.cpp src/cpp/image_aux.cpp src/cpp/vector_helper.cpp src/cpp/profiling.cpp src/cpp/image_debug.cpp src/cpp/finder_patterns.cpp src/cpp/finder_group.cpp src/cpp/image_processing.cpp src/cpp/pattern_valid.cpp src/cpp/bounding_box.cpp `pkg-config --cflags --libs opencv4`
	g++ -g -w -Ilibs/ -o output/bin/Decoder src/cpp/decoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp src/cpp/split_error.cpp src/cpp/split_bytes.cpp src/cpp/barcode_writer.cpp src/cpp/colour_palletes.cpp src/cpp/error_layout.cpp src/cpp/header_data.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/Encoder
	python src/python/distorter.py
	./output/bin/Alignment
	./output/bin/Decoder

demo:
	./output/bin/Encoder
	python src/python/distorter.py
	./output/bin/Alignment
	./output/bin/Decoder

c_encoder:
	make -C output/build/ Encoder
	mv ./output/build/src/cpp/Encoder ./output/bin/Encoder
	./output/bin/Encoder

rnd:
	g++ -g -o ./output/bin/Rnd src/cpp/random_data_gen.cpp
	./output/bin/Rnd