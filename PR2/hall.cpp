#include "hall.h"

Hall::Hall(std::string n) : name(std::move(n)) {}

const std::string& Hall::getName() const { return name; }

void Hall::addExhibit(Exhibit* e) { exhibits.push_back(e); }

const std::vector<Exhibit*>& Hall::getExhibits() const { return exhibits; }

double Hall::getAverageRating() const {
    if (exhibits.empty()) return 0.0;
    double sum = 0;
    for (auto* e : exhibits) sum += e->getAverageRating();
    return sum / static_cast<double>(exhibits.size());
}