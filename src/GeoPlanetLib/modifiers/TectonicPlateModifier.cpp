
#include <random>
#include <GeoPlanetLib/Utils.h>
#include <GeoPlanetLib/modifiers/TectonicPlateModifier.h>

using namespace gp;
using namespace gp::modifiers;
using namespace std;

/**
 * Used
 * https://stackoverflow.com/questions/22883840/c-get-random-number-from-0-to-max-long-long-integer
 * post about large random number generators
 */
bool TectonicPlateModifier::apply(std::shared_ptr<Surface> surface)
{
    surface->plates.clear();
    TectonicPlate::removePlatesFromSurface(surface);

    auto regions = surface->getRegions();

    std::random_device rd;
    std::mt19937_64 eng(rd()); // 64-bit Mersenne Twister 19937 generator
    std::uniform_int_distribution<unsigned long> distr;

    float expRange = abs(getFloatVariable("expansionRateRange"));

    for (unsigned int i = 0; i < numberOfPlates; ++i) {
        auto tectonicPlate = make_shared<TectonicPlate>(surface);
        auto randomIndex = distr(eng) % regions.size();
        tectonicPlate->addRegion(regions[randomIndex]);
        surface->plates.push_back(tectonicPlate);
        expansionRateMap[tectonicPlate.get()] = rand_f(-expRange, expRange);
    }

    if (!stepMode) {
        runPlateFloddFill(surface);
    }

    return true;
}

void TectonicPlateModifier::runPlateFloddFill(std::shared_ptr<Surface> surface)
{
    while (!expansionFinished) {
        expansionFinished = stepExpandPlates(surface);
    }
}

bool TectonicPlateModifier::stepExpandPlates(std::shared_ptr<Surface> surface)
{
    if (!expansionFinished) {
        expansionFinished = true;

        // // more random driven expansion
        // if (stepExpandPlate(surface->plates[rand() % surface->plates.size()])) {
        //     expansionFinished = false;
        // } else {
        //     for (auto plate : surface->plates) {
        //         if (stepExpandPlate(plate)) {
        //             expansionFinished = false;
        //             break;
        //         }
        //     }
        // }

        // evenly distributed expansion
        for (auto plate : surface->plates) {
            if (stepExpandPlate(plate)) {
                expansionFinished = false;
            }
        }
    }
	return expansionFinished;
}

bool TectonicPlateModifier::stepExpandPlate(std::shared_ptr<TectonicPlate> plate)
{
    // expand once for each whole value of expansion rate in this step
    auto rate = expansionRateMap[plate.get()];
    while (rate > 0) {
        if (!plate->expand()) {
            return false;
        }
        rate -= 1.0;
    }

    // last expansion is done probabilistically by rate
    if (rand_f() > rate) {
        if (!plate->expand()) {
            return false;
        }
    }
    return true;
}