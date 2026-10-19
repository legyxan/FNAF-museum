#pragma once
#include "exhibit.h"
#include "hall.h"
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <utility>

class Museum {
private:
    std::string name;
    std::vector<std::unique_ptr<Exhibit>> exhibits;
    std::vector<std::unique_ptr<Hall>> halls;
    std::map<std::string, Hall*> hallIndex;
    int nextId;

    Hall* getOrCreateHall(const std::string& hallName);

public:
    explicit Museum(std::string n);
    ~Museum() = default;

    void loadFromFile(const std::string& filename);
    void loadReviews(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void saveReviews(const std::string& filename) const;

    Exhibit* findById(int id) const;
    std::vector<Exhibit*> findByName(const std::string& q) const;

    std::vector<Exhibit*> getTopExhibits(int n = 3) const;
    std::vector<std::pair<std::string, double>> getHallRanking() const;

    void printAllHalls() const;
    void printExhibitsInHall(const std::string& hallName) const;
    void printAll() const;
    void printReviews(int exhibitId) const;      // ← НОВОЕ

    void addExhibit(std::unique_ptr<Exhibit> e);

    static std::string toLower(std::string s);
    static std::string trim(std::string s);
};