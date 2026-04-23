//
// Created by valleron on 4/23/26.
//


#include <fstream>
#include <iomanip>
#include <string>
#include "calculations.cpp"

struct ApproximationResult {
    double maxError;
    double stdDev;
};

/**
 * Oblicza błędy aproksymacji na gęstej siatce punktów.
 */
ApproximationResult analyzeApproximationError(
    int numTestPoints,
    double a,
    double b,
    const std::vector<double>& coefficients)
{
    std::vector<double> errors;
    errors.reserve(numTestPoints);

    double maxErr = 0.0;
    double sumSquaredErr = 0.0;

    double step = (b - a) / (numTestPoints - 1);

    for (int i = 0; i < numTestPoints; ++i) {
        double x = a + i * step;

        double actual = givenFunction(x);
        double approx = getValue(x, coefficients, a, b);

        double error = std::abs(actual - approx);
        errors.push_back(error);

        // Błąd maksymalny
        if (error > maxErr) maxErr = error;

        // Suma kwadratów do odchylenia (średnia błędów tutaj to zazwyczaj ~0)
        sumSquaredErr += error * error;
    }

    // Odchylenie standardowe błędów
    double endSquaredErr = std::sqrt(sumSquaredErr) / numTestPoints;

    return {maxErr, endSquaredErr};
}



/**
 * Generuje tabele LaTeX z analizą błędów dla różnych kombinacji n i m.
 * Plik wyjściowy zawiera dwie osobne tabele: dla błędu max i odchylenia std.
 */
void generateErrorDataForHeatmap(const std::string& filename, int max_n, int max_m,
                                 double a, double b) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    // Nagłówek CSV: n, m0, m1, m2...
    file << "n";
    for (int j = 0; j <= max_m; ++j) file << ",m" << j;
    file << "\n";

    for (int n = 2; n <= max_n; ++n) {
        file << n;
        auto nodes_x = generateNodes(n, a, b);
        std::vector<double> nodes_y;
        for (double x : nodes_x) nodes_y.push_back(givenFunction(x));

        for (int m = 0; m <= max_m; ++m) {
            file << ",";
            if (m >= n) {
                file << "NaN"; // pgfplotstable zignoruje brakujące dane
            } else {
                auto coeffs = calculateCoefficients(m, nodes_y);
                auto stats = analyzeApproximationError(10000, a, b, coeffs);
                file << stats.maxError; // Zapisujemy czystą liczbę
            }
        }
        file << "\n";
    }
    file.close();
}

int main(int argc, char *argv[]) {
    std::string filename;
}