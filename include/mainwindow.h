#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include "include/classes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString convertToText(Books assortment, std::map<int, int> base);
    QString ordersToText(Orders orders);

    template <typename T>
    QString publisherOrdersToText(T orders, auto bookBase);
    QString specialOrdersToText(std::vector<std::pair<int, OrderPtr>> orders);

    void printStepInfo(int step);
    void printStatInfo(StatisticInfo info);

    void updateOrdersAfterDay(System& system_);


private slots:
    void on_startButton_clicked();

    void on_stepButton_clicked();

    void on_skipButton_clicked();

    void on_exitButton_clicked();

private:
    Ui::MainWindow *ui;
    System system_;
};
#endif // MAINWINDOW_H
