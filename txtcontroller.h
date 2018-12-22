/**
 * Diese Datei enthält die Klasse TXTController.
 *
 * Autoren: Nils Schulte, Steffen Vollenkemper
 *
 * Datum: 05.12.2018
 *
 * Copyright © 2018,2019
 * FH Münster
 * Fachbereich Maschinenbau
 * Labor für Systemanalyse und Optimierung
 **/
//#pragma once
#ifndef  TXTCONTROLLER_H
#define  TXTCONTROLLER_H
#include <string>
#include <QTcpSocket>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include "packets.h"
#include "txtcontroller_konfiguration.h"
#include "txtcontroller_steuerbefehl.h"

#include <QMutex>

const int ALLE_KANAELE = 4; // Empfängt die Signale von allen Fernbedienungen.
const bool ACTIVATE_CODESITE = false;

/*
/// Die Modi in die Eingänge versetzt werden können.
enum Eingangsmodus{
    /// Der Eingang liefert die Spannung am Eingang als Integer im Intervall 0-1023.
    /// Dies ist die Standardeinstellung.
    SPANNUNG,
    /// Der Eingang liefert den Wert 1, wenn beide Kontakte leitend verbunden
    /// sind (z.B. Taster gedrückt), sonst 0.
    DIGITAL,
    /// Der Eingang liefert den Widerstand zwischen den beiden Kontakten
    /// des Eingangs als Integer im Intervall 0-1023. (z.B. Photowiderstand)
    ANALOG,
    /// Der Eingang liefert den Abstand in cm zu dem nächsten Objekt (Ultraschallsensor).
    ENTFERNUNG
};

/// Die Modi, in die Ausgänge versetzt werden können.
enum Ausgangsmodus{
    /// Zwischen den beiden Anschlüssen kann ein PWM Signal zwischen -512 und 512
    /// ausgegeben werden. Dies ist die Standardeinstellung.
    MOTOR,
    /// Die beiden Anschlüsse können unabhängig voneinander angesteuert werden.
    ZWEI_AUSGAENGE
};*/

/**  Die Grundklasse zur Steuerung des TXT-Controllers.
 *  Diese Klasse stellt die Funktionen des TXT-Protokolls zur Verfügung.
 *
 *  !! Achtung: Es ist zu beachten, dass alle Funktionen, die auf Eingänge, Zähler,
 *  Fernbedienungsdaten oder ähnliches zugreifen, immer die Werte liefern, die der
 *  Controller bei dem letzten Aufruf von LiesSensordaten oder Steuerbefehl.Senden
 *  gesendet hat. !!
 */

class TXTController
{
public:
    TXTController();
    /// Erzeugt eine Verbindung zu einem Controller.
    /// @param IP_Adresse Die IP-Adresse des Controllers
    TXTController(QString ip,TXTKonfiguration konfig);

    /// Trennt alle Verbindungen zu dem Controller und gibt das Objekt frei.
    ~TXTController();

    /**
     * @brief verbinden Baut eine TCP-Verbindung zum Controller auf, wechselt in den Online-Modus und sendet die Konfig.
     * @param ip die Ip-Adresse
     * @param konfig die Konfiguration
     * @param timeout das Timout in millisekunden
     * @return
     */
    bool verbinden(QString ip, TXTKonfiguration konfig, int timeout = 2000);

    /// Beendet den Online-Modus und trennt die TCP-Verbindung.
    bool trennen();

    /// True, wenn eine Verbindung zu dem Controller besteht.
    /// Dies ist gegeben falls Verbinden aufgerufen wurde und weder Trennen
    /// aufgerufen wurde, noch ein Verbindungsabbruch aufgetreten ist.
    bool istVerbunden();

    /// Empfängt und speichert die Daten aller Eingänge, Counter, etc.
    /// Ist äquivalent zu meinController.Steuerbefehl.Senden;
    /// (erneutes Senden des letzten Steuerpakets).
    /// Gibt bei Ergfolg true zurück.
    bool liesSensordaten();

    /// Gibt den Wert des Counters C<Counternummer> zurück.
    int liesCounter(int counternummer);

    /// Gibt den Zustand des digitalen Eingangs I<Eingangsnummer> zurück.
    /// Der Modus der Eingänge wird durch SendeKonfig gesetzt.
    bool liesDigital(int eingangsnummer);

    /// Gibt den Zustand des analogen Eingangs I<Eingangsnummer> zurück.
    /// Der Modus der Eingänge wird durch SendeKonfig gesetzt.
    int liesAnalog(int eingangsnummer);

    bool sendeBefehl();

    /// Setzt die Geschwindigkeit des Motors M<Motornummer>.
    /// @param Geschwindigkeit Motorgeschwindigkeit im Bereich von -512 bis +512.
    TXTController& setzeMotorgeschwindigkeit(uint16_t motornummer, int geschwindigkeit);

    /// Bewegt den Motor M<Motornummer> um die angegebene Schrittzahl.
    /// Wenn Schrittzahl gleich 0 ist, wird der Motor bewegt, bis der nächste
    /// Befehl für diesen Motor geschickt wird.
    /// (kann zum Beispiel für Regelkreise benutzt werden)
    TXTController& setzeMotorschrittzahl(uint16_t motornummer, int schrittzahl);

    /// Synchronisiert Motor M<Motornummer1> zu Motor M<Motornummer2>.
    TXTController& setzeMotorsynchronisation(uint16_t motornummer1, uint16_t motornummer2);

    /// Macht die Synchronisation des Motors M<Motornummer> rückgängig.
    /// Als Motornummer muss die Nummer übergeben werden, die bei
    /// SetzeMotorsynchronisation als erster Parameter übergeben wurde.
    TXTController& hebeMotorsynchronisationAuf(uint16_t motornummer);

    /// Setzt den PWM-Wert von Ausgang O<Ausgangsnummer>.
    /// @param pwmWert St‰rke des Signals im Bereich von 0 bis 512.
    TXTController& setzeAusgang(uint16_t ausgangsnummer, uint16_t pwmWert);

    /// Spielt das Geräusch mit der angebenen Nummer <Wiederholungen>-mal ab.
    /// Falls Wiederholungen gleich 0 ist wird das Ger‰usch wiederholt, bis
    /// ein neuer Befehl für ein Geräusch gesendet wird.
    TXTController& setzeGeraeusch(uint16_t geraeschID,uint16_t wiederholungen);

private:
    QMutex packetMtx;
    ftIF2013Command_ExchangeData packet;
    int16_t last_motor_command_id[ftIF2013_nMotorOutputs];
    uint16_t last_sound_command_id;


    QMutex packetResponseMtx;
    ftIF2013Response_ExchangeData lastExchangeDataResponse;


    uint32_t getTXTVersion();
    QString deviceType;
    uint32_t txtVersion;

    QString ip;
    QTcpSocket tcpClient;
    bool connected;

    bool sendCommand(const struct ftIF2013Command_Base *commanddata,
                     size_t commandsize,
                     ftIF2013Response_Base *responsedata,
                     size_t responsesize );
};

#endif // TXTCONTROLLER_H
