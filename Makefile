make:
	g++ -o output/Decoder src_cpp/decoder.cpp `pkg-config --cflags --libs opencv4`

decoder:
	g++ -o output/Decoder src_cpp/decoder.cpp `pkg-config --cflags --libs opencv4`
	./output/Decoder