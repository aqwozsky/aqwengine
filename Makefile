CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -MMD -MP
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = build/app

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, build/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

build/%.o: %.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	./$(TARGET)

clean:
	rm -rf build
