#include "txtcontroller_konfiguration.h"

TXTKonfiguration::TXTKonfiguration()
{
}



TXTKonfiguration &TXTKonfiguration::setzeEingangsmodus(int eingangsnummer, TXTKonfiguration::EINGANGSMODUS modus)
{
    switch(modus) {
    case DIGITAL:
        packet.m_config.universalInputs[eingangsnummer].mode = InputMode::MODE_R;
        packet.m_config.universalInputs[eingangsnummer].digital = true;
        break;
    case ANALOG:
        packet.m_config.universalInputs[eingangsnummer].mode =  InputMode::MODE_R;
        packet.m_config.universalInputs[eingangsnummer].digital = false;
      break;
    case SPANNUNG:
        packet.m_config.universalInputs[eingangsnummer].mode =  InputMode::MODE_U;
        packet.m_config.universalInputs[eingangsnummer].digital = false;
      break;
   case ENTFERNUNG:
        packet.m_config.universalInputs[eingangsnummer].mode = InputMode::MODE_ULTRASONIC;
        packet.m_config.universalInputs[eingangsnummer].digital = false;
        break;
    }
    return *this;
}

TXTKonfiguration &TXTKonfiguration::setzeAusgangsmodus(int motornummer, TXTKonfiguration::AUSGANGSMODUS modus)
{
    switch(modus) {
     case MOTOR:
        packet.m_config.motor[motornummer] = 0;
        break;
      case ZWEI_AUSGAENGE:
        packet.m_config.motor[motornummer] = 1;
        break;
    }
    return *this;
}



/*function TTXTController_Konfiguration.SetzeEingangsmodus
  (Eingangsnummer: Integer; Modus: TEingangsmodus): TTXTController_Konfiguration;
begin
  Dec(Eingangsnummer);

  lock.AcquireExclusive;
  case Modus of
    DIGITAL:
    begin
      paket.m_config.uni[Eingangsnummer].mode := Byte(InputMode.MODE_R);
      paket.m_config.uni[Eingangsnummer].digital := True;
    end;
    ANALOG:
    begin
      paket.m_config.uni[Eingangsnummer].mode := Byte(InputMode.MODE_R);
      paket.m_config.uni[Eingangsnummer].digital := False;
    end;
    SPANNUNG:
    begin
      paket.m_config.uni[Eingangsnummer].mode := Byte(InputMode.MODE_U);
      paket.m_config.uni[Eingangsnummer].digital := False;
    end;
    ENTFERNUNG:
    begin
      paket.m_config.uni[Eingangsnummer].mode := Byte(InputMode.MODE_ULTRASONIC);
      paket.m_config.uni[Eingangsnummer].digital := False;
    end;
  end;
  lock.ReleaseExclusive;

  Result := Self;
end;

function TTXTController_Konfiguration.SetzeAusgangsmodus
  (Motornummer: Integer; Modus: TAusgangsmodus): TTXTController_Konfiguration;
begin
// ver�ndert zum suchen
  Dec(Motornummer);

  lock.AcquireExclusive;
  case Modus of
    MOTOR:
      paket.m_config.motor[Motornummer] := 0;
    ZWEI_AUSGAENGE:
      paket.m_config.motor[Motornummer] := 1;
  end;
  lock.ReleaseExclusive;

  Result := Self;
end;*/
