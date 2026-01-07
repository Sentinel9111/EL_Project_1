# Project 1 voor Elektrotechniek

Kopieer eerst de git repository en installeer de platformIO in CLion of VSCode.

```shell
git clone git@github.com:Sentinel9111/EL_Project_1.git
```

Nieuwe veranderingen commit je met:
```shell
git stage *
git commit -m "commit message"
git pull
git push
```
\
Broncode is te vinden in de src folder.
Voorbeelden of oude code is te vinden in de examples folder.

## Schema
Gedeeltelijk elektrisch schema, hier zijn alleen de bovenste vier LDR's aangesloten.
Er kunnen 16 LDR's aangesloten worden, op pinnen `15, 2, 4, 22, 13, 12, 14, 27, 26, 25, 33, 32, 35, 34, 39(VN), 36(VP)`.
\
Pin 22 is geen ADC pin en mag eigenlijk niet gebruikt worden. Er moet nog een multiplexer toegevoegd worden om dit probleem op te lossen en pinnen vrij te maken.
<img src="/schematics/ELProject1.png">