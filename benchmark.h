#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

class Benchmark {
private:
    std::vector<double> timings;
    std::string methodName;

public:
    Benchmark(const std::string& name) : methodName(name) {
        timings.reserve(100000);
    }

    void start() {
        timings.push_back(getCurrentTime());
    }

    void stop() {
        timings.back() = getCurrentTime() - timings.back();
    }

    void printResults() {
        if (timings.empty()) {
            //std::cout << "Aucune mesure n'a été effectuée." << std::endl;
            return;
        }

        double minTiming = *std::min_element(timings.begin(), timings.end());
        double maxTiming = *std::max_element(timings.begin(), timings.end());
        double meanTiming = calculateMean();
        double stdDevTiming = calculateStandardDeviation();

        std::cout << "Résultats du benchmark pour la méthode '" << methodName << "' :" << std::endl;
        std::cout << "  Minimum : " << minTiming << " secondes" << std::endl;
        std::cout << "  Maximum : " << maxTiming << " secondes" << std::endl;
        std::cout << "  Moyenne : " << meanTiming << " secondes" << std::endl;
        std::cout << "  Écart-type : " << stdDevTiming << " secondes" << std::endl;
    }

private:
    double getCurrentTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1e6;
    }

    double calculateMean() const {
        double sum = 0.0;
        for (const auto& timing : timings) {
            sum += timing;
        }
        return sum / timings.size();
    }

    double calculateStandardDeviation() const {
        double mean = calculateMean();
        double variance = 0.0;

        for (const auto& timing : timings) {
            variance += pow(timing - mean, 2);
        }

        variance /= timings.size();
        return sqrt(variance);
    }
};
