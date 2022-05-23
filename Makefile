all:
	g++ -o main main.cpp Shape.cpp objImporter.cpp bmpLib.cpp Scene.cpp utility.cpp -lm -ldl -lglut -lGL -lGLU -fopenmp

run_zero:
	./main 0

run_one:
	./main 1

clean:
	rm main