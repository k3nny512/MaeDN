#include "controller.h"
#include <algorithm>
#include <QThread>

Controller::Controller():
    updateTimer(this)
{
//m_thread = new QThread(this);
//QTimer* timer = new QTimer(0); // _not_ this!
//timer->setInterval(1);
//timer->moveToThread(m_thread);
// Use a direct connection to whoever does the work in order
// to make sure that doIt() is called from m_thread.
//worker->connect(timer, SIGNAL(timeout()), SLOT(doIt()), Qt::DirectConnection);
// Make sure the timer gets started from m_thread.
//timer->connect(m_thread, SIGNAL(started()), SLOT(start()));
//m_thread->start();
}

Controller::~Controller()
{
    if(tcpClient.isOpen()) {
        ftIF2013Response_StopOnline startOnlineResponse;
        ftIF2013Command_StopOnline startOnlineCommand;
        sendCommand(&startOnlineCommand,sizeof (startOnlineCommand),
                    &startOnlineResponse,sizeof (startOnlineResponse));
        tcpClient.close();
    }
    updateTimer.stop();
    //updateThread.exit();
    for(Motor* m:motors)
        if(m) delete m;
}

Controller& Controller::addMotor(Motor& m)
{
    needsConfigUpdate = true;
    motors.append(&m);
    return *this;
}

Controller &Controller::addSensor(Sensor& s)
{
    needsConfigUpdate = true;
    sensors.append(&s);
    return *this;
}

bool Controller::disconnect()
{
    tcpClient.close();
}

bool Controller::connect(QString p_ip, int timeout)
{
    ip = p_ip;
    tcpClient.connectToHost(ip, 65000);
    QThread::msleep(50);
    if(tcpClient.waitForConnected(timeout)
            && tcpClient.state() ==
            QAbstractSocket::ConnectedState) {

        ftIF2013Response_StartOnline startOnlineResponse;
        ftIF2013Command_StartOnline startOnlineCommand;

        /*bool re = */sendCommand(&startOnlineCommand,sizeof (startOnlineCommand),
                    &startOnlineResponse,sizeof (startOnlineResponse));

        QThread::msleep(150);
        //updateTXTConfig();
        //needsConfigUpdate = true;
        //updateTXTConfig();
    }

    if(tcpClient.state() == QAbstractSocket::ConnectedState){
        updateTimer.setInterval(50);
        QObject::connect(&updateTimer, &QTimer::timeout, this, &Controller::updateTXT);
        updateTimer.start();
        //updateThread.start();
        return true;
    }
    return false;
}

bool Controller::updateTXT()
{
    if( tcpClient.state() != QAbstractSocket::ConnectedState)
    {
        tcpClient.close();

        return false;
    }

    if(needsConfigUpdate) {
        updateTXTConfig();
        QThread::msleep(50);
    }

    packetMtx.lock();
    for(Motor* m:motors){
        m->writeToPackage(packet);
    }
    /*for(Sensor* s:sensors){
        s->writeToPackage(packet);
    }*/
    auto packetCpy = packet;
    packetMtx.unlock();

    packetResponseMtx.lock();
    auto packetResponseCpy = lastExchangeDataResponse;
    packetResponseMtx.unlock();
    bool res = sendCommand(&packetCpy,sizeof(packet),
                           &packetResponseCpy,
                           sizeof(lastExchangeDataResponse));


    for(Sensor* s:sensors){
        s->readFromPackage(packetResponseCpy);
    }

    for(Motor* m:motors){
        m->readFromPackage(packetResponseCpy);
    }

    packetResponseMtx.lock();
    lastExchangeDataResponse = packetResponseCpy;
    packetResponseMtx.unlock();

    /*for(Sensor* s:sensors) {
        qDebug()<<"s"<<s->port<<" "<< s->readAnalog();
    }
    for(Motor* s:motors) {
        qDebug()<<"m" <<s->portMotor<<" "<< s->getStepsLeft();
    }*/
    return res;
}

bool Controller::updateTXTConfig()
{
    if(needsConfigUpdate) {
        needsConfigUpdate = false;
        lastConfigId +=1;
    }

    ftIF2013Command_UpdateConfig configPacket;
    configPacket.m_config_id = int16_t(lastConfigId);

    for(Motor* m:motors){
        m->writeToConfigPackage(configPacket);
    }
    for(Sensor* s:sensors){
        s->writeToConfigPackage(configPacket);
    }
    ftIF2013Response_UpdateConfig res;
    bool re = sendCommand(&configPacket,sizeof(configPacket),
                          &res,sizeof(ftIF2013Response_UpdateConfig));
    return re;
}

