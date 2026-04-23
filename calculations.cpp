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



/**
 * Struktura do mapowania zakresu [min, max] na [-1, 1] i odwrotnie.
 */
struct ChebyshevSpace {
    double a, b;
    double to_range(double x_scaled) const { return 0.5 * (a + b) + 0.5 * (b - a) * x_scaled; }
    double to_scaled(double x_raw) const { return (2.0 * x_raw - (a + b)) / (b - a); }
};

/**
 * Generuje optymalne węzły Czebyszewa dla danego przedziału.
 * To w tych punktach POWINIENEŚ próbkować swoją funkcję jawną f(x).
 */
std::vector<double> generateNodes(int n, double a, double b) {
    std::vector<double> nodes(n);
    ChebyshevSpace space{a, b};
    for (int k = 1; k <= n; ++k) {
        // Zera wielomianu Czebyszewa w przedziale [-1, 1]
        double x_scaled = std::cos((2.0 * k - 1.0) / (2.0 * n) * std::numbers::pi);
        nodes[k - 1] = space.to_range(x_scaled);
    }
    return nodes;
}
std::vector<double> valuesInNodes(std::vector<double> nodes) {
    std::vector<double> values;
    for (auto node: nodes) {
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
    int n = sampled_y.size();
    std::vector<double> coeffs(m + 1, 0.0);

    for (int j = 0; j <= m; ++j) {
        double sum = 0.0;
        for (int k = 1; k <= n; ++k) {
            double x_k = std::cos((2.0 * k - 1.0) / (2.0 * n) * std::numbers::pi);
            sum += sampled_y[k - 1] * std::cos(j * std::acos(x_k));
        }
        coeffs[j] = (2.0 / n) * sum;
    }
    // Pierwszy współczynnik w bazie Czebyszewa jest tradycyjnie dzielony przez 2
    coeffs[0] /= 2.0;
    return coeffs;
}

/**
 * Algorytm Clenshawa - odczyt wartości
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
