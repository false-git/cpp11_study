run: smart_ptr
	./smart_ptr

smart_ptr: smart_ptr.cc
	$(CXX) -o $@ $<
