#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"
#include <QMainWindow>
#include <QApplication>
#include <QCoreApplication>
#include <QThread>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Controller controller;



private slots:

    void on_BTConnect_clicked();

    void on_BTClose_clicked();

    void on_BTRight_clicked();

    void on_BTLeft_clicked();

    void on_BTMoveTo_clicked();

    void on_BTOut_clicked();

    void on_BTIn_clicked();

    void on_BTUp_clicked();

    void on_BTDown_clicked();

    void on_BTMagnetClicked_clicked();

    void on_BTMagnetOn_clicked();

    void on_BTMagnetOff_clicked();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    void TurnCW(uint16_t steps, int16_t speed);
    void TurnCCW(uint16_t steps, int16_t speed);
    void rOut(uint16_t steps, int16_t speed);
    void rIn(uint16_t steps, int16_t speed);
    void heightUp(uint16_t steps, int16_t speed);
    void heightDown(uint16_t steps, int16_t speed);
    void magClick(uint16_t steps, int16_t speed);
    void magGrab(uint16_t steps, int16_t speed);


};

#endif // MAINWINDOW_H
