#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"
#include "txtcontroller.h"
#include <iostream>
#include <QCoreApplication>
#include <QThread>
#include "math.h"

Motor mTurn(1,1);
Motor mHeight(2,2);
Motor mRad(3,3);
Motor mMag(4,4);
Sensor sTurn(5);
Sensor sHeight(6);
Sensor sRad(7);




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->BTMagnetOff->setEnabled(false);

/*
    while (sHeight.readDigital() == true)
    {
        ui->TEConsole->append("Endstop Höhe");
    }
    while (sHeight.readDigital() == false)
    {
        ui->TEConsole->append("kein Endstop");

    }
*/
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_W) rIn(ui->LESteps->text().toInt()/10,(ui->LESpeed->text().toInt()));
    if (event->key() == Qt::Key_S) rOut(ui->LESteps->text().toInt()/10,-(ui->LESpeed->text().toInt()));
    if (event->key() == Qt::Key_A) TurnCCW(ui->LESteps->text().toInt(),(ui->LESpeed->text().toInt()));
    if (event->key() == Qt::Key_D) TurnCW(ui->LESteps->text().toInt(),-(ui->LESpeed->text().toInt()));
    if (event->key() == Qt::Key_G) magGrab(ui->LESteps->text().toInt(),-(ui->LESpeed->text().toInt()));
    if (event->key() == Qt::Key_PageUp) heightUp(ui->LESteps->text().toInt(),(ui->LESpeed->text().toInt()));
    if (event->key() == Qt::Key_PageDown) heightDown(ui->LESteps->text().toInt(),-(ui->LESpeed->text().toInt()));


}

void MainWindow::magGrab(uint16_t steps, int16_t speed) //klappt noch nicht
{
    int on = 1000;
    int off = 1000;
    mHeight.setSpeed(speed);
    mHeight.turnSteps(-100);

    mMag.setSpeed(on);
    mMag.turnSteps(on);

    mHeight.setSpeed(speed);
    mHeight.turnSteps(100);

    mMag.setSpeed(off);
    mMag.turnSteps(off);



}

void MainWindow::magClick(uint16_t steps, int16_t speed)
{
    controller.addMotor(mMag);
    mMag.setSpeed(speed);
    mMag.turnSteps(steps);

}

void MainWindow::TurnCW(uint16_t steps, int16_t speed)
{
    //controller.addMotor(mTurn);
    mTurn.setSpeed(speed);
    mTurn.turnSteps(steps);

}

void MainWindow::TurnCCW(uint16_t steps, int16_t speed)
{
    controller.addMotor(mTurn);
    mTurn.setSpeed(speed);
    mTurn.turnSteps(steps);

}

void MainWindow::rOut(uint16_t steps, int16_t speed)
{
    controller.addMotor(mRad);
    mRad.setSpeed(speed);
    mRad.turnSteps(steps);

}

void MainWindow::rIn(uint16_t steps, int16_t speed)
{
    controller.addMotor(mRad);
    mRad.setSpeed(speed);
    mRad.turnSteps(steps);

}

void MainWindow::heightUp(uint16_t steps, int16_t speed)
{
    controller.addMotor(mHeight);
    mHeight.setSpeed(speed);
    mHeight.turnSteps(steps);

}

void MainWindow::heightDown(uint16_t steps, int16_t speed)
{
    controller.addMotor(mHeight);
    mHeight.setSpeed(speed);
    mHeight.turnSteps(steps);

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_BTConnect_clicked()
{
    ui->TEConsole->append("Start");
    if(!controller.connect(ui->LEIP->text())) {
        ui->TEConsole->append("error, cant connect");
        //return 0;
    } else {
        ui->TEConsole->append("connected");
    }

}

void MainWindow::on_BTClose_clicked()
{
    //ui->TEConsole->append("Try to Stop");
    if(controller.disconnect()) {
        ui->TEConsole->append("error, cant disconnect");
        //return 0;
    } else {
        ui->TEConsole->append("disconnected");
        QThread::msleep(2000);
    }


}

void MainWindow::on_BTRight_clicked()
{
    //if (sHeight.readDigital() == true){

    TurnCW(ui->LESteps->text().toInt(),-(ui->LESpeed->text().toInt()));
    //}

}

void MainWindow::on_BTLeft_clicked()
{
    TurnCCW(ui->LESteps->text().toInt(),(ui->LESpeed->text().toInt()));
}

void MainWindow::on_BTMoveTo_clicked()
{
    /*


    x,y

    0,0
    -------------
    |           |
    |           |
    |           |
    |           |
    |           |
    -------------
          |
          |
          |
          O

    */
    int startx = 0;
    int starty = 0;
    int x = ui->LEXcord->text().toInt();
    int y = ui->LEYcord->text().toInt();



    int r = sqrt(x*x+y*y);      //<----- Radius
    //int c = tan^-1(y/x)       //<----- TAN??/ c ist winkel

}


void MainWindow::on_BTOut_clicked()
{
    rOut(ui->LESteps->text().toInt()/10,(ui->LESpeed->text().toInt()));
}

void MainWindow::on_BTIn_clicked()
{
    rIn(ui->LESteps->text().toInt()/10,-(ui->LESpeed->text().toInt()));
}

void MainWindow::on_BTUp_clicked()
{
    heightUp(ui->LESteps->text().toInt(),(ui->LESpeed->text().toInt()));
}

void MainWindow::on_BTDown_clicked()
{
    heightDown(ui->LESteps->text().toInt(),-(ui->LESpeed->text().toInt()));
}

void MainWindow::on_BTMagnetClicked_clicked()
{
    magClick(ui->LESteps->text().toInt(),-(ui->LESpeed->text().toInt()));
}

void MainWindow::on_BTMagnetOn_clicked()
{
    ui->BTMagnetOn->setEnabled(false);
    ui->BTMagnetOff->setEnabled(true);
    magClick(1000,1000);
}

void MainWindow::on_BTMagnetOff_clicked()
{
    ui->BTMagnetOff->setEnabled(false);
    ui->BTMagnetOn->setEnabled(true);
    magClick(0,0);

}
