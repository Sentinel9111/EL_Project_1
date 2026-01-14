# Spelregels
## Shotfield
In shotfield is het de bedoeling om binnen jouw speeltijd van 4 minuten zoveel mogelijk shotjes te vinden.
\
Het aantal stappen dat je mag zetten (1-4) word bepaald door een dobbelsteen die in het spelbord is ingebouwd.
\
Het is belangrijk dat je deze stappen vakje voor vakje doet, doe je dat niet zal het spel mogelijk niet registeren dat je op een shotje hebt gestaan.
\
Als de pieper af gaat dan heb je een shotje gevonden.
\
Zodra je al je stappen hebt gezet druk je op jouw spelers knop, nu begint de tijd van de andere speler te lopen en zal hij/zij het aantal gevonden shotjes moeten drinken.
\
De winnaar is de speler die de meeste shotjes gevonden heeft. 

## Inhoud spelbord
- 7x7 spelbord met blokkades en sensoren.
- 2 pionnen.
- 2 spelersknoppen.
- Digitale dobbelsteen (1-4).
- Display met spelerstijd en -score.

## Spelverloop
- Het spel begint met de 2 pionnen op de startvakken.
- Zodra de andere speler op zijn/haar knop drukt pauzeert hun tijd en begint jouw tijd te lopen, dan krijg je direct te zien hoeveel stappen je mag zetten.
- Nu mag je jouw pion het aantal stappen verplaatsen, dat doe je stap voor stap, laat je pion ongeveer 1 seconden staan tussen de stappen in.
- Als je een shotje hebt gevonden gaat er een buzzer af.
- Alle shotjes die je tijden deze beurt hebt gevonden worden opgeteld.
- Heb je alle stappen gezet, dan druk je op jouw spelersknop. Als je shotjes had gevonden moet de andere speler in zijn eigen spelerstijd deze shotjes drinken.
- Wanneer de tijd van een van de spelers op is dan mag je andere speler blijven dobbelen en zijn tijd op maken.
- De speler die de meeste shotjes heeft gevonden wint het spel.

---
# Hardware
Voor de hardware word gebruikt gemaakt van de volgende onderdelen:
- DOIT ESP32 devkit v1.
- 16 light-dependent resistors (LDRs) met 10kΩ weerstanden.
- CD74HC4067 16-kanaals analoge multiplexer.
- ST7735S TFT display.
- piëzo buzzer module.
- Light-emitting diode (LED) met een 200Ω weerstand.
- Breadboard en draadbruggen.

<!-- ### Schema -->

---
# Code
De broncode is te vinden in de `src` folder, om deze code te compileren moet je PlatformIO geinstalleerd hebben en de gehele repository clonen.

## Programmstructuur
### Hoofdlus (loop)
De ESP32 voert continu de volgende taken uit:
- Controleren van de LDR-sensoren voor pionbeweging.
- Bijwerken van de spelerstimers.
- Bijwerken van het display.
- Verwerken van knopindrukken.
- Afspelen van geluid en licht bij explosies.

### Toestanden (States)
Het spel kent drie toestanden:
- `IDLE` – wachten tot het spel start.
- `RUNNING` – spel is bezig.
- `GAME_OVER` – spel is afgelopen.

Afhankelijk van de toestand gedraagt het programma zich anders.

### Mijnen
- Mijnen worden willekeurig geplaatst bij de start.
- Elke mijn kan slechts één keer ontploffen.
- Het systeem detecteert een pion via de LDR-waarde.
- Als een mijn ontploft speel de piëzo een toon af en licht de LED op.

### Multiplexer
Omdat de ESP32 niet genoeg analoge ingangen heeft:
- Wordt een multiplexer gebruikt.
- De ESP32 kiest telkens één LDR.
- Leest de waarde.
- Gaat daarna door naar de volgende.

### Display
Het TFT-scherm toont:
- Timer van speler 1.
- Timer van speler 2.
- Waarde van de dobbelsteen.
- “Game Over” wanneer het spel is afgelopen.

### Reset
Wanneer de resetknop wordt ingedrukt:
- Het spel stopt.
- Timers worden teruggezet.
- Nieuwe mijnen worden willekeurig geplaatst.
- Het scherm wordt leeggemaakt.

## Flowchart
Hier is een erg versimpelde flowchart van de code.
\
<object data="/other/Flowchart.pdf" type="application/pdf" width="700px" height="700px">
    <embed src="/other/Flowchart.pdf"><a href="/other/Flowchart.pdf"></embed>
</object>