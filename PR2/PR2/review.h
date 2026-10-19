#pragma once
#include <string>

class Review {
private:
    int exhibitId;
    std::string author;
    int rating;
    std::string text;

public:
    Review(int id, std::string a, int r, std::string t)
        : exhibitId(id), author(std::move(a)),
        rating(r), text(std::move(t)) {
    }

    int getId() const { return exhibitId; }
    int getRating() const { return rating; }
    const std::string& getAuthor() const { return author; }
    const std::string& getText() const { return text; }
};