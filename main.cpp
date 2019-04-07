#include <fstream>
#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

class Matrix {
    int rows;
    int cols;
    double** matrix;

public:
    Matrix(ifstream& source) {
        int col, row;
        if (source.is_open()) {
            source >> row >> col;
            this->rows = row;
            this->cols = col;
            this->matrix = new double*[this->rows];
            for (int i = 0; i < this->rows; i++) {
                int tmp;
                this->matrix[i] = new double[this->cols];

                for (int j = 0; j < this->cols; j++) {
                    source >> tmp;
                    this->matrix[i][j] = tmp;
                }
            }
        } else {
            throw new exception();
        }
    }

    Matrix() {}

    void init(double** matrix, int rows, int cols) {
        this->matrix = matrix;
        this->rows = rows;
        this->cols = cols;
    }

    double** get_matrix() {
        return this->matrix;
    }

    int get_rows() {
        return this->rows;
    }

    int get_cols() {
        return this->cols;
    }

    void print(ofstream& out) {
        out << this->rows << " " << this->cols << endl;
        for (int i = 0; i < this->rows; i++) {
            for(int j = 0; j < this->cols; j++) {
                out << this->matrix[i][j] << " ";
            }
            out << endl;
        }
        out << endl;
    }

    Matrix multiply(Matrix multiplier) {
        double** first = this->get_matrix();
        double** second = multiplier.get_matrix();
        double** result;
        int res_rows = this->get_rows();
        int res_cols = multiplier.get_cols();
        Matrix matrix;

        if (get_cols() != multiplier.get_rows()) {
            throw new exception();
        }

        // allocate memory
        result = new double*[res_rows];
        for (int i = 0; i < res_rows; i++)
        {
            result[i] = new double[res_cols];
            std::fill(result[i], result[i] + res_cols, 0);
        }

        int ij, i, j;

        #pragma omp parallel for shared(first, second, result) private(ij, i, j) schedule(dynamic, 5)
        for (ij = 0; ij < res_rows * res_cols; ij++)
        {
            i = ij / res_cols;
            j = ij % res_rows;
            
            for (int k = 0; k < get_cols(); k++) {
                result[i][j] += first[i][k] * second[k][j];
            }
        }

        matrix.init(result, res_rows, res_cols);
        return matrix;
    }
};

int main() {
    const int MATRIX_MULTIPLY_TICK = 100000;
    ifstream fin("input3.txt");
    ofstream fout("output.txt");
    Matrix m1(fin); 
    Matrix m2(fin);
    Matrix m3;

    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < MATRIX_MULTIPLY_TICK; i++) {
        m3 = m1.multiply(m2);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = stop - start;

    m3.print(fout);
    fin.close();
    fout.close();
    cout << "complete " << diff.count() << endl;
}
