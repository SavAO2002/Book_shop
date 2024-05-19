#pragma once

#include "constants.h"
#include <QDebug>

struct Book {
    static int bookCount; // for diff ids

    int id; // identifier for searching
    std::string author;
    std::string name;
    std::string lang;
    std::string publishYear;
    std::string genre;
    std::string publisher;
    int pageCount;
    float cost;
    float addCost;
    int rating;
    bool isNew;
    bool isOrdered = false;

    Book() = default;

    Book(ConstantsPtr constants, float addCost_, float addNewCost_):
        id(bookCount), 
        author(constants->getAuthor()),
        name(constants->getName()),
        lang(constants->getLang()),
        publishYear(constants->getYear()),
        genre(constants->getGenres()),
        publisher(constants->getPublisher()),
        pageCount(constants->getPages()),
        cost(constants->getCost()),
        //addCost(addCost),
        rating(constants->getRating()),
        isNew(constants->getIsNew())
    {
        if (isNew) {
            addCost = addNewCost_;
        } else {
            addCost = addCost_;
        }
        bookCount++;
    };

    void upRating() {
        rating += 1;
    }

    std::string getName() {
        return name;
    }
};

inline int Book::bookCount = 1;
using BookPtr = std::shared_ptr<Book>;
using Books = std::map<int, BookPtr>; // <id, Book>

struct Customer {
    std::string name;
    std::string phone;
    std::string mail;

    Customer() = default;

    Customer(ConstantsPtr constants):
        name(constants->getCustomerName()),
        phone(constants->getPhone()),
        mail(constants->getMail())
    { };
};

struct Order {
    Customer customer;
    std::map<BookPtr, int> booksList;

    Order() = default;

    Order(ConstantsPtr constants, int itemsCount, Books&& assortment) {
        customer = Customer(constants);
        for (int i = 0; i < itemsCount; i++) {
            BookPtr book = assortment[Book::bookCount - 1 - int(std::rand() % assortment.size())];
            booksList.insert({book, constants->getBookCount()});
        }
    }
};

using OrderPtr = std::shared_ptr<Order>;
using Orders = std::vector<OrderPtr>;

struct StatisticInfo {
    std::map<std::string, int> selledBooksThematics = {{"detective", 0}, {"classic", 0}, {"fantasy", 0}, {"history", 0}, {"adventures", 0}, {"romantic", 0}, {"science", 0}, {"poetry", 0}}; // кол-во проданных книг по тематикам
    std::map<int, int> topSelled; // список продаваемых книг  <id, count>, надо отсортировать
    int processedOrdersCount = 0;
    int publisherOrdersCount = 0;
    float profit = 0.0;
};

class BookShop {
    ConstantsPtr constants_;
    Books assortment;
    std::vector<std::pair<int, OrderPtr>> ordersList; // специальные заказы для покупателей
    std::map<int, int> bookBase; // <id, count>
    std::vector<std::pair<int, std::map<int, int>>> publisherOrders; // заказы из-за отсутствия книг
    StatisticInfo statInfo;

public:

    BookShop() = default;

    BookShop(Books& books, ConstantsPtr constants):
        constants_(constants),
        assortment(std::move(books))
    {
        for (const auto[id, book]: assortment) {
            bookBase[id] = constants->assortmentBookCount();
        }
    }

    Books getAssortment() {
        return assortment;
    }

    std::map<int, int> getBookBase() {
        return bookBase;
    }

    void setPublishOrders(std::vector<std::pair<int, std::map<int, int>>>&  orders) {
        publisherOrders = std::move(orders);
    }

    void setSpecialOrders(std::vector<std::pair<int, OrderPtr>>&  orders) {
        ordersList = std::move(orders);
    }

    std::vector<std::pair<int, std::map<int, int>>> getPublisherOrders() {
        return publisherOrders;
    }

    std::vector<std::pair<int, OrderPtr>> getSpecialOrders() {
        return ordersList;
    }

    StatisticInfo getStatInfo() {
        return statInfo;
    }

    int booksSize() {
        return assortment.size();
    }

    int ordersSize() {
        return ordersList.size();
    }

    BookPtr findBook(int id) {
        return assortment[id];
    }

    void checkSmallBookCount() {
        std::map<int, int> order; // <id, count>
        int orderDays = 0;
        for (auto it = bookBase.begin(); it != bookBase.end(); it++) {
            if (bookBase[it->first] < 5) {
                if (!assortment[it->first]->isOrdered) {
                    order.insert({it->first, 10});
                    assortment[it->first]->isOrdered = true;
                    statInfo.profit -= assortment[it->first]->cost * (1 + assortment[it->first]->addCost) * 10;
                }
            }
        }
        if (!order.empty()) {
            orderDays = constants_->getOrderDays();
            publisherOrders.push_back({orderDays, order});
            statInfo.publisherOrdersCount += 1;
        }
    }

