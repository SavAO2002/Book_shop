#pragma once

#include <map>
#include <vector>
#include <set>
#include <string>
#include <utility>
#include <random>
#include <memory>

struct Range {
    int lowerBound;
    int upperBound;
};

struct RandomGenerator {
    std::random_device rd;
    std::default_random_engine engine;
    std::uniform_int_distribution<int> dist;

    RandomGenerator(int lower, int upper)
        : rd()
        , engine(rd())
        , dist(lower, upper)
    {}

    int generate() {
        return dist(engine);
    }
};

class Constants {
    static  std::vector<std::string> mails;

    static  std::vector<std::string> phones;

    static  std::vector<std::string> customerNames;

    static  std::vector<std::string> bookNames1;

    static  std::vector<std::string> bookNames2;

    static  std::vector<std::string> authors;

    static  std::vector<std::string> languages;

    static  std::vector<std::string> genres;

    static  std::vector<std::string> publishers;

    RandomGenerator bookCountGenerator;
    RandomGenerator deliveryGenerator;
    RandomGenerator ordersCountGenerator;
    RandomGenerator orderItemsCountGenerator;

    const int DEFAULT_BOOK_COUNT = 10;

public:

        Constants(Range ordersCount, Range orderItemsCount):
            bookCountGenerator(2, 4),
            deliveryGenerator(3, 5),
            ordersCountGenerator(ordersCount.lowerBound, ordersCount.upperBound),
            orderItemsCountGenerator(orderItemsCount.lowerBound, orderItemsCount.upperBound)
        { }

        Constants(const Constants& cnst) = default;

//////////////////////////////////////////////////////////////////////////
// Book fields
//////////////////////////////////////////////////////////////////////////

        std::string getAuthor() {
            return authors[std::rand() % authors.size()];
        }

        std::string getName() {
            return bookNames1[std::rand() % bookNames1.size()] + bookNames2[std::rand() % bookNames2.size()];
        }

        std::string getLang() {
            return languages[std::rand() % languages.size()];
        }

        std::string getYear() {
            return std::to_string(1700 + std::rand() % 300);
        }

        std::string getGenres() {
            return genres[std::rand() % genres.size()];
        }

        std::string getPublisher() {
            return publishers[std::rand() % publishers.size()];
        }

        int getPages() {
            return 30 + std::rand() % 700;
        }

        float getCost() {
            return 200.0 + std::rand() % 300;
        }

        int getRating() {
            return 0;
        }

        bool getIsNew() {
            return std::rand() % 2;
        }

//////////////////////////////////////////////////////////////////////////
// Customer fields
//////////////////////////////////////////////////////////////////////////

        std::string getCustomerName() {
            return customerNames[std::rand() % customerNames.size()];
        }

        std::string getPhone() {
            return phones[std::rand() % phones.size()];
        }

        std::string getMail() {
            return mails[std::rand() % mails.size()];
        }

//////////////////////////////////////////////////////////////////////////
// Other constants
//////////////////////////////////////////////////////////////////////////

        int getBookCount() {
            return bookCountGenerator.generate();
        }

        int getOrderCount() {
            return ordersCountGenerator.generate();
        }

        int getItemsCount() {
            return orderItemsCountGenerator.generate();
        }

        int getOrderDays() { //там от 3 до 5 дней
            return deliveryGenerator.generate();
        }

        int assortmentBookCount() {
            return DEFAULT_BOOK_COUNT;
        }
};

using ConstantsPtr = std::shared_ptr<Constants>;

inline std::vector<std::string> Constants::mails = {"oiugfdfgh@mail.ru", "asdfg@mail.ru", "tgbhui@rambler.ru", "asguis@mail.ru", "pokjhbvcx@rambler.ru"};

inline std::vector<std::string> Constants::phones = {"+79851234567", "+79161134557", "+79261194557", "+79751834560", "+792712344067"};

inline std::vector<std::string> Constants::customerNames = {"Ivan", "Maria", "Tatiana", "Aleksey", "Anton", "Marina"};

inline std::vector<std::string> Constants::bookNames1 = {"Белый", "Загадочный", "Возрождение:", "Исход:"};
inline std::vector<std::string> Constants::bookNames2 = {" сон", " Орден", " рыцарь", " мир", " ворон"};

inline std::vector<std::string> Constants::authors = {"Marissa Meier", "M.I. Scanavi", "J.K. Rowling", "A.S. Pushkin", "M. Chembers"};

inline std::vector<std::string> Constants::languages = {"russian", "english", "french", "chinese"};

inline std::vector<std::string> Constants::genres = {"detective", "classic", "fantasy", "history", "adventures", "romantic", "science", "poetry"};

inline std::vector<std::string> Constants::publishers = {"Mahaon", "Terra Fantastica", "Мир и Образование", "Rampion Books"};
