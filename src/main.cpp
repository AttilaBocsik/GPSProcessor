#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "IUnitConverter.h"
#include "MetricConverter.h"
#include "ImperialConverter.h"

// using json = nlohmann::json;

// GPS pont kiolvasásához egyszerű parser
double extractValue(const std::string& obj, const std::string& key) {
    size_t pos = obj.find(key);
    if (pos == std::string::npos) return 0.0;
    pos = obj.find(":", pos);
    size_t start = obj.find_first_of("0123456789.-", pos);
    size_t end = obj.find_first_not_of("0123456789.-", start);
    return std::stod(obj.substr(start, end - start));
}

// Egy teljes JSON objektum kiolvasása az inputból
std::string readNextObject(std::ifstream& in) {
    std::string obj;
    char c;
    int braceCount = 0;
    bool started = false;

    while (in.get(c)) {
        if (c == '{') {
            started = true;
            braceCount++;
        }
        if (started) {
            obj.push_back(c);
        }
        if (c == '}') {
            braceCount--;
            if (braceCount == 0) break; // Egy teljes objektumot kiolvastunk
        }
    }
    return obj;
}

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

    std::ifstream inFile(inputPath);
    if (!inFile) {
        std::cerr << "Nem sikerült megnyitni az input fájlt: " << inputPath << std::endl;
        return 3;
    }
    /*
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
    */
    // Innen a módosítás
    std::ofstream outFile(outputPath);
    if (!outFile) {
        std::cerr << "Nem sikerült megnyitni az output fájlt: " << outputPath << std::endl;
        return 4;
    }

    // Átugorjuk a fájl elejét a "input": [ részig
    std::string dummy;
    std::getline(inFile, dummy, '[');

    // Első objektum
    std::string prevObj = readNextObject(inFile);
    if (prevObj.empty()) {
        std::cerr << "Nincs feldolgozható pont az input fájlban!" << std::endl;
        return 5;
    }

    outFile << "{\n  \"output\": [\n";
    bool first = true;

    while (true) {
        std::string nextObj = readNextObject(inFile);
        if (nextObj.empty()) break;

        double lat1 = extractValue(prevObj, "lat");
        double lon1 = extractValue(prevObj, "lon");
        double lat2 = extractValue(nextObj, "lat");
        double lon2 = extractValue(nextObj, "lon");

        GPSPoint from{lat1, lon1};
        GPSPoint to{lat2, lon2};

        double dist = converter->calculateDistance(from, to);
        double bearing = converter->calculateBearing(from, to);

        if (!first) outFile << ",\n";
        first = false;

        outFile << "    {\n"
                << "      \"fromGPSP\": {\"lat\": " << std::fixed << std::setprecision(5) << lat1
                << ", \"lon\": " << lon1 << "},\n"
                << "      \"distance\": " << static_cast<int>(std::round(dist)) << ",\n";

        if (mode == "--metric") {
            outFile << "      \"bearing\": " << static_cast<int>(std::round(bearing)) << "\n";
        } else {
            outFile << "      \"bearing\": " << std::round(bearing * 100.0) / 100.0 << "\n";
        }

        outFile << "    }";

        prevObj = nextObj; // Csúsztatás
    }
    outFile << "\n  ]\n}\n";
    // eddig a módosítás
    std::cout << "Sikeresen kiírva: " << outputPath << std::endl;
    return 0;
}
