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

    double maxErr = 0.0;
    double sumSquaredErr = 0.0;

    double step = (b - a) / (numTestPoints - 1);

    for (int i = 0; i < numTestPoints; ++i) {
        double x = a + i * step;

        double actual = givenFunction(x);
        double approx = getValue(x, coefficients, a, b);

        double error = std::abs(actual - approx);

        // Błąd maksymalny
        if (error > maxErr) maxErr = error;

        // Suma kwadratów do odchylenia (średnia błędów tutaj to zazwyczaj ~0)
        sumSquaredErr += error * error;
    }

    double endSquaredErr = std::sqrt(sumSquaredErr) / numTestPoints;

    return {maxErr, endSquaredErr};
}

ApproximationResult analyzeTrigApproximationError(
    int numTestPoints,
    double a,
    double b,
    const std::vector<double>& sineCoefficients,
    const std::vector<double>& cosineCoefficients)
{

    double maxErr = 0.0;
    double sumSquaredErr = 0.0;

    double step = (b - a) / (numTestPoints - 1);

    for (int i = 0; i < numTestPoints; ++i) {
        double x = a + i * step;

        double actual = givenFunction(x);
        double approx = getTrigValue(x, sineCoefficients, cosineCoefficients, a, b);

        double error = std::abs(actual - approx);

        // Błąd maksymalny
        if (error > maxErr) maxErr = error;

        // Suma kwadratów do odchylenia (średnia błędów tutaj to zazwyczaj ~0)
        sumSquaredErr += error * error;
    }

    double endSquaredErr = std::sqrt(sumSquaredErr) / numTestPoints;

    return {maxErr, endSquaredErr};
}

/**
 * Generuje tabele LaTeX z analizą błędów dla różnych kombinacji n i m.
 * Plik wyjściowy zawiera dwie osobne tabele: dla błędu max i odchylenia std.
 */
void generateHardcodedLatexHeatmap(const std::string& filename, int max_n, int max_m,
                                     double a, double b) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "\\begin{table}[h]\n\\centering\n\\scriptsize\n";
    file << "\\setlength{\\tabcolsep}{2pt}\n"; // ciasne kolumny
    file << "\\begin{tabular}{|l|" << std::string(max_m + 1, 'c') << "|}\n\\hline\n";

    // Nagłówek
    file << "n \\textbackslash m";
    for (int m = 0; m <= max_m; m+=20) file << " & " << m;
    file << " \\\\\\hline\n";

    for (int n = 5; n <= max_n; n+=20) {
        file << n;
        auto nodes_x = generateNodes(n, a, b);
        std::vector<double> nodes_y;
        for (double x : nodes_x) nodes_y.push_back(givenFunction(x));

        for (int m = 0; m <= max_m && m < n; m+=20) {
            file << " & ";
                auto coeffs = calculateCoefficients(m, nodes_y);
                auto stats = analyzeApproximationError(10000, a, b, coeffs);


            double minErr = 0.0;  // Najlepszy możliwy błąd
            // TODO zautomatyzować
            double maxErr = 25; // Najgorszy błąd (dostosować do danych)

            int intensity = static_cast<int>(std::max(0.0, std::min(100.0,
                            ((stats.maxError - minErr) / (maxErr - minErr)) * 100.0 )));

                file << "\\cellcolor{red!" << intensity << "}";
                // Formatowanie liczby do zapisu naukowego
                file << std::scientific << std::setprecision(3) << stats.maxError;

        }
        file << " \\\\\n";
    }

    file << "\\hline\n\\end{tabular}\n";
    file << "\\caption{Mapa błędu maksymalnego (intensywniejszy czerwony = większy błąd)}\n";
    file << "\\end{table}\n";
    file.close();
}


void generateTrigLatexHeatmap(const int min_n, const int max_n, const int step_n, const int min_m,
                                     const int max_m, const int step_m,
                                     const double a, const double b, const std::string& filename,
                                     const std::string& label, const std::string& caption,
                                     const std::string& color, const bool max_err) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "\\begin{table}[h]\n\\centering\n\\scriptsize\n";
    file << "\\setlength{\\tabcolsep}{2pt}\n"; // ciasne kolumny
    file << "\\begin{tabular}{|l|" << std::string(max_m + 1, 'c') << "||}\n\\hline\n";

    // Nagłówek
    file << "n \\textbackslash m";
    for (int m = min_m; m <= max_m; m+=step_m) file << " & " << m;
    file << " \\\\\\hline\n";
    double maxErr = 0.0;
    double minErr = INFINITY;
    for (int n = min_n; n <= max_n; n+=step_n) {
        auto nodes_x = generateEvenNodes(n, a, b);
        std::vector<double> nodes_y;
        for (double x : nodes_x) nodes_y.push_back(givenFunction(x));
        for (int m=min_m; m <= max_m && m < n / 2.0; m+=step_m) {
            const std::vector<double> sineCoefficients = calculateSineCoefficients(m,
                nodes_y, nodes_x, a, b);
            const std::vector<double> cosineCoefficients = calculateCosineCoefficients(m,
                nodes_y, nodes_x, a, b);
            ApproximationResult res = analyzeTrigApproximationError(1000, a ,b,
                sineCoefficients, cosineCoefficients);
            maxErr = std::max(maxErr, max_err ? res.maxError : res.stdDev);
            minErr = std::min(minErr, max_err ? res.maxError : res.stdDev);
        }
    }
    printf("%f\n", maxErr);
    printf("%f\n", minErr);
    for (int n = min_n; n <= max_n; n+=step_n) {
        file << n;
        auto nodes_x = generateEvenNodes(n, a, b);
        std::vector<double> nodes_y;
        for (double x : nodes_x) nodes_y.push_back(givenFunction(x));

        for (int m = min_m; m <= max_m && m < n / 2.0; m+=step_m) {
            file << " & ";
            auto coeffsSine = calculateSineCoefficients(m, nodes_y, nodes_x, a, b);
            auto coeffsCosine = calculateCosineCoefficients(m, nodes_y, nodes_x, a, b);
            auto stats = analyzeTrigApproximationError(1000, a, b,coeffsSine, coeffsCosine);

            //double minErr = 0.0;  // Najlepszy możliwy błąd

            int intensity = static_cast<int>(std::max(0.0, std::min(100.0,
                            (((max_err ? stats.maxError : stats.stdDev) - minErr) / (maxErr - minErr)) * 100.0 )));

            file << "\\cellcolor{" <<  color << "!" << intensity << "}";
            // Formatowanie liczby do zapisu naukowego
            file << std::scientific << std::setprecision(3) << (max_err ? stats.maxError : stats.stdDev);
        }
        file << " \\\\\n";
    }

    file << "\\hline\n\\end{tabular}\n";
    file << "\\caption{" << caption << ". Mapa błędu maksymalnego (intensywniejszy kolor = większy błąd)}\n";
    file << "\\label{tab:" << label <<"}\n";
    file << "\\end{table}\n";
    file.close();
}

int main(int argc, char *argv[]) {
    double start = -2 * M_PI + 1;
    double end = 3 * M_PI +1;
    generateTrigLatexHeatmap(10, 100, 10, 2, 13, 1, start, end, "table_small_values_max.tex", "small_values_max",
        "Tabela przedstawia wartości błędu maksymalnego dla aproksymacji używających niskich częstotliwości",
        "Red", true);
    generateTrigLatexHeatmap(10, 100, 10, 2, 13, 1, start, end, "table_small_values_min.tex", "small_values_square",
        "Tabela przedstawia wartości błędu MSE dla aproksymacji używających niskich częstotliwości",
        "Orange", false);
}