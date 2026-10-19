#pragma once
#include <iostream>
#include <string>

class IPrintable {
public:
    virtual void print(std::ostream& os) const = 0;
    virtual ~IPrintable() = default;
};

class IReviewable {
public:
    virtual void addReview(const std::string& author, int rating,
        const std::string& text) = 0;
    virtual double getAverageRating() const = 0;
    virtual ~IReviewable() = default;
};