#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSizePolicy>

QString MainWindow::convertToText(Books assortment, std::map<int, int> base) { // конвертация для вывода ассортимента
    std::string text;
    for (const auto [id, book]: assortment) {
        text += std::to_string(id) + " - " + book->name + "; " + book->author + ", количество: " + std::to_string(base[id]) + ";\n";
    }
    return QString::fromStdString(text);
}

QString MainWindow::ordersToText(Orders orders) { // конвертация для вывода общего потока заказов
    std::string text;
    for (const auto order: orders) {
        text += order->customer.name + " - " + order->customer.mail + ";\n";
        for (const auto [book, count]: order->booksList) {
            text +=std::to_string(book->id) + " - " + book->name + ", количество: " + std::to_string(count) + ", стоимость: " + std::to_string(book->cost * (1 + book->addCost) * count) +";\n";
        }
        text += "\n";
    }
    text += "-------------------------\n";
    return QString::fromStdString(text);
}

template <typename T>
QString MainWindow::publisherOrdersToText(T orders, auto bookBase) { // конвертация для вывода времени выполнения заказов + вывод заказанных в издательстве книг, если необходимо
    std::string text;
    for (const auto [time, order]: orders) {
        text += "Время выполнения заказа - " + std::to_string(time)+ ";\n";
        if (order.empty()) {
            text += "Заявок в издательство нет";
        }
        for (const auto [id, count]: order) {
            text += "Id: " + std::to_string(id) + ", количество: " + std::to_string(count) + ", стоимость: " + std::to_string(bookBase[id]->cost * (1 + bookBase[id]->addCost) * count) + ";\n";
        }
        text += "\n";
    }
    text += "-------------------------\n";
    return QString::fromStdString(text);
}

std::string orderToText(OrderPtr order) {
    std::string text;
    text += order->customer.name + " - " + order->customer.mail + ";\n";
    for (const auto [book, count]: order->booksList) {
        text += "Id: " + std::to_string(book->id) + ", количество: " + std::to_string(count) + ", стоимость: " + std::to_string(book->cost * (1 + book->addCost) * count) + ";\n";
    }
    text += "\n";
    return text;
}

QString MainWindow::specialOrdersToText(std::vector<std::pair<int, OrderPtr>> orders) { // конвертация для вывода времени выполнения специальных заказов
    std::string text;
    for (const auto [time, order]: orders) {
        text += "Время выполнения заказа - " + std::to_string(time)+ ";\n";
        text += orderToText(order);
    }
    text += "-------------------------\n";
    return QString::fromStdString(text);
}

void MainWindow::printStepInfo(int step) { // вывод информации по работе магазина
    ui->assortmentText->setPlainText(QString::fromStdString("Ассортимент:\n"));
    ui->assortmentText->moveCursor(QTextCursor::End);
    ui->assortmentText->insertPlainText(convertToText(system_.getShop().getAssortment(), system_.getShop().getBookBase()));

    ui->infoText->setPlainText(QString::fromStdString("Общий поток заказов:\n"));
    ui->processedText->setPlainText(QString::fromStdString("Обработанные заказы:\n"));

    for (int i = 0; i < step; i++) {
        updateOrdersAfterDay(system_);

        OrdersInfo ordersInfo = system_.step();
        ui->infoText->moveCursor(QTextCursor::End);
        ui->infoText->insertPlainText(ordersToText(ordersInfo.incomingOrders));

        ui->processedText->moveCursor(QTextCursor::End);
        ui->processedText->insertPlainText(ordersToText(ordersInfo.processedOrders));

        ui->publisherText->moveCursor(QTextCursor::End);
        ui->publisherText->setPlainText(specialOrdersToText(system_.getShop().getSpecialOrders()));

        ui->publisherText->moveCursor(QTextCursor::End);
        ui->publisherText->insertPlainText(publisherOrdersToText(system_.getShop().getPublisherOrders(), system_.getShop().getAssortment()));
    }
}

void MainWindow::printStatInfo(StatisticInfo info) { // вывод статистической информации
    QMessageBox msgBox;
    std::string text;
    Books assortment = system_.getShop().getAssortment();
    text += "Заработок: " + std::to_string(info.profit) + "\n";
    text += "Количество обработанных заказов: " + std::to_string(info.processedOrdersCount) + "\n";
    text += "Количество заказов в издательство: " + std::to_string(info.publisherOrdersCount) + "\n";
    text += "Количество проданных книг по тематикам:\n";
    for (const auto[theme, count]: info.selledBooksThematics) {
        text += theme + " - " + std::to_string(count) + "\n";
    }
    text += "\n";
    text += "Топ продаваемых книг: \n";

    std::vector<std::pair<int, int>> pairs;
    for (auto it = info.topSelled.begin(); it != info.topSelled.end(); it++) {
        pairs.push_back(*it);
    }
    std::sort(pairs.begin(), pairs.end(), [=] (std::pair<int, int> p1, std::pair<int, int> p2) {
        return p1.second < p2.second;
    });
    for (const auto[id, count]: pairs) {
        text += assortment[id]->name + "; " + assortment[id]->author + " - " + std::to_string(count) + "\n";
    }
    msgBox.setWindowTitle("Statistic information");
    msgBox.setText(QString::fromStdString(text));
    msgBox.setMinimumHeight(0);
    msgBox.setMaximumHeight(16777215);
    msgBox.setMinimumWidth(0);
    msgBox.setMaximumWidth(16777215);
    msgBox.resize(2000, 2000);
    msgBox.exec();
    return;
}

