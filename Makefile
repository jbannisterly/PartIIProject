make:
	g++ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`

decoder:
	g++ -Wall -Ilibs/ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp src_cpp/compressor.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/Decoder

encoder:
	g++ -Ilibs/ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp src_cpp/compressor.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/Encoder

alignment:
	g++ -Wall -g -o output/Alignment src_cpp/alignment.cpp src_cpp/image_aux.cpp src_cpp/vector_helper.cpp src_cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	./output/Alignment

alignment_profile:
	g++ -Wall -pg -o output/Alignment src_cpp/alignment.cpp src_cpp/image_aux.cpp src_cpp/vector_helper.cpp src_cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	./output/Alignment
	gprof ./output/Alignment gmon.out > output/Alignment_Profile
	rm gmon.out

profiling:
	g++ -Wall -o output/profiling src_cpp/profiling.cpp

error:
	g++ -o output/Error src_cpp/error_correction.cpp
	./output/Error

error2:
	g++ -Ilibs/ -o output/Error2 src_cpp/error_correction_schifra.cpp
	./output/Error2

full:
	g++ -Ilibs/ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp src_cpp/compressor.cpp `pkg-config --cflags --libs opencv4 zlib`
	g++ -o output/Alignment src_cpp/alignment.cpp src_cpp/image_aux.cpp src_cpp/vector_helper.cpp src_cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	g++ -Ilibs/ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp src_cpp/compressor.cpp `pkg-config --cflags --libs opencv4 zlib`
	./output/Encoder
	python src/distorter.py
	./output/Alignment
	./output/Decoder

demo:
	./output/Encoder
	python src/distorter.py
	./output/Alignment
	./output/Decoder
