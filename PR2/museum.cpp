#include "museum.h"
#include "textfix.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <vector>

#define NOMINMAX
#include <windows.h>

static std::string findFile(const std::string& filename) {
    std::vector<std::string> candidates;
    candidates.push_back(filename);
    candidates.push_back("../" + filename);
    candidates.push_back("../../" + filename);
    candidates.push_back("../../../" + filename);
    candidates.push_back("../../../../" + filename);

    char exePath[MAX_PATH] = { 0 };
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH)) {
        std::string exeDir(exePath);
        size_t pos = exeDir.find_last_of("\\/");
        if (pos != std::string::npos) exeDir = exeDir.substr(0, pos);
        candidates.push_back(exeDir + "\\" + filename);
        candidates.push_back(exeDir + "\\..\\" + filename);
        candidates.push_back(exeDir + "\\..\\..\\" + filename);
        candidates.push_back(exeDir + "\\..\\..\\..\\" + filename);
        candidates.push_back(exeDir + "\\..\\..\\PR2\\" + filename);
        candidates.push_back(exeDir + "\\..\\..\\..\\PR2\\" + filename);
        candidates.push_back(exeDir + "\\..\\..\\..\\PR2\\PR2\\" + filename);
    }

    for (size_t i = 0; i < candidates.size(); ++i) {
        std::ifstream f(candidates[i]);
        if (f.is_open()) return candidates[i];
    }
    return filename;
}

Hall* Museum::getOrCreateHall(const std::string& hallName) {
    auto it = hallIndex.find(hallName);
    if (it != hallIndex.end()) return it->second;
    auto h = std::make_unique<Hall>(hallName);
    Hall* ptr = h.get();
    halls.push_back(std::move(h));
    hallIndex[hallName] = ptr;
    return ptr;
}

Museum::Museum(std::string n) : name(std::move(n)), nextId(1) {}

void Museum::loadFromFile(const std::string& filename) {
    std::string path = findFile(filename);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << "\n";
        return;
    }

    unsigned char bom[3] = { 0, 0, 0 };
    file.read(reinterpret_cast<char*>(bom), 3);
    if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)) {
        file.seekg(0);
    }

    std::string line;
    bool firstLine = true;
    int loaded = 0;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (firstLine) { firstLine = false; continue; }

        std::stringstream ss(line);
        std::vector<std::string> t;
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            t.push_back(fix(tok));   // ← авто-фикс: UTF-8 → 1251
        }

        if (t.size() < 9) continue;

        try {
            int id = std::stoi(t[0]);
            const std::string& type = t[1];
            int year = std::stoi(t[4]);

            std::unique_ptr<Exhibit> e;
            if (type == "Animatronic") {
                e = std::make_unique<Animatronic>(
                    id, t[2], t[3], year, t[5], t[6], t[7], std::stoi(t[8]));
            }
            else if (type == "Suit") {
                e = std::make_unique<Suit>(
                    id, t[2], t[3], year, t[5], t[6], t[7], t[8]);
            }
            else if (type == "Artifact") {
                e = std::make_unique<Artifact>(
                    id, t[2], t[3], year, t[5], t[6], t[7], t[8]);
            }
            else {
                continue;
            }

            getOrCreateHall(e->getHall())->addExhibit(e.get());
            if (id >= nextId) nextId = id + 1;
            exhibits.push_back(std::move(e));
            ++loaded;
        }
        catch (...) {}
    }
    std::cout << "Загружено экспонатов: " << loaded << "\n";
}

void Museum::loadReviews(const std::string& filename) {
    std::string path = findFile(filename);
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Файл отзывов не открыт: " << filename << "\n";
        return;
    }

    std::string line;
    int loaded = 0;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<std::string> t;
        std::string tok;
        while (std::getline(ss, tok, ';')) t.push_back(fix(tok));

        if (t.size() < 4) continue;

        try {
            int id = std::stoi(t[0]);
            int rating = std::stoi(t[1]);
            if (Exhibit* e = findById(id)) {
                e->addReview(t[2], rating, t[3]);
                ++loaded;
            }
        }
        catch (...) {}
    }
    std::cout << "Загружено отзывов: " << loaded << "\n";
}

void Museum::saveToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    file << "id,type,name,creator,year,hall,description,param1,param2\n";
    for (const auto& e : exhibits) {
        file << e->getId() << ',' << e->getType() << ','
            << win1251ToUtf8(e->getName()) << ','
            << win1251ToUtf8(e->getCreator()) << ','
            << e->getYear() << ','
            << win1251ToUtf8(e->getHall()) << ','
            << win1251ToUtf8(e->getDescription()) << ','
            << win1251ToUtf8(e->getExtraCSV()) << '\n';
    }
    std::cout << "Сохранено экспонатов: " << exhibits.size() << "\n";
}

