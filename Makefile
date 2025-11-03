make:
	g++ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`

decoder:
	g++ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp `pkg-config --cflags --libs opencv4`
	./output/Decoder

encoder:
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp src_cpp/image_aux.cpp `pkg-config --cflags --libs opencv4`
	./output/Encoder

alignment:
	g++ -o output/Alignment src_cpp/alignment.cpp src_cpp/image_aux.cpp src_cpp/vector_helper.cpp `pkg-config --cflags --libs opencv4`
	./output/Alignment
