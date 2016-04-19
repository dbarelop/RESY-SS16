# Softwarearchitektur

* Betriebssystem: Raspbian Lite
* Der Roboter wird mit 4 Tasks laufen:
    *  Ultraschall
    *  LED's
    *  RFID
    *  Steuerung Motor
* Der Notausbutton wird mit Interrupt umgesetzt
* Als Scheduler wird EDF verwendet (Earliest Deadline First)
## Rechnerkernauteilung:

| Name | CPU Auslastung | Prozesszeit [ms] | Deadline |
| --- |:---:|:---:| --- |
| Ultraschall | 20% | 50 | 250 |
| LED's | 10% | 10 | 100 |
| RFID | 20% | 20 | 100 |
|Motorsteuerung | 20% | aperiodisch ||
| Linux (geschätzt) | 10% | x | x |

![Mit RR](https://burns.in.htwg-konstanz.de/labworks-RESY_SS16/resy_ss16_1/blob/develop/mit_RR.eps)