    void updateProfit(float num) {
        statInfo.profit += num;
    }

    void updateOrderBase(const std::pair<int, OrderPtr> order) {
        ordersList.push_back(order);
    }

    void updateBase(const std::map<int, int>& order) {
        for (const auto [id, count]: order) {
            bookBase[id] += count;
            assortment[id]->isOrdered = false;
        }
    }

    std::tuple<OrderPtr, int> processingOrder(OrderPtr order) {
        int countToPublisher = 0;
        Order specialOrder;
        Order processedOrder;
        int orderDays = constants_->getOrderDays();
        for (auto [book, count]: order->booksList) {
            int id = book->id;
            if (bookBase[id] >= count) {
                bookBase[id] -= count;
                processedOrder.booksList.insert({book, count});
                //далее сбор статистической информации
                if (statInfo.topSelled.find(id) != statInfo.topSelled.end()) {
                    statInfo.topSelled[id] += count;
                } else {
                    statInfo.topSelled.insert({id, count});
                }
                statInfo.publisherOrdersCount += 1;
                statInfo.selledBooksThematics[book->genre] += count;
                statInfo.profit += book->cost * (1 + book->addCost) * count;
                //
            } else {
                if (bookBase[id] != 0) {
                    processedOrder.booksList.insert({book, bookBase[id]});
                    statInfo.selledBooksThematics[book->genre] += bookBase[id];
                    statInfo.profit += book->cost * (1 + book->addCost) * bookBase[id];
                }
                int del = count - bookBase[id];
                bookBase[id] = 0;
                countToPublisher++;
                specialOrder.booksList.insert({book, del});
            }
            book->upRating();
        }
        processedOrder.customer = order->customer;
        if (!specialOrder.booksList.empty()) { // если надо дозаказать отдельному товарищу
            specialOrder.customer = order->customer;
            updateOrderBase({constants_->getOrderDays(), std::make_shared<Order>(specialOrder)});
        } else {
            statInfo.processedOrdersCount += 1;
        }

        if (countToPublisher != 0) {
            orderDays += constants_->getOrderDays();
        }
        return std::make_tuple(std::make_shared<Order>(processedOrder), orderDays);
    }
};

struct ModelInfo {
    int step;
    int maxBookCount;
    int days;
    float addCost;
    float addCostForNewBook;
};

struct OrdersInfo {
    using OneOrder = std::map<int, int>;

    //std::map<int, OneOrder> publisherOrders;
    Orders processedOrders;
    Orders incomingOrders;
};

class System {
    BookShop shop;
    ModelInfo info_;
    bool skip = false;
    ConstantsPtr constants_;

public:

    System() = default;

    BookShop getShop() {
        return shop;
    }

    ModelInfo getInfo() {
        return info_;
    }

    void updateProfit(float num) {
        shop.updateProfit(num);
    }

    void updateOrders(std::vector<std::pair<int, std::map<int, int>>>& orders) {
        shop.setPublishOrders(orders);
    }

    void updateSpecialOrders(std::vector<std::pair<int, OrderPtr>>& orders) {
        shop.setSpecialOrders(orders);
    }

    void updateBase(const std::map<int, int>& orders) {
        shop.updateBase(orders);
    }

    Orders createOrderList() {
        Orders list;
        int count = constants_->getOrderCount() % info_.maxBookCount;
        for (int i = 0; i < count; i++) {
            int itemsCount = constants_->getItemsCount();
            list.emplace_back(std::make_shared<Order>(constants_, itemsCount, shop.getAssortment()));
        }
        return list;
    }

    Books createStartAssortment() {
        Books assortment;
        for (int i = 0; i < info_.maxBookCount; i++) {
            BookPtr book = std::make_shared<Book>(constants_, info_.addCost, info_.addCostForNewBook);
            assortment.insert({book->id, book});
        }
        return assortment;
    }

    System(ModelInfo& info, ConstantsPtr constants): info_(std::move(info)), constants_(constants) {
        Books assortment = createStartAssortment();
        shop = std::move(BookShop(assortment, constants));
    }


    OrdersInfo step() {
        Orders orders = createOrderList();
        OrdersInfo info;
        std::copy(orders.begin(), orders.end(), std::back_inserter(info.incomingOrders));
        for (auto order: orders) {
            auto [processedOrder, time] = shop.processingOrder(order);

            if (!processedOrder->booksList.empty()) {
                info.processedOrders.push_back(processedOrder);
            }
        }

        shop.checkSmallBookCount();
        return info;
    }

};
