CXXFLAGS += -std=c++11

run: run-thread

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

run-thread: thread
	./thread

thread: thread.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

