CXXFLAGS += -std=c++11

run: run-smart_ptr

clean:
	rm smart_ptr

run-smart_ptr: smart_ptr
	./smart_ptr

smart_ptr: smart_ptr.cc
	$(CXX) $(CXXFLAGS) -o $@ $<
