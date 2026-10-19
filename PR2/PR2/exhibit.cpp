#include "exhibit.h"
#include "textfix.h"
#include <algorithm>
#include <iomanip>

Exhibit::Exhibit(int id_, std::string n, std::string c, int y,
    std::string h, std::string d)
    : id(id_), name(std::move(n)), creator(std::move(c)),
    year(y), hall(std::move(h)), description(std::move(d)) {
}

int Exhibit::getId() const { return id; }
const std::string& Exhibit::getName() const { return name; }
const std::string& Exhibit::getCreator() const { return creator; }
int Exhibit::getYear() const { return year; }
const std::string& Exhibit::getHall() const { return hall; }
const std::string& Exhibit::getDescription() const { return description; }
const std::vector<Review>& Exhibit::getReviews() const { return reviews; }

void Exhibit::addReview(const std::string& author, int rating,
    const std::string& text) {
    int r = std::clamp(rating, 1, 5);
    reviews.emplace_back(id, author, r, text);
}

double Exhibit::getAverageRating() const {
    if (reviews.empty()) return 0.0;
    double sum = 0;
    for (const auto& r : reviews) sum += r.getRating();
    return sum / static_cast<double>(reviews.size());
}

void Exhibit::print(std::ostream& os) const {
    os << "[" << fix(getType()) << "] #" << id << " \"" << fix(name) << "\"\n"
        << "  " << fix("Автор") << ": " << fix(creator) << " (" << year << ")\n"
        << "  " << fix("Зал") << ": " << fix(hall) << "\n"
        << "  " << fix("Описание") << ": " << fix(description) << "\n"
        << "  " << fix("Особенности") << ": " << fix(getSpecialInfo()) << "\n"
        << "  " << fix("Отзывов") << ": " << reviews.size()
        << ", " << fix("средний рейтинг") << ": "
        << std::fixed << std::setprecision(2) << getAverageRating() << "\n";

    if (!reviews.empty()) {
        os << "  " << fix("Отзывы посетителей") << ":\n";
        for (size_t i = 0; i < reviews.size(); ++i) {
            os << "   " << (i + 1) << ". " << fix(reviews[i].getAuthor())
                << " (" << reviews[i].getRating() << "/5): "
                << fix(reviews[i].getText()) << "\n";
        }
    }
}

void Exhibit::printReviews(std::ostream& os) const {
    os << "\n=== " << fix("Отзывы об экспонате") << " \""
        << fix(name) << "\" ===\n";
    if (reviews.empty()) {
        os << fix("Отзывов пока нет.") << "\n";
        return;
    }
    for (size_t i = 0; i < reviews.size(); ++i) {
        os << (i + 1) << ") " << fix(reviews[i].getAuthor())
            << " - " << fix("оценка") << " "
            << reviews[i].getRating() << "/5\n"
            << "   " << fix(reviews[i].getText()) << "\n\n";
    }
    os << fix("Средний рейтинг") << ": "
        << std::fixed << std::setprecision(2) << getAverageRating() << "\n";
}

// --- Animatronic ---
Animatronic::Animatronic(int id, std::string n, std::string c, int y,
    std::string h, std::string d,
    std::string endo, int danger)
    : Exhibit(id, std::move(n), std::move(c), y, std::move(h), std::move(d)),
    endoskeletonType(std::move(endo)), dangerLevel(danger) {
}

std::string Animatronic::getType() const { return "Animatronic"; }

std::string Animatronic::getSpecialInfo() const {
    return fix("Эндоскелет") + ": " + fix(endoskeletonType) +
        ", " + fix("уровень опасности") + ": " +
        std::to_string(dangerLevel) + "/5";
}

std::string Animatronic::getExtraCSV() const {
    return endoskeletonType + "," + std::to_string(dangerLevel);
}

// --- Suit ---
Suit::Suit(int id, std::string n, std::string c, int y,
    std::string h, std::string d,
    std::string s, std::string m)
    : Exhibit(id, std::move(n), std::move(c), y, std::move(h), std::move(d)),
    size(std::move(s)), material(std::move(m)) {
}

std::string Suit::getType() const { return "Suit"; }

std::string Suit::getSpecialInfo() const {
    return fix("Размер") + ": " + fix(size) + ", " +
        fix("материал") + ": " + fix(material);
}

std::string Suit::getExtraCSV() const {
    return size + "," + material;
}

// --- Artifact ---
Artifact::Artifact(int id, std::string n, std::string c, int y,
    std::string h, std::string d,
    std::string o, std::string r)
    : Exhibit(id, std::move(n), std::move(c), y, std::move(h), std::move(d)),
    origin(std::move(o)), rarity(std::move(r)) {
}

std::string Artifact::getType() const { return "Artifact"; }

std::string Artifact::getSpecialInfo() const {
    return fix("Происхождение") + ": " + fix(origin) +
        ", " + fix("редкость") + ": " + fix(rarity);
}

std::string Artifact::getExtraCSV() const {
    return origin + "," + rarity;
}