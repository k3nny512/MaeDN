#include "txtcontroller.h"
#include <string.h>

TXTController::TXTController():last_sound_command_id(0)
{
    for(int i = 0;i<ftIF2013_nMotorOutputs;i+=1) {
        last_motor_command_id[i] = 0;
    }
}

TXTController::TXTController(QString p_ip,TXTKonfiguration konfig)
{
    verbinden(p_ip,konfig);
}

TXTController::~TXTController()
{
    if(tcpClient.isOpen()) {
        ftIF2013Response_StopOnline startOnlineResponse;
        ftIF2013Command_StopOnline startOnlineCommand;
        sendCommand(&startOnlineCommand,sizeof (startOnlineCommand),
                    &startOnlineResponse,sizeof (startOnlineResponse));
        tcpClient.close();
    }
}

bool TXTController::verbinden(QString p_ip,TXTKonfiguration konfig,int timeout)
{
    ip = p_ip;
    tcpClient.connectToHost(ip, 65000);
    if(tcpClient.waitForConnected(timeout)) {
        connected =  true;
        ftIF2013Response_StartOnline startOnlineResponse;
        ftIF2013Command_StartOnline startOnlineCommand;
        sendCommand(&startOnlineCommand,sizeof (startOnlineCommand),
                    &startOnlineResponse,sizeof (startOnlineResponse));

        ftIF2013Response_UpdateConfig res;
        bool re = sendCommand(&konfig.packet,sizeof(konfig.packet),
                              &res,sizeof(res));
        if(re)
            connected =  true;
        else {
            tcpClient.close();
            connected =  false;
        }
    } else {
        connected = false;
    }
    return connected;
}

bool TXTController::trennen()
{

}

bool TXTController::istVerbunden()
{
    return connected;
}

int TXTController::liesCounter(int counternummer)
{
    QMutexLocker lock(&packetResponseMtx);
    return lastExchangeDataResponse.m_counter_value[counternummer - 1];
}

bool TXTController::liesDigital(int eingangsnummer)
{
    QMutexLocker lock(&packetResponseMtx);
    return lastExchangeDataResponse.m_universalInputs[eingangsnummer - 1] != 0;
}

int TXTController::liesAnalog(int eingangsnummer)
{
    QMutexLocker lock(&packetResponseMtx);
    return lastExchangeDataResponse.m_universalInputs[eingangsnummer - 1];
}

bool TXTController::sendeBefehl()
{
    packetMtx.lock();
    auto packetCpy = packet;
    packetMtx.unlock();

    packetResponseMtx.lock();
    auto packetResponseCpy = lastExchangeDataResponse;
    packetResponseMtx.unlock();
    bool res = sendCommand(&packetCpy,sizeof(packet),
                           &packetResponseCpy,
                           sizeof(lastExchangeDataResponse));


    packetResponseMtx.lock();
    lastExchangeDataResponse = packetResponseCpy;
    packetResponseMtx.unlock();


    packetMtx.lock();
    for(int i = 0;i<ftIF2013_nMotorOutputs;i+=1) {
        last_motor_command_id[i] = packet.m_motor_command_id[i];
    }
    last_sound_command_id = packet.m_sound_command_id;
    packetMtx.unlock();

    return res;
}




bool TXTController::sendCommand(const ftIF2013Command_Base *commanddata,
                                size_t commandsize,
                                ftIF2013Response_Base *responsedata,
                                size_t responsesize)
{
    if(!connected) {
        qDebug() << "Cant send command while not connected " <<commanddata->m_id;
        return false;
    }
    auto result = tcpClient.write(reinterpret_cast<const char*>(commanddata), int64_t(commandsize));
    tcpClient.waitForBytesWritten(2000);

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
        tcpClient.waitForReadyRead(10000);
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

// Get Interface Version
uint32_t TXTController::getTXTVersion()
{
    // Send a query status command
    ftIF2013Command_QueryStatus command;
    ftIF2013Response_QueryStatus response;
    if( !sendCommand( &command, sizeof(command), &response, sizeof(response) ) )
    {
        return 0;
    }
    deviceType = QString(response.m_devicename);
    txtVersion = response.m_version;

    return txtVersion;
}


TXTController &TXTController::setzeMotorgeschwindigkeit(uint16_t motornummer, int geschwindigkeit)
{
    QMutexLocker lock(&packetMtx);
    packet.m_pwmOutputValues[motornummer * 2 - 2] = int16_t(std::max(0, geschwindigkeit));
    packet.m_pwmOutputValues[motornummer * 2 - 1] = int16_t(std::max(0, -geschwindigkeit));
    return *this;
}

TXTController &TXTController::setzeMotorschrittzahl(uint16_t motornummer, int schrittzahl)
{
    QMutexLocker lock(&packetMtx);
    packet.m_motor_distance[motornummer-1] = int16_t(schrittzahl);
    packet.m_motor_command_id[motornummer-1] = last_motor_command_id[motornummer-1]+1;
    return *this;
}

TXTController &TXTController::setzeMotorsynchronisation(uint16_t motornummer1, uint16_t motornummer2)
{
    QMutexLocker lock(&packetMtx);
    packet.m_motor_master[motornummer1-1] = int16_t(motornummer2);
    packet.m_motor_command_id[motornummer1-1] = last_motor_command_id[motornummer1-1]+1;
    return *this;
}

TXTController &TXTController::hebeMotorsynchronisationAuf(uint16_t motornummer)
{
    QMutexLocker lock(&packetMtx);
    packet.m_motor_master[motornummer-1] = 0;
    packet.m_motor_command_id[motornummer-1] = last_motor_command_id[motornummer-1]+1;
    return *this;
}

TXTController &TXTController::setzeAusgang(uint16_t ausgangsnummer, uint16_t pwmWert)
{
    QMutexLocker lock(&packetMtx);
    packet.m_pwmOutputValues[ausgangsnummer-1] = int16_t(pwmWert);
    return *this;
}

TXTController &TXTController::setzeGeraeusch(uint16_t geraeschID, uint16_t wiederholungen)
{
    QMutexLocker lock(&packetMtx);
    packet.m_sound_index = uint16_t(geraeschID);
    packet.m_sound_repeat = uint16_t(wiederholungen);
    packet.m_sound_command_id = last_sound_command_id+1;
    return *this;
}

