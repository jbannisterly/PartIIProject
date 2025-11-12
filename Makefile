make:
	g++ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`

decoder:
	g++ -Wall -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp src_cpp/compressor.cpp `pkg-config --cflags --libs opencv4`
	./output/Decoder

encoder:
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp src_cpp/compressor.cpp `pkg-config --cflags --libs opencv4`
	./output/Encoder

alignment:
	g++ -Wall -o output/Alignment src_cpp/alignment.cpp src_cpp/image_aux.cpp src_cpp/vector_helper.cpp src_cpp/profiling.cpp `pkg-config --cflags --libs opencv4`
	./output/Alignment

profiling:
	g++ -Wall -o output/profiling src_cpp/profiling.cpp

demo:
	./output/Encoder
	python src/distorter.py
	./output/Alignment
	./output/Decoder
