#include <iostream>
#include "include/classes.h"

int x_main() {
    Range ordersCount = {
        .lowerBound = 3,
        .upperBound = 5
    };

    Range orderItemsCount = {
        .lowerBound = 1,
        .upperBound = 5
    };
    
    ConstantsPtr constants = std::make_shared<Constants>(ordersCount, orderItemsCount);

    Order ord(constants, 12);
    std::cout << ord.booksList.size() << std::endl;
    // for (auto [book, count]: ord.booksList) {
    //     std::cout << book->getName() << ' ' << count << std::endl;
    // }

    ModelInfo info = {
        .step = 1,
        .maxBookCount = 5,
        .days = 10,
        .addCost = 0.2,
        .addCostForNewBook = 0.3
    };

    System system(info, constants);
    system.startExperiment();
    return 0;
}
