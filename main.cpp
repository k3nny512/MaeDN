#include <iostream>
#include "controller.h"
#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QThread>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
/*
    Controller controller;
    Motor mTurn(1,1);
    Motor mHeight(2,2);
    Motor mRad(3,3);

    Sensor sTurn(7);
    Sensor sHeight(6);

    controller
            .addMotor(mTurn)
            .addMotor(mHeight)
            .addMotor(mRad)
            .addSensor(sTurn)
            .addSensor(sHeight);

    if(!controller.connect("192.168.0.31")) {
        qDebug() << "error, cant connect";
        return 0;
    } else {
        qDebug() << "connected";
    }

    //QThread::msleep(1000);

    QTimer::singleShot(100, [&]()
    {
        //Setze die Motorgeschwindigkeiten und die Schrittzahl;
        uint16_t steps = 500;
        int16_t speed = 400;
        mHeight.setSpeed(speed);
        mHeight.turnSteps(steps);




        m1.setSpeed(speed);
        m2.setSpeed(speed);
        m1.syncTo(m2);
        m1.turnSteps(steps);
        m1.getStepsLeft()
        m2.turnSteps(steps);
        s7.readAnalog();

        

    } );

     QTimer::singleShot(2000, [&]() {
         //Breche das Programm ab
         a.exit();
     });

     */
    return a.exec();
}
