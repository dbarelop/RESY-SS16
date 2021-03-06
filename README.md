# Softwarearchitektur

* Betriebssystem: Raspbian Lite
* Der Roboter wird mit 4 Tasks laufen:
    *  Ultraschall
    *  LED's
    *  RFID
    *  Steuerung Motor
    * Logik
* Der Notausbutton wird mit Interrupt umgesetzt
* Als Scheduler wird EDF verwendet (Earliest Deadline First)
* Informationen werden über shared-memory ausgetauscht.
* Sensoren geben Informationen an Logiktask.
* Logiktask wertet die Informationen aus und steuert dementsprechend den Motor.

## Rechnerkernaufteilung:

| Name | CPU Auslastung | Prozesszeit [ms] | Deadline |
| --- |:---:|:---:| --- |
| Ultraschall | 20% | 50 | 250 |
| LED's | 10% | 10 | 100 |
| RFID | 20% | 20 | 100 |
|Motorsteuerung | 20% | 10 | 50 |
| Linux (geschätzt) | 10% | x | x |


## Mitschrieb nach erster Vorstellung
* Alle Sensordaten und Motorsteuerung getrennt von einer Main-Task
* Datenflussdiagramm erstellen
