CFLAGS = `pkg-config --cflags opencv4`
LIBS = `pkg-config --libs opencv4`

your-target: main.cpp
    g++ $(CFLAGS) -o your-target main.cpp $(LIBS)
