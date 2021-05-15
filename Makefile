CXX = g++
CXXFLAGS = -std=c++17 -Wall
OBJ = mineutils.o
TEST_TARGETS = mnu_test.exe
MAIN_TARGET =  # Left empty

.PHONY: all
all: $(TEST_TARGETS)

mineutils.o: mineutils.cpp mineutils.h
	$(CXX) $< -o $@ $(CXXFLAGS) -c

mnu_test.exe: mnu_test.cpp mineutils.h $(OBJ)
	$(CXX) $< -o mnu_test.o $(CXXFLAGS) -c
	$(CXX) mnu_test.o $(OBJ) -o $@