bool Controller::sendCommand(const ftIF2013Command_Base *commanddata,
                                size_t commandsize,
                                ftIF2013Response_Base *responsedata,
                                size_t responsesize)
{
    if(tcpClient.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Cant send command while not connected " <<commanddata->m_id;
        return false;
    }
    auto result = tcpClient.write(reinterpret_cast<const char*>(commanddata), int64_t(commandsize));
    tcpClient.waitForBytesWritten(1000);

    ftIF2013ResponseId expectedResponseId = responsedata->m_id;
    if (size_t(result) != commandsize)
    {
        qDebug() << "SendCommand: Error sending command " <<commanddata->m_id;
        return false;
    }

    size_t nRead=0;
    char *pos = reinterpret_cast<char*>(responsedata);
    while( nRead < responsesize )
    {
        tcpClient.waitForReadyRead(2000);
        int64_t result = tcpClient.read(pos, int64_t(responsesize-nRead));
        // result 0 means socket closed, <0 means error, otherwise #bytes received
        if( result <= 0 )
        {
            qDebug() << "SendCommand: Error receiving response " <<expectedResponseId << endl;
            return false;
        }
        nRead += uint(result);
        pos += result;
    }
    if( responsedata->m_id != expectedResponseId )
    {
        qDebug() << "SendCommand: Response ID " << responsedata->m_id << " does't match expected " << expectedResponseId << endl;
        return false;
    }
    return true;
}

void Motor::writeToConfigPackage(ftIF2013Command_UpdateConfig &packet)
{
    packet.m_config.motor[portMotor] = 0;
}

void Motor::writeToPackage(ftIF2013Command_ExchangeData& packet)
{
    QMutexLocker lock(&mtx);
    if(needsUpdateMotorCommand) {
        lastCommandId+=1;
        needsUpdateMotorCommand = false;
    }
    packet.m_motor_command_id[portMotor-1] = lastCommandId;

    packet.m_pwmOutputValues[portMotor * 2 - 2] = int16_t(std::max(0, +speed));
    packet.m_pwmOutputValues[portMotor * 2 - 1] = int16_t(std::max(0, -speed));

    packet.m_motor_distance[portMotor-1] = int16_t(stepsTo);

    if(synctTo)
        packet.m_motor_master[portMotor-1] = int16_t(synctTo->portMotor);
    else
        packet.m_motor_master[portMotor-1] = 0;
}

void Motor::readFromPackage(ftIF2013Response_ExchangeData& packet)
{
    QMutexLocker lock(&mtx);
    stepsCurrent = packet.m_counter_value[portCounter - 1];
}

Motor::Motor(int p_motorPortNumber,int p_counterPortNumber):
    portMotor(p_motorPortNumber),
    portCounter(p_counterPortNumber)
{

}

Motor::~Motor()
{

}

Motor &Motor::setSpeed(int16_t newSpeed)
{
    QMutexLocker lock(&mtx);
    speed = newSpeed;
    return *this;
}

Motor &Motor::syncTo(Motor& motor)
{
    QMutexLocker lock(&mtx);
    needsUpdateMotorCommand = true;
    if (&motor == this) {
        synctTo = nullptr;
    } else {
        synctTo = &motor;
    }
    return *this;
}

Motor &Motor::turnSteps(uint16_t steps)
{
    QMutexLocker lock(&mtx);
    needsUpdateMotorCommand = true;
    stepsCurrent = 0;
    stepsTo = uint16_t(steps);
    return *this;
}

int16_t Motor::getStepsLeft()
{
    QMutexLocker lock(&mtx);
    return stepsTo-stepsCurrent;
}

int16_t Motor::getSpeed()
{
    QMutexLocker lock(&mtx);
    return speed;
}


ConfigPackageWriter::~ConfigPackageWriter()
{

}

CommandPackageWriter::~CommandPackageWriter()
{

}

CommandPackageReader::~CommandPackageReader()
{

}

Sensor::Sensor(int p_port, Sensor::EINGANGSMODUS p_modus):port(p_port),modus(p_modus)
{
}

void Sensor::writeToConfigPackage(ftIF2013Command_UpdateConfig &packet)
{
    switch(modus) {
        case DIGITAL:
            packet.m_config.universalInputs[port-1].mode = InputMode::MODE_R;
            packet.m_config.universalInputs[port-1].digital = true;
            break;
        case ANALOG:
            packet.m_config.universalInputs[port-1].mode =  InputMode::MODE_R;
            packet.m_config.universalInputs[port-1].digital = false;
          break;
        case SPANNUNG:
            packet.m_config.universalInputs[port-1].mode =  InputMode::MODE_U;
            packet.m_config.universalInputs[port-1].digital = false;
          break;
       case ENTFERNUNG:
            packet.m_config.universalInputs[port-1].mode = InputMode::MODE_ULTRASONIC;
            packet.m_config.universalInputs[port-1].digital = false;
            break;
    }
}

void Sensor::readFromPackage(ftIF2013Response_ExchangeData & packet)
{
    QMutexLocker lock(&mtx);
    value = packet.m_universalInputs[port - 1];
}

bool Sensor::readDigital()
{
    QMutexLocker lock(&mtx);
    return value != 0;
}

int Sensor::readAnalog()
{
    QMutexLocker lock(&mtx);
    return value;
}
