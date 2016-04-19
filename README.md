# Softwarearchitektur

* Betriebssystem: Raspbian Lite
* Der Roboter wird mit 4 Tasks laufen:
    *  Ultraschall
    *  LED's
    *  RFID
    *  Steuerung Motor
* Der Notausbutton wird mit Interrupt umgesetzt
* Als Scheduler wird EDF verwendet (Earliest Deadline First)

## Rechnerkernaufteilung:

| Name | CPU Auslastung | Prozesszeit [ms] | Deadline |
| --- |:---:|:---:| --- |
| Ultraschall | 20% | 50 | 250 |
| LED's | 10% | 10 | 100 |
| RFID | 20% | 20 | 100 |
|Motorsteuerung | 20% | aperiodisch ||
| Linux (geschätzt) | 10% | x | x |
