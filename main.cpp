#include <math.h>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include <climits>
#include <sstream>
#include <ctime>
#include <chrono>

using namespace std;

int getPivot(int* array, int start, int end) {
		int length = end - start - 1;
		switch (length) {
			case -1:
			case 0:
				return INT_MIN;
				break;
			case 1:
			case 2:
				return array[start];
			default:
				return (array[start] + array[start + length / 2] + array[end - 1]) / 3;
		}
}

int compare(const void* a, const void* b) {
	const int* x = (int*) a;
	const int* y = (int*) b;

	if (*x > * y) 	  return 1;
	else if (*x < *y) return -1;
	else              return 0;
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int* array;
	int* subArray;
	int* sizes;
	int* groupSizes = NULL;
	int* offsets;
	int* groupOffsets = NULL;
	int* pivots;
	int* groupPivots = NULL;
	int length;
	int pivot = 0;

	std::ifstream arrayInput;
	std::ofstream solutionOutput;

	if (rank == 0) {
		arrayInput.open("input.txt");
		arrayInput >> length;
	}

	MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);

	array = new int[length];

	subArray = new int[length];

	if (rank == 0) {
		for (int i = 0; i < length; i++) {
			arrayInput >> array[i];
		}
		arrayInput.close();
	}

	sizes = new int[size];
	for (int i = 0; i < size; i++) {
		sizes[i] = length / size;
	}
	for (int i = 0; i < length % size; i++) {
		sizes[i]++;
	}

	offsets = new int[size];

	int iterationsCount = (int) (log(size) / log(2));

	pivots = new int[size];

	auto start = chrono::high_resolution_clock::now();

	if (size == 1) {
		qsort(array, length, sizeof(int), compare);
	}

	for (int iteration = 0; iteration < iterationsCount; iteration++) {
		
		offsets[0] = 0;
		for (int i = 1; i < size; i++) {
			offsets[i] = offsets[i - 1] + sizes[i - 1];
		}

		int mySize;
		int addSize;
		int sendSize;

		MPI_Scatter(sizes, 1, MPI_INT, &mySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scatterv(array, sizes, offsets, MPI_INT, subArray, mySize, MPI_INT, 0, MPI_COMM_WORLD);

		int color = rank / ((int)pow(2, iterationsCount - iteration));
		MPI_Comm LOCAL_MPI_COMM_WORLD;
		MPI_Comm_split(MPI_COMM_WORLD, color, rank, &LOCAL_MPI_COMM_WORLD);
		int localRank, localSize;
		MPI_Comm_rank(LOCAL_MPI_COMM_WORLD, &localRank);
		MPI_Comm_size(LOCAL_MPI_COMM_WORLD, &localSize);

		if (localRank == 0) {
			pivot = getPivot(subArray, 0, mySize);
		}

		MPI_Bcast(&pivot, 1, MPI_INT, 0, LOCAL_MPI_COMM_WORLD);

		int checkBit = 1 << ((int)(log(size) / log(2)) - (iteration + 1));

		if (! (rank & checkBit)) {
			MPI_Recv(&addSize, 1, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&(subArray[mySize]), addSize, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			int partLength = mySize + addSize;

			int less = 0;
			int greater = partLength - 1;

			while (less <= greater) {
				if (subArray[less] >= pivot) {
					int buf = subArray[less];
					subArray[less] = subArray[greater];
					subArray[greater] = buf;
					greater--;
				} else {
					less++;
				}
			}

			sendSize = partLength - less;
			MPI_Send(&sendSize, 1, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD);
			MPI_Send(&(subArray[less]), sendSize, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD);

			sendSize = less;
		} else {
			MPI_Send(&mySize, 1, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD);
			MPI_Send(subArray, mySize, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD);
			MPI_Recv(&sendSize, 1, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(subArray, sendSize, MPI_INT, rank ^ checkBit, 1000, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// Last one 
		if (iteration == iterationsCount - 1) {
			qsort(subArray, sendSize, sizeof(int), compare);
		}

		MPI_Gather(&sendSize, 1, MPI_INT, sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);

		offsets[0] = 0;
		for (int i = 1; i < size; i++) {
			offsets[i] = offsets[i - 1] + sizes[i - 1];
		}

		MPI_Gatherv(subArray, sendSize, MPI_INT, array, sizes, offsets, MPI_INT, 0, MPI_COMM_WORLD);
	}

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	if (rank == 0) {
		cout << duration.count() << endl;
		solutionOutput.open("output.txt");
		for (int i = 0; i < length; i++) {
			solutionOutput << array[i] << " ";
		}
		solutionOutput.close();
	}

	if (rank == 0) {
		delete[] groupSizes;
		delete[] groupOffsets;
		delete[] groupPivots;
	}

	delete[] array;
	delete[] sizes;
	delete[] offsets;
	delete[] pivots;
	delete[] subArray;

	MPI_Finalize();
}