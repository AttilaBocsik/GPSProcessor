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

int main(int argc, char *argv[]) {
    // Ellenőrzés: van-e legalább 3 argumentum (programnév + input + output + mód)
    if (argc < 4) {
        std::cerr << "Használat: " << argv[0] << " <input.json> <output.json> [--metric | --imperial]" << std::endl;
        return 1; // 1: hibás hívás / hiányzó argumentum
    }

    // Parancssori argumentumok beolvasása
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    std::string mode = argv[3];
    // Mértékegység-átváltó példány (polimorfizmus: közös interfész mögött kétféle megvalósítás)
    std::unique_ptr<IUnitConverter> converter;
    if (mode == "--metric") {
        converter = std::make_unique<MetricConverter>(); // metrikus (méter, kilométer, fok stb.)
    } else if (mode == "--imperial") {
        converter = std::make_unique<ImperialConverter>(); // angolszász (például láb/mérföld)
    } else {
        std::cerr << "Hibás mértékegység! Használat: --metric vagy --imperial" << std::endl;
        return 2; // 2: ismeretlen mértékegység mód
    }
    // Bemeneti JSON fájl megnyitása
    std::ifstream inputFile(inputPath);
    if (!inputFile) {
        std::cerr << "Nem sikerült megnyitni az input fájlt: " << inputPath << std::endl;
        return 3; // 3: input fájl hiba
    }

    // Bemenet JSON-ként beolvasva
    json inputJson;
    try {
        inputFile >> inputJson;
    } catch (...) {
        std::cerr << "Hibás JSON fájlformátum!" << std::endl;
        return 4; // 4: hibás JSON
    }

    // Feltételezés: az input JSON-ben "input" kulcs alatt egy tömb van pontokkal
    const auto &points = inputJson["input"];
    json output; // Ide épül a kimeneti JSON
    // Pontpárok bejárása: minden szomszédos pont között számolunk
    for (size_t i = 1; i < points.size(); ++i) {
        // GPS koordináták kiolvasása az előző (i-1) és az aktuális (i) pontra
        GPSPoint from{points[i - 1]["GPSP"]["lat"], points[i - 1]["GPSP"]["lon"]};
        GPSPoint to{points[i]["GPSP"]["lat"], points[i]["GPSP"]["lon"]};
        // Távolság és irányszög számolása a választott konverterrel
        double dist = converter->calculateDistance(from, to);
        double bearing = converter->calculateBearing(from, to);
        // Kimeneti elem összeállítása:
        // - a "from" pont koordinátái 5 tizedesre kerekítve
        // - távolság és irányszög egészre kerekítve (int)
        output["output"].push_back({
            {
                "fromGPSP",
                {{"lat", std::round(from.lat * 100000) / 100000}, {"lon", std::round(from.lon * 100000) / 100000}}
            },
            {"distance", static_cast<int>(std::round(dist))},
            {"bearing", static_cast<int>(std::round(bearing))}
        });
    }

    // Kimeneti JSON fájl kiírása szépen formázva (2 szóköz behúzás)
    std::ofstream outFile(outputPath);
    outFile << std::setw(2) << output << std::endl;
    // Sikerüzenet a konzolra
    std::cout << "Sikeresen kiírva: " << outputPath << std::endl;
    return 0; // 0: sikeres futás
}
