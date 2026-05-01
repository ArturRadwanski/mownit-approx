//
// Created by valleron on 4/23/26.
//
#pragma once

#include <cmath>
#include <vector>
#include <algorithm>
#include <span>

double givenFunction(const double x) {
    return -2 * x * sin(2*(x-1));
}

// double givenFunction(const double x) {
//     return 30 * x * 11 * exp(-11) - 30*exp(-11.0 * x) + (1/30);
// }



/**
 * Struktura do mapowania zakresu [min, max] na [-1, 1] i odwrotnie.
 */
struct ChebyshevSpace {
    double a, b;
    double to_range(double x_scaled) const { return 0.5 * (a + b) + 0.5 * (b - a) * x_scaled; }
    double to_scaled(double x_raw) const { return (2.0 * x_raw - (a + b)) / (b - a); }
};

/**
 * Generuj węzły Czebyszewa dla danego przedziału
 */
std::vector<double> generateNodes(int n, double a, double b) {
    std::vector<double> nodes(n);
    ChebyshevSpace space{a, b};
    for (int k = 1; k <= n; ++k) {
        double x_scaled = std::cos((2.0 * k - 1.0) / (2.0 * n) * std::numbers::pi);
        nodes[k - 1] = space.to_range(x_scaled);
    }
    return nodes;
}
std::vector<double> generateEvenNodes(int n, double a, double b) {
    std::vector<double> nodes(n);
    const double step = (b - a) / (n - 1);
    for (int i=0;i<n;i++) {
        nodes[i] = a + step*i;
    }
    return nodes;
}

// std::vector<double> generateEvenNodes(const int n, const double a, const double b) {
//     std::vector<double> nodes(n);
//     const double step = (b - a) / (n - 1.0);
//     for (int i=0;i<n;i++) {
//         nodes.push_back(a + i * step);
//     }
//     return nodes;
// }


std::vector<double> valuesInNodes(const std::vector<double> *nodes) {
    std::vector<double> values;
    for (const auto node: *nodes) {
        double y = givenFunction(node);
        values.push_back(y);
    }
    return values;
}
/**
 * Oblicza współczynniki aproksymacji wykorzystując ortogonalność w węzłach.
 * n - liczba węzłów, m - stopień wielomianu (m < n)
 */
std::vector<double> calculateCoefficients(int m, const std::vector<double>& sampled_y) {
    const int n = sampled_y.size();
    std::vector<double> coeffs(m + 1, 0.0);

    for (int j = 0; j <= m; ++j) {
        double sum = 0.0;
        for (int k = 1; k <= n; ++k) {
            double x_k = std::cos((2.0 * k - 1.0) / (2.0 * n) * std::numbers::pi);
            sum += sampled_y[k - 1] * std::cos(j * std::acos(x_k));
        }
        coeffs[j] = (2.0 / n) * sum;
    }
    coeffs[0] /= 2.0;
    return coeffs;
}
std::vector<double> calculateCosineCoefficients(int m, const std::vector<double>& sampled_y, const std::vector<double>& nodes, const double start, const double end) {
    const int n = sampled_y.size();
    std::vector<double> coeffs(m + 1, 0.0);
    const double scaled = 2 * std::numbers::pi / (end - start);
    for (int j = 0; j <= m; ++j) {
        double numerator = 0.0;
        double denominator = 0.0;
        for (int k = 0; k < n - 1; ++k) {
            const double x_k = nodes[k];
            const double cosValue = cos(j * (x_k - start) * scaled);
            numerator+= sampled_y[k] * cosValue;
            denominator+= cosValue * cosValue;
        }
        coeffs[j] = denominator == 0.0 ? 0.0 : numerator / denominator;
    }
    return coeffs;
}

std::vector<double> calculateSineCoefficients(int m, const std::vector<double>& sampled_y, const std::vector<double>& nodes, const double start, const double end) {
    const int n = sampled_y.size();
    std::vector<double> coeffs(m + 1, 0.0);
    const double scaled = 2 * std::numbers::pi / (end - start);
    for (int j = 0; j <= m; ++j) {
        double numerator = 0.0;
        double denominator = 0.0;
        for (int k = 0; k < n - 1; ++k) {
            const double x_k = nodes[k];
            const double sinValue = sin(j * (x_k - start) * scaled);
            numerator+= sampled_y[k] * sinValue;
            denominator+= sinValue * sinValue;
        }
        coeffs[j] = denominator == 0.0 ? 0.0 : numerator / denominator;
    }
    return coeffs;
}

double getTrigValue(const double x, const std::vector<double>& sineCoefficients, const std::vector<double>& cosineCoefficients, const double start, const double end) {
    double result = 0.0;
    const double scaled = 2 * std::numbers::pi / (end - start);
    for (int i=0; i<sineCoefficients.size(); ++i) {
        result += sineCoefficients.at(i) * sin(i*(x-start) * scaled) + cosineCoefficients.at(i) * cos(i*(x - start)*scaled);
    }
    return result;
}
/**
 * odczyt wartości
 */
double getValue(const double x_raw, std::span<const double> coeffs, const double a, const double b) {
    ChebyshevSpace space{a, b};
    double x = space.to_scaled(x_raw);
    double b_k1 = 0.0, b_k2 = 0.0;
    double two_x = 2.0 * x;

    for (int i = coeffs.size() - 1; i >= 1; --i) {
        double b_k = coeffs[i] + two_x * b_k1 - b_k2;
        b_k2 = b_k1;
        b_k1 = b_k;
    }
    return coeffs[0] + x * b_k1 - b_k2;
}
