#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    int size = atoi(argv[1]);

	srand(unsigned(time(0)));
	double** matrix = new double* [size];
	for (int i = 0; i < size; i++) {
		matrix[i] = new double[size + 1];
		matrix[i][i] = 0;
		for (int j = 0; j < size + 1; j++) {
			if (i != j) {
				matrix[i][j] = rand() / 100;
				matrix[i][i] += matrix[i][j];
			}
		}
	}

	std::ofstream matrixOutput;
	std::ofstream approxOutput;
	matrixOutput.open("input.txt");
	approxOutput.open("x0.txt");

	matrixOutput << size << " " << size + 1 << endl;
	approxOutput << size << endl;
	for (int i = 0; i < size; i++) {
		matrixOutput << matrix[i][0];
		approxOutput << rand() / 100 << endl;
		for (int j = 1; j < size + 1; j++) {
			matrixOutput << " " << matrix[i][j];
		}
		matrixOutput << endl;
	}

    return 0;
}