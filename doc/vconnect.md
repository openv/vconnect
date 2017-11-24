# vconnect - simple Implementierung des 300 Protokoll 

==Mein Setup== 
### Anlage 
Steuerung VitoDens HO1 ID: 20C2
Vitosol
Optolinkadapter (USB mit FTDI)

### Externer Datensammler 
Sheevaplug mit Ubuntu
## Software 
[ser2net](http://ser2net.sourceforge.net/) für die Bereitstellung eines TCP/IP Port zum seriellen Optolink Port
[vconnect](vconnect) die 300er Implementierung. Die Befehle sind noch hardcoded auf eine Anlage Type 20C2 eingestellt. Es wird nur lesender Zugriff unterstützt.
Externer Webserver mit [munin](http://munin-monitoring.org/) zur grafischen Aufbereitung. Nutzt intern das rrdtool.

Download des aktuellen SVN Status vom 26.12.2012.

[vconn_2012-12-26.tar.bz2](files/vconn_2012-12-26.tar.bz2)
Auspacken und kompilieren. Das Programm wird unter <span style="font-family: 'Courier New',Courier,monospace;">/usr/local/sbin</span> installiert.
```

tar xvfz vconn.tar.bz2
cd vConnect
make
sudo make install

```

Beispiel zum Ausslesen der Aussentemperatur und der Kesseltemperatur:
```

vConnect localhost 3000 getTempA getTempKist
getTempA.value 1.7
getTempKist.value 45

```

Da diese Werte direkt in den Munin plugins verwendet werden habe ich die .value Syntax direkt festgeschrieben.
### Konfig für ser2net 
Um wie im obigen Beispiel via TCP/IP auf Port 3000 des Hosts zugreifen zu können, wird folgende Konfig für ser2net gebraucht.
```bash

# /etc/ser2net.conf
#
# Parameter für vito RS232 4800 8E2
3000:raw:600:/dev/ttyUSB0:4800 EVEN 2STOPBITS 8DATABITS LOCAL -RTSCTS

```

==Anmerkung== 
Die Software ist auf einem Status, dass sie für meine Zwecke macht was sie soll. D.h. sie kompiliert unter Mac OS X, FreeBSD 8, Linux und Windows mit Cygwin.
Es fehlen noch etliche Features. Die Konfiguration der Adressen ist noch hardcoded im Quelltext vorgenommen, kann jedoch verallgemeinert werden. Die [Adressen](Adressen) sind nach den Tabellen hier im Wiki soweit eingetragen funktional, aber eben nur für meine Steuerung.

Es sind sicherlich noch viele Fehler in der Software und die Flexibilität lässt noch zu wünschen übrig, aber man muss ja mal einen Anfang machen.

Ich habe mit vcontrold meine Anlage ebenfalls auslesen können, jedoch aufgrund nicht nachvollziehbarer Umstände immer wieder Lesefehler erhalten, die in unsinnigen Werten resultierten. Deshalb ist die Fehlerabsicherung des 300 Protokoll für mich wesentlicher Grund gewesen eine Implementierung vorzunehmen. Die erhöhte Geschwindigkeit gegenüber dem KW Protokoll ist ein angenehmer Nebeneffekt.

## Beispiel Munin 
![vito-day-5.png](files/vito-day-5.png) ![vito-week-1.png](files/vito-week-1.png)
Das Plugin für munin zum Erzeugen der obigen Grafiken ist in Verbindung mit vConnect ein einfaches Shell Script und sieht so aus.
```

#!/bin/sh

### Host to connect
VC_HOST=localhost
### Port as defined with ser2net
VC_PORT=3000

if [ "$1" = "autoconf" ]; then
 echo yes;
 exit 0;
fi

if [ "$1" = "config" ]; then
cat <<__EOM
graph_title Heizung Temperaturen
graph_args --lower-limit -20 --upper-limit 80
graph_category Heizung
graph_vlabel Temp in C
graph_scale no
graph_info This graph shows some of the temps
getTempA.label Aussen
getTempA.info Aussentemeratur in Grad C
getTempA.warning 25
getTempA.critical 30
getTempWWist.label Warmwasser
getTempWWist.info Warmwassertemperatur in Grad C
getTempKist.label Kessel
getTempKist.info Kesseltemperatur in Grad C
getTempKol.label Kollektor
getTempKol.info Kollektortemperatur in Grad C
getTempKol.warning 90
getTempKol.critical 120
getTempSpu.label SolarWW
getTempSpu.info Speichertemperatur unten in Grad C
getTempSpu.warning 75
getTempSpu.critical 85
__EOM
exit 0;
fi

/usr/local/sbin/vConnect $VC_HOST $VC_PORT getTempA getTempWWist getTempKist getTempKol getTempSpu
exit 0

```

Natürlich kann man auch vclient nutzen. Das Template zur Ausgabe muss dann für munin den in der config Sektion gewählten Parameter um .value und den Wert ergänzen.
```

getTempA.value -1.2

```


Kann ein Wert nicht gelesen werden, muss man U zurückgeben.
```

getTempA.value U

```


Ich werde diese Seite im Laufe der Zeit noch erweitern und nähere Anleitungen hinzufügen. In der Zwischenzeit nutzt das Forum, diskutiert darüber und fragt mich im Zweifelsfall direkt.

Gruß
Frank
