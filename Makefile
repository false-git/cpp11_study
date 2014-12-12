CXXFLAGS += -std=c++11

run: run-lambda

clean:
	rm -f smart_ptr move thread lambda

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

run-lambda: lambda
	./lambda

lambda: lambda.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