void Museum::saveReviews(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    for (const auto& e : exhibits)
        for (const auto& r : e->getReviews())
            file << e->getId() << ';' << r.getRating() << ';'
            << win1251ToUtf8(r.getAuthor()) << ';'
            << win1251ToUtf8(r.getText()) << '\n';
    std::cout << "Отзывы сохранены.\n";
}

Exhibit* Museum::findById(int id) const {
    for (const auto& e : exhibits)
        if (e->getId() == id) return e.get();
    return nullptr;
}

std::vector<Exhibit*> Museum::findByName(const std::string& q) const {
    std::vector<Exhibit*> res;
    std::string needle = toLower(q);
    for (const auto& e : exhibits) {
        if (toLower(e->getName()).find(needle) != std::string::npos ||
            toLower(e->getCreator()).find(needle) != std::string::npos)
            res.push_back(e.get());
    }
    return res;
}

std::vector<Exhibit*> Museum::getTopExhibits(int n) const {
    std::vector<Exhibit*> sorted;
    for (const auto& e : exhibits)
        if (!e->getReviews().empty()) sorted.push_back(e.get());
    std::sort(sorted.begin(), sorted.end(),
        [](Exhibit* a, Exhibit* b) {
            return a->getAverageRating() > b->getAverageRating();
        });
    if (static_cast<int>(sorted.size()) > n) sorted.resize(n);
    return sorted;
}

std::vector<std::pair<std::string, double>> Museum::getHallRanking() const {
    std::vector<std::pair<std::string, double>> res;
    for (const auto& h : halls)
        res.emplace_back(h->getName(), h->getAverageRating());
    std::sort(res.begin(), res.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    return res;
}

void Museum::printAllHalls() const {
    std::cout << "\n=== " << fix("Залы музея") << " ===\n";
    if (halls.empty()) { std::cout << "(пусто)\n"; return; }
    int i = 1;
    for (const auto& h : halls) {
        std::cout << " " << i++ << ") " << fix(h->getName())
            << " (" << fix("экспонатов") << ": " << h->getExhibits().size()
            << ", " << fix("ср. рейтинг") << ": " << std::fixed << std::setprecision(2)
            << h->getAverageRating() << ")\n";
    }
}

void Museum::printExhibitsInHall(const std::string& hallName) const {
    std::string inputDigits;
    for (size_t i = 0; i < hallName.size(); ++i)
        if (hallName[i] >= '0' && hallName[i] <= '9') inputDigits += hallName[i];

    for (size_t i = 0; i < halls.size(); ++i) {
        const std::string& nm = halls[i]->getName();
        std::string hallDigits;
        for (size_t j = 0; j < nm.size(); ++j)
            if (nm[j] >= '0' && nm[j] <= '9') hallDigits += nm[j];

        if (!inputDigits.empty() && hallDigits == inputDigits) {
            std::cout << "\n=== " << fix(halls[i]->getName()) << " ===\n";
            for (size_t k = 0; k < halls[i]->getExhibits().size(); ++k) {
                halls[i]->getExhibits()[k]->print(std::cout);
                std::cout << "---\n";
            }
            return;
        }
    }

    auto it = hallIndex.find(hallName);
    if (it != hallIndex.end()) {
        std::cout << "\n=== " << fix(it->second->getName()) << " ===\n";
        for (size_t k = 0; k < it->second->getExhibits().size(); ++k) {
            it->second->getExhibits()[k]->print(std::cout);
            std::cout << "---\n";
        }
        return;
    }

    std::cout << fix("Зал не найден. Доступные залы") << ":\n";
    for (const auto& h : halls)
        std::cout << "  - " << fix(h->getName()) << "\n";
}

void Museum::printAll() const {
    std::cout << "\n=== " << fix("Все экспонаты музея") << " \""
        << fix(name) << "\" ===\n";
    if (exhibits.empty()) { std::cout << "(пусто)\n"; return; }
    for (const auto& e : exhibits) {
        e->print(std::cout);
        std::cout << "---\n";
    }
}

void Museum::addExhibit(std::unique_ptr<Exhibit> e) {
    getOrCreateHall(e->getHall())->addExhibit(e.get());
    exhibits.push_back(std::move(e));
}

std::string Museum::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string Museum::trim(std::string s) {
    const char* ws = " \t\r\n\f\v";
    auto start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}
void Museum::printReviews(int exhibitId) const {
    Exhibit* e = findById(exhibitId);
    if (!e) {
        std::cout << fix("Экспонат с таким ID не найден.") << "\n";
        return;
    }
    e->printReviews(std::cout);
}