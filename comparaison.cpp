#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <string>
#include <iomanip>

volatile long long benchmarkResult = 0;
volatile int benchmarkSearchValue = -1;

template<typename Setup, typename Func>
double benchmark(Setup setup, Func func, int repetitions, int operations) {
    std::chrono::nanoseconds total(0);
    for (int i = 0; i < repetitions; ++i) {
        auto container = setup();
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < operations; ++j) {
            func(container);
        }
        auto end = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }
    return std::chrono::duration<double, std::micro>(total).count()
        / (repetitions * operations);
}

void freeAll(std::vector<int>& v, std::list<int>& l) {
    v.clear();
    l.clear();
    v.shrink_to_fit();
    std::list<int>().swap(l);
    std::cout << "Memoire liberee : vector = " << v.size()
              << ", list = " << l.size() << "\n";
}

int main() {
    const int N = 10000;
    const int repetitions = 100;
    const int operations = 1000;
    const int middle = N / 2;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Benchmark N = " << N << "\n";
    std::vector<int> vec(N, 42);
    std::list<int> lst(N, 42);

    std::cout << "\nInsertion au milieu\n";
    const double vectorInsert = benchmark(
        [&]() { return std::vector<int>(N, 42); },
        [&](std::vector<int>& container) {
            container.insert(container.begin() + middle, 99);
        },
        repetitions, operations);
    const double listInsert = benchmark(
        [&]() { return std::list<int>(N, 42); },
        [&](std::list<int>& container) {
            auto it = container.begin();
            std::advance(it, middle);
            container.insert(it, 99);
        },
        repetitions, operations);
    std::cout << "vector : " << vectorInsert << " us\n";
    std::cout << "list   : " << listInsert << " us\n";
    std::cout << "ecart  : " << listInsert - vectorInsert << " us\n";

    std::cout << "\nSuppression au milieu\n";
    const double vectorErase = benchmark(
        [&]() { return std::vector<int>(N, 42); },
        [&](std::vector<int>& container) {
            container.erase(container.begin() + middle);
        },
        repetitions, operations);
    const double listErase = benchmark(
        [&]() { return std::list<int>(N, 42); },
        [&](std::list<int>& container) {
            auto it = container.begin();
            std::advance(it, middle);
            container.erase(it);
        },
        repetitions, operations);
    std::cout << "vector : " << vectorErase << " us\n";
    std::cout << "list   : " << listErase << " us\n";
    std::cout << "ecart  : " << listErase - vectorErase << " us\n";

    std::cout << "\nRecherche\n";
    const double vectorFind = benchmark(
        [&]() { return std::vector<int>(N, 42); },
        [&](std::vector<int>& container) {
            auto it = std::find(container.begin(), container.end(), benchmarkSearchValue);
            benchmarkResult += it != container.end();
        },
        repetitions, operations);
    const double listFind = benchmark(
        [&]() { return std::list<int>(N, 42); },
        [&](std::list<int>& container) {
            auto it = std::find(container.begin(), container.end(), benchmarkSearchValue);
            benchmarkResult += it != container.end();
        },
        repetitions, operations);
    std::cout << "vector : " << vectorFind << " us\n";
    std::cout << "list   : " << listFind << " us\n";
    std::cout << "ecart  : " << listFind - vectorFind << " us\n";

    std::cout << "\nParcours\n";
    const double vectorSum = benchmark(
        [&]() { return std::vector<int>(N, 42); },
        [&](std::vector<int>& container) {
            long long sum = std::accumulate(container.begin(), container.end(), 0LL);
            benchmarkResult += sum;
        },
        repetitions, operations);
    const double listSum = benchmark(
        [&]() { return std::list<int>(N, 42); },
        [&](std::list<int>& container) {
            long long sum = std::accumulate(container.begin(), container.end(), 0LL);
            benchmarkResult += sum;
        },
        repetitions, operations);
    std::cout << "vector : " << vectorSum << " us\n";
    std::cout << "list   : " << listSum << " us\n";
    std::cout << "ecart  : " << listSum - vectorSum << " us\n";

    freeAll(vec, lst);
    return 0;
}