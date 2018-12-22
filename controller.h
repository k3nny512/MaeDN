#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include <QList>
#include <QTcpSocket>
#include <QTimer>
#include <QMutex>
#include "packets.h"

class Sensor:public ConfigPackageWriter,public CommandPackageReader {
public:
    enum EINGANGSMODUS{
        /// Der Eingang liefert die Spannung am Eingang als Integer im Intervall 0-1023.
        /// Dies ist die Standardeinstellung.
        SPANNUNG,
        /// Der Eingang liefert den Wert 1, wenn beide Kontakte leitend verbunden
        /// sind (z.B. Taster gedr¸ckt), sonst 0.
        DIGITAL,
        /// Der Eingang liefert den Widerstand zwischen den beiden Kontakten
        /// des Eingangs als Integer im Intervall 0-1023. (z.B. Photowiderstand)
        ANALOG,
        /// Der Eingang liefert den Abstand in cm zu dem nächsten Objekt (Ultraschallsensor).
        ENTFERNUNG
    };
protected:
    int value;
    const int port;
    EINGANGSMODUS modus;
    friend class Controller;

    QMutex mtx;
    void writeToConfigPackage(ftIF2013Command_UpdateConfig&) override;
    void readFromPackage(ftIF2013Response_ExchangeData&) override;

public:
    Sensor(int port,EINGANGSMODUS modus = ENTFERNUNG);

    bool readDigital();

    int readAnalog();
};

class Motor:public ConfigPackageWriter, public CommandPackageWriter, public CommandPackageReader
{
private:
    const int portMotor;
    const int portCounter;
    bool needsUpdateMotorCommand = true;
    int16_t lastCommandId = 0;
    Motor* synctTo = nullptr;
    uint16_t stepsTo = 0;
    int16_t stepsCurrent = 0;
    int16_t speed = 0;
    QMutex mtx;

    friend class Controller;
    void writeToConfigPackage(ftIF2013Command_UpdateConfig&) override;
    void writeToPackage(ftIF2013Command_ExchangeData&) override;
    void readFromPackage(ftIF2013Response_ExchangeData&) override;
public:
    Motor(int motorPortNumber,int counterPortNumber);
    ~Motor() override;

    /**
     * @brief syncTo sets up Motorsyncronisation with the specified Motor
     * @param motor
     * @return
     */
    Motor& syncTo(Motor& motor);
    Motor& setSpeed(int16_t newSpeed);
    Motor& turnSteps(uint16_t steps);

    int16_t getStepsLeft();
    int16_t getSpeed();

};

class Controller: public QObject
{ Q_OBJECT
public:
    Controller();
    ~Controller();


    /**
     * @brief addSensor adds a motor to the controller and sends the approproate configuration to the controller.
     * @param s the motor to add to the controller.
     * @return the controller object for chanining add* commands.
     */
    Controller& addMotor(Motor& m);


    /**
     * @brief addSensor adds a sensor to the controller and sends the approproate configuration to the controller.
     * @param s the sensor to add to the controller.
     * @return the controller object for chanining add* commands.
     */
    Controller& addSensor(Sensor& s);


    /**
     * @brief connect Baut eine TCP-Verbindung zum Controller auf, wechselt in den Online-Modus und sendet die Konfig.
     * @param ip die Ip-Adresse
     * @param konfig die Konfiguration
     * @param timeout das Timout in millisekunden
     * @return
     */
    bool connect(QString ip, int timeout = 2000);
    bool disconnect();


private:
    QList<Motor*> motors;
    QList<Sensor*> sensors;

    bool needsConfigUpdate = true;
    uint16_t lastConfigId = 1;


    QMutex packetMtx;
    ftIF2013Command_ExchangeData packet;

    QMutex packetResponseMtx;
    ftIF2013Response_ExchangeData lastExchangeDataResponse;

    //uint32_t getTXTVersion();
    QString deviceType;
    uint32_t txtVersion;

    QString ip;

    QTcpSocket tcpClient;

    QTimer updateTimer;

    bool sendCommand(const struct ftIF2013Command_Base *commanddata,
                     size_t commandsize,
                     ftIF2013Response_Base *responsedata,
                     size_t responsesize);

    bool updateTXTConfig();
public slots:
    /**
     * @brief updateTXT Sends everythin to the TXT.
     * Updates the Config if nessesary.
     * @return true if sucessfull.
     */
    bool updateTXT();
};

#endif // CONTROLLER_H
