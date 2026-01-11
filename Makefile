make:
	g++ -o output/bin/Decoder src/cpp/decoder.cpp src/cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	g++ -o output/bin/Encoder src/cpp/encoder.cpp src/cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`

decoder:
	g++ -g -w -Ilibs/ -o output/bin/Decoder src/cpp/decoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/Decoder

encoder:
	g++ -g -w -Ilibs/ -o output/bin/Encoder src/cpp/encoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/Encoder

alignment:
	g++ -g -o output/bin/Alignment src/cpp/alignment.cpp src/cpp/image_aux.cpp src/cpp/vector_helper.cpp src/cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
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
	g++ -g -w -Ilibs/ -o output/bin/Encoder src/cpp/encoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp `pkg-config --cflags --libs opencv4 zlib`
	g++ -o output/bin/Alignment src/cpp/alignment.cpp src/cpp/image_aux.cpp src/cpp/vector_helper.cpp src/cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	g++ -g -w -Ilibs/ -o output/bin/Decoder src/cpp/decoder.cpp src/cpp/barcode_layout.cpp src/cpp/image_aux.cpp src/cpp/compressor.cpp src/cpp/colours.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/bin/Encoder
	python src/python/distorter.py
	./output/bin/Alignment
	./output/bin/Decoder

demo:
	./output/bin/Encoder
	python src/python/distorter.py
	./output/bin/Alignment
	./output/bin/Decoder