bool someFieldBlank(Ui::MainWindow *ui) { // проверка, что все поля заполнены
    return ui->requestCountLower->text().isEmpty() || ui->requestCountUpper->text().isEmpty() ||
           ui->requestItemsCountLower->text().isEmpty() || ui->requestItemsCountUpper->text().isEmpty() ||
           ui->addCostLine->text().isEmpty() || ui->addCostNewLine->text().isEmpty() || ui->daysLine->text().isEmpty() ||
           ui->bookCountLine->text().isEmpty() || ui->stepLine->text().isEmpty();
}

void MainWindow::updateOrdersAfterDay(System& system_) {
    auto pubOrders = system_.getShop().getPublisherOrders(); // update orders to publisher
    for (auto it = pubOrders.begin(); it != pubOrders.end(); it++)  {
        if (it->first - 1 == 0) {
            system_.updateBase(it->second);
            it = pubOrders.erase(it);
            it--;
        } else {
            it->first -= 1;
        }
    }
    system_.updateOrders(pubOrders);

    auto specialOrders = system_.getShop().getSpecialOrders(); // update customer orders
    for (auto it = specialOrders.begin(); it != specialOrders.end(); it++)  {
        if (it->first == 1) {
            ui->processedText->moveCursor(QTextCursor::End);
            ui->processedText->insertPlainText(QString::fromStdString(orderToText(it->second)));

            for (const auto [book, count]: it->second->booksList) {
                system_.updateProfit(book->cost * (1 + book->addCost) * count);
            }

            it = specialOrders.erase(it);
            it--;
        } else {
            it->first -= 1;
        }
    }
    system_.updateSpecialOrders(specialOrders);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_startButton_clicked()
{
    Range ordersCount;
    Range orderItemsCount;
    ModelInfo info;

    if (someFieldBlank(ui)) {
        QMessageBox msgBox;
        msgBox.setText("Не заполнены поля с параметрами");
        msgBox.exec();
        return;
    }

    ui->stepButton->setEnabled(true);
    ui->skipButton->setEnabled(true);

    ordersCount.lowerBound = ui->requestCountLower->text().toInt();
    ordersCount.upperBound = ui->requestCountUpper->text().toInt();
    orderItemsCount.lowerBound = ui->requestItemsCountLower->text().toInt();
    orderItemsCount.upperBound = ui->requestItemsCountUpper->text().toInt();

    info.addCost = ui->addCostLine->text().toFloat();
    info.addCostForNewBook = ui->addCostNewLine->text().toFloat();
    info.days = ui->daysLine->text().toInt();
    info.maxBookCount = ui->bookCountLine->text().toInt();
    info.step = ui->stepLine->text().toInt();

    system_ = System(info, std::make_shared<Constants>(ordersCount, orderItemsCount));
    ui->dayLine->setText(QString::fromStdString("1"));

    if (2 > system_.getInfo().days){
        ui->stepButton->setDisabled(true);
        ui->skipButton->setDisabled(true);
        printStatInfo(system_.getShop().getStatInfo());
    }

    printStepInfo(1);
}

void MainWindow::on_stepButton_clicked()
{
    bool ok;
    bool modelFinal = false;
    int previousDay = ui->dayLine->text().toInt(&ok);
    int step;
    if (previousDay + system_.getInfo().step >= system_.getInfo().days){
        // block buttons
        ui->stepButton->setDisabled(true);
        ui->skipButton->setDisabled(true);
        ui->dayLine->setText(QString::fromStdString(std::to_string(system_.getInfo().days)));
        step = system_.getInfo().days - previousDay;
        modelFinal = true;
    } else {
        ui->dayLine->setText(QString::fromStdString(std::to_string(previousDay + system_.getInfo().step)));
        step = system_.getInfo().step;
    }

    printStepInfo(step);
    if (modelFinal) {
        printStatInfo(system_.getShop().getStatInfo());
    }

}

void MainWindow::on_skipButton_clicked()
{
    bool ok;
    int remainingDays = system_.getInfo().days;
    int previousDay = ui->dayLine->text().toInt(&ok);
    remainingDays -= previousDay;

    printStepInfo(remainingDays);

    ui->stepButton->setDisabled(true);
    ui->skipButton->setDisabled(true);
    ui->dayLine->setText(QString::fromStdString(std::to_string(system_.getInfo().days)));

    printStatInfo(system_.getShop().getStatInfo());
}

void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}
