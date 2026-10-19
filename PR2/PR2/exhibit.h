#pragma once
#include "interfaces.h"
#include "review.h"
#include <vector>
#include <string>

class Exhibit : public IPrintable, public IReviewable {
protected:
    int id;
    std::string name;
    std::string creator;
    int year;
    std::string hall;
    std::string description;
    std::vector<Review> reviews;

public:
    Exhibit(int id_, std::string n, std::string c, int y,
        std::string h, std::string d);

    virtual ~Exhibit() = default;

    virtual std::string getType() const = 0;
    virtual std::string getSpecialInfo() const = 0;
    virtual std::string getExtraCSV() const = 0;

    int getId() const;
    const std::string& getName() const;
    const std::string& getCreator() const;
    int getYear() const;
    const std::string& getHall() const;
    const std::string& getDescription() const;
    const std::vector<Review>& getReviews() const;

    void addReview(const std::string& author, int rating,
        const std::string& text) override;
    double getAverageRating() const override;
    void print(std::ostream& os) const override;
    void printReviews(std::ostream& os) const;   // ← НОВОЕ
};

class Animatronic : public Exhibit {
private:
    std::string endoskeletonType;
    int dangerLevel;
public:
    Animatronic(int id, std::string n, std::string c, int y,
        std::string h, std::string d,
        std::string endo, int danger);
    std::string getType() const override;
    std::string getSpecialInfo() const override;
    std::string getExtraCSV() const override;
};

class Suit : public Exhibit {
private:
    std::string size;
    std::string material;
public:
    Suit(int id, std::string n, std::string c, int y,
        std::string h, std::string d,
        std::string s, std::string m);
    std::string getType() const override;
    std::string getSpecialInfo() const override;
    std::string getExtraCSV() const override;
};

class Artifact : public Exhibit {
private:
    std::string origin;
    std::string rarity;
public:
    Artifact(int id, std::string n, std::string c, int y,
        std::string h, std::string d,
        std::string o, std::string r);
    std::string getType() const override;
    std::string getSpecialInfo() const override;
    std::string getExtraCSV() const override;
};