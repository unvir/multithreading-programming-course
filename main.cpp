#include <math.h>
#include <iostream>
#include <fstream>
#include "mpi.h"
#include <sstream>
#include <ctime>

using namespace std;

int main(int argc, char** argv) {

	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	double* data;
	double** matrix;
	double** scatteredMatrix;
	double* freeTerm;
	double* scatteredFree;
	double* approx;
	double* tempApprox;
	double* norms;
	int* lengths;
	int length;
	int m, n; 
	double eps = 0.001;

	std::ifstream matrixInput;
	std::ifstream approxInput;
	std::ofstream solutionOutput;

	if (rank == 0) {
		matrixInput.open("input.txt");
		approxInput.open("x0.txt");

		matrixInput >> m >> n;
		approxInput >> m;

		if (m % size == 0) n = m;
		else if (m > size) n = m + size - (m % size);
		else               n = size;
	}

	MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	data = (double*)malloc(n * m * sizeof(double));
	matrix = (double**)malloc(n * sizeof(double*));
	for (int i = 0; i < n; i++) matrix[i] = &(data[m * i]);
	freeTerm = (double*)malloc(n * sizeof(double));
	approx = (double*)malloc(n * sizeof(double));

	if (rank == 0) {
		// read matrix, free and approx from files
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < m; j++) {
				matrixInput >> matrix[i][j];
			}
			matrixInput >> freeTerm[i];
		}

		for (int i = 0; i < m; i++) {
			approxInput >> approx[i];
		}

		matrixInput.close();
		approxInput.close();
	}

	// temp approx init
	tempApprox = (double*)malloc((n / size) * sizeof(double));
	// scattered matrix, free init
	data = (double*)malloc(m * (n / size) * sizeof(double));
	scatteredMatrix = (double**)malloc((n / size) * sizeof(double*));
	for (int i = 0; i < n / size; i++) scatteredMatrix[i] = &(data[m * i]);
	scatteredFree = (double*)malloc(n * sizeof(double));
	// norms init
	norms = (double*)malloc(size * sizeof(double));
	// sizes init
	lengths = (int*)malloc(size * sizeof(int));
	for (int i = 0; i < size; i++) {
		lengths[i] = n / size;
	}
	// last is n/size - (n-m)
	lengths[size - 1] += m - n;

	// start itme
	double startwtime = MPI_Wtime();

	// initial scatter and broadcast
	MPI_Scatter(lengths, 1, MPI_INT, &length, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(*matrix, (n / size) * m, MPI_DOUBLE, *scatteredMatrix, (n / size) * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(freeTerm, n / size, MPI_DOUBLE, scatteredFree, n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(approx, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(norms, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// local norm
	double norm;

	// jacobi run
	do {
		for (int i = 0; i < length; i++) {
			tempApprox[i] = scatteredFree[i];
			for (int j = 0; j < m; j++) {
				if (i + (n / size) * rank != j) {
					tempApprox[i] -= scatteredMatrix[i][j] * approx[j];
				}
			}
			tempApprox[i] /= scatteredMatrix[i][i + (n / size) * rank];
		}
		norm = fabs(approx[rank * (n / size)] - tempApprox[0]);
		for (int i = 1; i < length; i++) {
			if (fabs(approx[rank * (n / size) + i] - tempApprox[i]) > norm)
				norm = fabs(approx[rank * (n / size) + i] - tempApprox[i]);
			approx[rank * (n / size) + i] = tempApprox[i];
		}

		// midterm gather and broadcast
		MPI_Gather(tempApprox, n / size, MPI_DOUBLE, approx, n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Gather(&norm, 1, MPI_DOUBLE, norms, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(approx, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(norms, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		// getting max norm
		norm = norms[0];
		for (int i = 0; i < size; i++) {
			if (norms[i] > norm) {
				norm = norms[i];
			}
		}
	} while (norm > eps);
	
	delete[] tempApprox;
	
	double endwtime = MPI_Wtime();

	MPI_Finalize();

	if (rank == 0) {
		cout << endwtime - startwtime << endl;

		solutionOutput.open("output.txt");
		for (int i = 0; i < m; i++) {
			solutionOutput << approx[i] << endl;
		}
	}

	delete[] data;
	delete[] matrix;
	delete[] scatteredMatrix;
	delete[] freeTerm;
	delete[] scatteredFree;
	delete[] approx;
	delete[] tempApprox;
	delete[] norms;
	delete[] lengths;

	return 0;
}