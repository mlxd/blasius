#include "cblas_prefix.hpp"

#include <complex>
#include <vector>

int main(int argc, char* argv[]) {
    // benchmark sizes
    const auto minSize = 2u;
    const auto maxSize = 2048u;
    const auto maxRepetitions = 2048u;

    // do measurements with increasing matrix dimensions and decreasing
    // repetitions to keep wall clock time short
    std::size_t repetitions{};
    std::size_t size{};
    for (size = minSize, repetitions = maxRepetitions; size <= maxSize; size *= 2, repetitions /= 2) {

        /// set up data
        const auto aRows = size;
        const auto aCols = size;
        const auto bRows = size;
        const auto bCols = size;

        std::vector<double> a(aRows * aCols); // m * k
        std::vector<double> b(bRows * bCols); // k * n
        std::vector<double> c(aRows * bCols); // m * n

        fill_random(std::begin(a), std::end(a));
        fill_random(std::begin(b), std::end(b));

        // create reference solution matrix using serialIkj
        std::vector<Real> reference(c.size());
        serialIkj(a.data(), b.data(), reference.data(), aRows, aCols, bCols);

        /// warm up the caches
        gemm(a.data(), b.data(), c.data(), aRows, aCols, bCols);

        /// timed computations
        auto time = 0.0;
        for (std::size_t r{0}; r < repetitions; ++r) {
            std::fill(std::begin(c), std::end(c), 0);
            time += timed([&]() { gemm(a.data(), b.data(), c.data(), aRows, aCols, bCols); });

            // validate the result c
            if (!std::equal(std::begin(c), std::end(c), std::begin(reference), std::end(reference),
                            [](const Real& l, const Real& r) { return std::abs(l - r) < 1.0e-3; })) {
                std::cerr << "validation error!\n";
                return EXIT_FAILURE;
            }
        }
        time /= repetitions; // get avg time per call

        std::cout << size << ";" << time << '\n';
    }
}
