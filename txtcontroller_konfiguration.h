/*----------------------------------------
 Diese Datei enthält die Klasse TXTController_Konfiguration.

 Autoren: Nils Schulte, Steffen Vollenkemper

 Datum: 05.12.2018

 Copyright © 2018,2019
 FH Münster
 Fachbereich Maschinenbau
 Labor für Systemanalyse und Optimierung
------------------------------------------ */

//#pragma once
#ifndef TXTCONTROLLER_KONFIGURATION_H
#define TXTCONTROLLER_KONFIGURATION_H
#include "packets.h"




/* Ermöglicht die Konfiguration der Ein- und Ausgänge des Controllers in einem
 * "fluent interface".
 * Nicht angegebene Ein- oder Ausgänge behalten die letzten ihnen zugewiesenen
 * Modi bzw. die Standardmodi, falls noch keine Modi gesetzt wurden.
 * Wird Senden nicht aufgerufen, so werden die Änderungen an der Konfiguration
 * trotzdem gespeichert und können zu einem späteren Zeitpunkt an den
 * Controller gesendet werden.
 *
 * Beispielcode:
 *
 * @longCode(#
 *      MeinController.Konfiguration
 *          .SetzeEingangsmodus(2, ENTFERNUNG) // Eingang I2 als Ultraschallsensor
 *          .SetzeEingangsmodus(5, DIGITAL) // Eingang I5 z. B. als Taster
 *          .SetzeEingangsmodus(7, ANALOG) // Eingang I7 z. B. als Fotowiderstand
 *          .SetzeAusgangsmodus(1, MOTOR) // Motor M1
 *          .SetzeAusgangsmodus(2, MOTOR) // Motor M2
 *          .SetzeAusgangsmodus(4, ZWEI_AUSGÄNGE) // Ausgänge O7 und O8
 *          .Senden;
 *      #)
 */

class TXTController;

class TXTKonfiguration
{
    friend class TXTController;
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

    /// Die Modi, in die Ausgänge versetzt werden können.
    enum AUSGANGSMODUS{
        /// Zwischen den beiden Anschlüssen kann ein PWM Signal zwischen -512 und 512
        /// ausgegeben werden. Dies ist die Standardeinstellung.
        MOTOR,
        /// Die beiden Anschlüsse können unabhängig voneinander angesteuert werden.
        ZWEI_AUSGAENGE
    };
    /// Erzeugt eine neue Konfiguration.
    TXTKonfiguration();

    /// Setzt den Modus des Eingangs I<Eingangsnummer>.
    /// Für eine Übersicht möglicher Modi siehe Eingangsmodus.
    /// @returns Gibt das Konfigurationsobjekt selbst zurück.
    TXTKonfiguration& setzeEingangsmodus(int eingangsnummer, EINGANGSMODUS modus);

    /// Setzt den Modus des Motors M<Motornummer> bzw. der Ausgänge
    /// O<Motornummer*2-1> und O<Motornummer*2>.
    /// Für eine Übersicht möglicher Modi siehe Ausgangsmodus.
    /// @returns Gibt das Konfigurationsobjekt  selbst zurück.
    TXTKonfiguration& setzeAusgangsmodus(int motornummer, AUSGANGSMODUS modus);

private:
    /// Der Controller zu dem diese Konfiguration gehört.
    //TXTController roboter;

    /// Das Netzwerkpaket, das an den Controller geschickt wird.
    ftIF2013Command_UpdateConfig packet;

    /// Ein Lock, das den Zugriff auf paket regelt.
    //TSRWLock lock;

};

#endif // TXTCONTROLLER_KONFIGURATION_H
