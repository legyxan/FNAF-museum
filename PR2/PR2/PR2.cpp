#include "museum.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <string>
#include <vector>
#include <utility>

#define NOMINMAX
#include <windows.h>

void printMenu() {
    std::cout << "\n========== МУЗЕЙ АНИМАТРОНИКОВ FREDDY'S ==========\n"
        << " 1. Показать все экспонаты\n"
        << " 2. Показать список залов\n"
        << " 3. Зайти в зал (введите номер: 1, 2, 3)\n"
        << " 4. Найти экспонат (по имени/автору)\n"
        << " 5. Оставить отзыв\n"
        << " 6. [ФИШКА] Топ-3 экспоната по отзывам\n"
        << " 7. [ФИШКА] Рейтинг залов\n"
        << " 8. Сохранить данные в файл\n"
        << " 9. Показать отзывы об экспонате\n"
        << " 0. Выход\n"
        << "=================================================\n"
        << "Выбор: ";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    Museum museum("Музей аниматроников Freddy's");
    museum.loadFromFile("exhibits.csv");
    museum.loadReviews("reviews.txt");

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: museum.printAll(); break;
        case 2: museum.printAllHalls(); break;
        case 3: {
            std::cout << "Введите номер зала: ";
            std::string hallName;
            std::getline(std::cin, hallName);
            museum.printExhibitsInHall(hallName);
            break;
        }
        case 4: {
            std::cout << "Поиск: ";
            std::string q;
            std::getline(std::cin, q);
            auto found = museum.findByName(q);
            if (found.empty()) std::cout << "Ничего не найдено.\n";
            for (auto* e : found) { e->print(std::cout); std::cout << "---\n"; }
            break;
        }
        case 5: {
            int id, rating;
            std::string author, text;
            std::cout << "ID экспоната: "; std::cin >> id;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            Exhibit* e = museum.findById(id);
            if (!e) { std::cout << "Не найден.\n"; break; }

            e->printReviews(std::cout);
            std::cout << "\n";

            std::cout << "Имя: "; std::getline(std::cin, author);
            std::cout << "Оценка (1-5): "; std::cin >> rating;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Текст: "; std::getline(std::cin, text);
            e->addReview(author, rating, text);
            std::cout << "Отзыв добавлен!\n";
            break;
        }
        case 6: {
            std::cout << "\n=== ТОП-3 ===\n";
            auto top = museum.getTopExhibits(3);
            if (top.empty()) { std::cout << "Нет отзывов.\n"; break; }
            int p = 1;
            for (auto* e : top)
                std::cout << p++ << ") " << e->getName() << " - "
                << std::fixed << std::setprecision(2)
                << e->getAverageRating() << "\n";
            break;
        }
        case 7: {
            std::cout << "\n=== РЕЙТИНГ ЗАЛОВ ===\n";
            auto rank = museum.getHallRanking();
            int i = 1;
            for (const auto& pr : rank)
                std::cout << i++ << ") " << pr.first << " - "
                << std::fixed << std::setprecision(2) << pr.second << "\n";
            break;
        }
        case 8:
            museum.saveToFile("exhibits.csv");
            museum.saveReviews("reviews.txt");
            break;
        case 9: {
            std::cout << "ID экспоната: ";
            int id;
            std::cin >> id;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            museum.printReviews(id);
            break;
        }
        case 0: std::cout << "Выход.\n"; break;
        default: std::cout << "Нет такого пункта.\n";
        }
    }
    return 0;
}