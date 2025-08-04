#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <iomanip>
#include <nlohmann/json.hpp>
#include "IUnitConverter.h"
#include "MetricConverter.h"
#include "ImperialConverter.h"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Használat: " << argv[0] << " <input.json> <output.json> [--metric | --imperial]" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    std::string mode = argv[3];

    std::unique_ptr<IUnitConverter> converter;
    if (mode == "--metric") {
        converter = std::make_unique<MetricConverter>();
    } else if (mode == "--imperial") {
        converter = std::make_unique<ImperialConverter>();
    } else {
        std::cerr << "Hibás mértékegység! Használat: --metric vagy --imperial" << std::endl;
        return 2;
    }

    std::ifstream inputFile(inputPath);
    if (!inputFile) {
        std::cerr << "Nem sikerült megnyitni az input fájlt: " << inputPath << std::endl;
        return 3;
    }

    json inputJson;
    try {
        inputFile >> inputJson;
    } catch (...) {
        std::cerr << "Hibás JSON fájlformátum!" << std::endl;
        return 4;
    }

    const auto& points = inputJson["input"];
    json output;
    for (size_t i = 1; i < points.size(); ++i) {
        GPSPoint from{points[i - 1]["GPSP"]["lat"], points[i - 1]["GPSP"]["lon"]};
        GPSPoint to{points[i]["GPSP"]["lat"], points[i]["GPSP"]["lon"]};

        double dist = converter->calculateDistance(from, to);
        double bearing = converter->calculateBearing(from, to);

        json entry;
        entry["fromGPSP"] = {
            {"lat", std::round(from.lat * 100000) / 100000},
            {"lon", std::round(from.lon * 100000) / 100000}
        };
        entry["distance"] = static_cast<int>(std::round(dist));

        // Radiánnál lebegőpontos, foknál egész
        if (mode == "--metric") {
            entry["bearing"] = static_cast<int>(std::round(bearing));
        } else {
            entry["bearing"] = std::round(bearing * 100.0) / 100.0; // 2 tizedes jegy
        }

        output["output"].push_back(entry);
    }

    std::ofstream outFile(outputPath);
    outFile << std::setw(2) << output << std::endl;
    std::cout << "Sikeresen kiírva: " << outputPath << std::endl;
    return 0;
}
