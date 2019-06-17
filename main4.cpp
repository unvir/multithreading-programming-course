#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>
#include <sstream>
#include <vector>

#define INF 2147483647

using namespace std;

int** init(ifstream &inputFile, int length) {
    int** matrix = new int* [length];
    for (int i = 0; i < length; i++) {
        matrix[i] = new int[length];
        for (int j = 0; j < length; j++) {
            inputFile >> matrix[i][j];
        }
    }
    return matrix;
}

long* dijkstra_omp(int** matrix, int length, int start) {
    bool* visited = new bool[length];
    long* distances = new long[length];
    long* mins = new long[omp_get_max_threads()];
    int* minNumbers = new int[omp_get_max_threads()];
    long min = INF;


    #pragma omp parallel for shared(distances, visited) schedule(dynamic)
    for (int i = 0; i < length; i++) {
        distances[i] = INF;
        visited[i] = false;
    }

    distances[start] = 0;
    visited[start] = true;
    int cur = start;
    int j;

    for (int i = 0; i < length - 1; i++) {
        for (j = 0; j < omp_get_max_threads(); j++) {
            mins[j] = INF;
            minNumbers[j] = -1;
        }
        #pragma omp parallel for shared(matrix, distances, visited, mins, minNumbers) firstprivate(length) private(cur, j) schedule(dynamic)
        for (j = 0; j < length; j++) {
            cur = minNumbers[omp_get_thread_num()];
            if (!visited[j] && (cur == -1 || distances[j] < distances[cur])) {
                mins[omp_get_thread_num()] = distances[j];
                minNumbers[omp_get_thread_num()] = j;
            }
        }

        min = INF;
        for (j = 0; j < omp_get_max_threads(); j++) {
            if (mins[j] < min) {
                min = mins[j];
                cur = minNumbers[j];
            }
        }

        if (distances[cur] == INF)
            break;
        visited[cur] = true;

        #pragma omp parallel for firstprivate(cur, length) private(j) shared(matrix, distances) schedule(dynamic)
        for(j = 0; j < length; j++) {
            if (distances[cur] + matrix[cur][j] < distances[j]) {
                distances[j] = distances[cur] + matrix[cur][j];
            }
        }
    }

    delete[] visited;
    return distances;
}

int main() {

    std::ifstream inputFile;
    std::ofstream outputFile;

    int length;
    int** matrix;
    vector<vector<int>> vec;

    const int MAX_THREADS = 8;
    const int LAUNCH_TIMES = 3;

    outputFile.open("output.txt");
    inputFile.open("input.txt");
    inputFile >> length;
    matrix = init(inputFile, length);
    inputFile.close();

    omp_set_dynamic(1);

    for (int j = 0; j < LAUNCH_TIMES; j++) {

        for (int k = 0; k < MAX_THREADS; k++) {
            omp_set_num_threads(k + 1);

            auto start = chrono::high_resolution_clock::now();
            long *result = dijkstra_omp(matrix, length, 0);
            auto stop = chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(stop - start);
            outputFile << "Length: " << length << ". Threads: " << k + 1 << ". Time: " << duration.count() << std::endl;

            delete[] result;
        }

        outputFile << endl;
    }

    delete[] matrix;
    outputFile.close();

    return 0;
}