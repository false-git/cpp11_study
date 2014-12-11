CXXFLAGS += -std=c++11

run: run-move

clean:
	rm smart_ptr

run-smart_ptr: smart_ptr
	./smart_ptr

smart_ptr: smart_ptr.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

run-move: move
	./move

move: move.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

