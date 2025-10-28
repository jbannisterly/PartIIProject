make:
	g++ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`

decoder:
	g++ -o output/Decoder src_cpp/decoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	./output/Decoder

encoder:
	g++ -o output/Encoder src_cpp/encoder.cpp src_cpp/barcode_layout.cpp `pkg-config --cflags --libs opencv4`
	./output/Encoder