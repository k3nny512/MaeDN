/*----------------------------------------
 Diese Datei enthält die Klasse TXTController_Steuerbefehl.

 Autoren: Nils Schulte, Steffen Vollenkemper

 Datum: 05.12.2018

 Copyright © 2018,2019
 FH Münster
 Fachbereich Maschinenbau
 Labor für Systemanalyse und Optimierung
------------------------------------------ */

#pragma once
//ifndef TXTCONTROLLER_STEUERBEFEHL_H
#define TXTCONTROLLER_STEUERBEFEHL_H
#include "packets.h"
#include <stdint.h>


/*
 *  Ermöglicht es Steuerbefehle an den Controller in einem "fluent interface"
 *  zu schicken.
 *  Es müssen nur die Befehle angegeben werden, die gegenüber dem letzten
 *  Aufruf geändert werden sollen.
 *
 *  Beispielcode:
 *
 *  @longCode
 *    MeinController.Steuerbefehl
 *      .SetzeMotorsynchronisation(1, 2) // M1 wird zu M2 synchronisiert.
 *      .SetzeMotorgeschwindigkeit(1, 512) // M1 fährt mit voller Geschwindigkeit vorwärts.
 *      .SetzeMotorgeschwindigkeit(2, 512) // M2 fährt mit voller Geschwindigkeit vorwärts.
 *      .SetzeMotorgeschwindigkeit(3, -256) // M3 fährt mit halber Geschwindigkeit rückwärts.
 *      .SetzeMotorschrittzahl(3, 100) // M3 hört nach 100 Schritten auf, sich zu drehen.
 *      .SetzeAusgang(5, 128) // O5 liefert ein PWM-Signal mit einem Tastgrad von 25% (128/512).
 *      .SetzeGeraeusch(2, 3) // Spielt Geräusch Nummer 2 dreimal ab.
 *      .Senden;
 */

class TXTSteuerbefehl
{
    friend class TXTController;
public:
    /// Erzeugt eine neue Konfiguration.
    /// Sollte nur im Konstruktor von TTXTController aufgerufen werden.
    TXTSteuerbefehl();


};

//#endif // TXTCONTROLLER_STEUERBEFEHL_H
