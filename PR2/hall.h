#pragma once
#include "exhibit.h"
#include <vector>
#include <string>

class Hall {
private:
    std::string name;
    std::vector<Exhibit*> exhibits;

public:
    explicit Hall(std::string n);

    const std::string& getName() const;
    void addExhibit(Exhibit* e);
    const std::vector<Exhibit*>& getExhibits() const;
    double getAverageRating() const;
};