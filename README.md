# GKiM_project
program converting bitmap to our own graphical ext. and vice versa using RLE/ByteRun compression



## Dokumentacja projektu

```
Projekt własnego formatu graficznego oraz dedykowanej aplikacji do obsługi tego formatu w ramach
przedmiotu Grafika Komputerowa i Multimedia
```
- Autorzy Spis treści
- Tematyka projektu
- Specyfikacja autorskiego formatu graficznego
   - Rozwinięcie
   - Paleta
   - Paleta narzucona
   - Paleta odcieni szarości
   - Paleta dedykowana
   - Używana kompresja
   - Dithering
   - Podsumowanie
- Instrukcja użytkowania aplikacji
   - Przykładowa obsługa aplikacji


## Autorzy

Zespół projektowy nr.4 :

```
- Baran Mikołaj
- Czuba Łukasz
- Dyrek Kacper
- Jurek Wiktor
```
## Tematyka projektu

Projekt grupowy polegający na stworzeniu własnego formatu graficznego. Dostępne palety barwy:

```
- narzucona paleta 16 kolorów
- dedykowana paleta 16 kolorów
- skala szarości – 16 odcieni szarości
```
Ilość barw wynika z ograniczenia ilości bitów na kolor do 4. Plik graficzny powinien zawierać kompresję
bezstratną wykonaną według algorytmu ByteRun bądź RLE w zależności, który z nich osiągnie lepszy
stopień kompresji. Dodatkową opcją jest dithering, który może zostać dodany do zapisu w autorskim
formacie.

## Specyfikacja autorskiego formatu graficznego

(^) **Rozmiar danych Opis danych
podpis** 4 bajty ‘BCDJ’
**wysokość** 10 bitów maksymalna wysokość bitmapy 1024px
**szerokość** 10 bitów maksymalna szerokość bitmapy 1024px
**paleta** 2 bity 00 - paleta narzucona,
01 - odcienie szarości,
10 - paleta dedykowana
**używana kompresja** 1 bit RLE lub ByteRun
**dithering** 1 bit zastosowano bądź nie
**dedykowana paleta** 48 bajtów jeżeli paleta = 10, to przesyłamy paletę dedykowaną


### Rozwinięcie

Wysokość i szerokość zapisane są na 10 bitach, informacje o palecie na kolejnych dwóch bitach, natomiast
ostatnie 2 bity są odpowiedni dla danych na temat algorytmu kompresji oraz ditheringu. Łącznie te dane
zajmują 24 bity co daje nam ciąg długości 3 bajtów w których informacje ustawione są w wyżej podanej
kolejności, aby je odczytać należy wziąć pod uwagę cały ciąg 24 bitów i z odpowiednich pozycji odczytać
wartości dla poszczególnych zmiennych.

### Paleta

00 – paleta narzucona – paleta 16 kolorów uzyskanych z 4 bitów danych( 1 bit dla składowej
czerwonej, 2 bity dla składowej zielonej oraz 1 bit dla składowej niebieskiej)

01 – paleta kolorów zawiera 16 odcieni szarości – wersja 4 bitowa

10 – paleta dedykowana – program wybiera 16 kolorów najczęściej występującym na mapie
bitowej, a następnie zapisuje je do nowego formatu graficznego, aby możliwe było prawidłowe
odkodowanie pliku do formy bitmapy

### Paleta narzucona

```
Kolor reprezentacja RGB(binarna) reprezentacja RGB(dziesiętna)
```
```
0000 0 , 0 , 0
```
```
0001 0 , 0 , 255
```
```
0010 0 , 85 , 0
```
```
0011 0 , 85 , 255
```
```
0100 0 , 170 , 0
```
```
0101 0 , 170 , 255
```
```
0110 0 , 255 , 0
```
```
0111 0 , 255 , 255
```
```
1000 255 , 0 , 0
```
```
1001 255 , 0 , 255
```
```
1010 255 , 85 , 0
```
```
1011 255 , 85 , 255
```
```
1100 255 , 170 , 0
```
```
1101 255 , 170 , 255
```
```
1110 255 , 255 , 0
```
```
1111 255 , 255 , 255
```

### Paleta odcieni szarości

```
Kolor reprezentacja RGB (binarna) reprezentacja RGB (dziesiętna)
```
```
0000 0,0,
```
```
0001 17,17,
```
```
0010 34,34,
```
```
0011 51,51,
```
```
0100 68,68,
```
```
0101 85,85,
```
```
0110 102,102,
```
```
0111 119,119,
```
```
1000 136,136,
```
```
1001 153,153,
```
```
1010 170,170,
```
```
1011 187,187,
```
```
1100 204,204,
```
```
1101 221,221,
```
```
1110 238,238,
```
```
1111 255,255,
```
### Paleta dedykowana

Zapisane jest 16 kolorów każdy na 3 bajtach oznacza to że potrzebujemy 48 bajtów na zapis palety.
Dedykowana paleta jest zapisana jako ciąg zer i jedynek, każde kolejne 3 bajty oznaczają kolejny kolor.
Paleta dedykowana wyznaczana jest na podstawie algorytmu MedianCut. Zapis palety dedykowanej
odbywa się sekwencyjnie zapisujemy kolejno kolory od zerowego miejsca w tablicy do ostatniego o
indeksie 15.

### Używana kompresja

0 – oznacza, że do zapisu obrazka został wykorzystany algorytm kompresji RLE

1 – oznacza, że do zapisu obrazka został wykorzystany algorytm kompresji ByteRun

Oba z powyższych algorytmów to algorytmy kompresji bezstratnej. Program wybiera algorytm na
podstawie stopnia kompresji, ten algorytm, który skompresuje obrazek w większym stopniu zostanie
wykorzystany.


### Dithering

0 – nie został zastosowany dithering

1 – został zastosowany dithering

### Podsumowanie

Pobieranie danych z obrazka następuje odpowiednio od lewej do prawej, i z góry do dołu. Następnie są
one konwertowane na indeksy kolorów z odpowiedniej palety (dedykowanej, narzuconej bądź odcieni
szarości). Program dokonuje kompresji danych za pomocą obu algorytmów, wybiera lepszy z nich, zapisuje
informacje o zakodowanym obrazku do nowego formatu wraz ze skompresowanym obrazkiem w
wybranej palecie kolorów.

## Instrukcja użytkowania aplikacji

Aplikacja do konwersji plików graficznych w formacie bitmapy na autorski format graficzny z
rozszerzeniem .bcdj została napisana w języku C++ z wykorzystaniem biblioteki SDL. Program opiera się o
konsolę a także graficzny interfejs. Po uruchomieniu aplikacji wyświetla się konsola z menu wyboru :

```
rys.1 Menu wyboru – główna część programu
```
Na podstawie menu wybieramy odpowiednio operację którą chcemy wykonać :

```
 wybierając 0 zakończymy działanie programu
 wybierając 1 dokonamy konwersji obrazka o rozszerzeniu .bmp na autorki format graficzny
o wpisujemy nazwę pliku na którym chcemy dokonać konwersji (wraz z rozszerzeniem oraz
o wymiarach mniejszych niż 1024x1024) np.: obrazek4.bmp
o pojawia się menu wyboru palety kolorów oraz staruje interfejs graficzny z podglądem, a
także informacja o palecie dedykowanej
o wybieramy odpowiednią paletę w oknie podglądu odpowiednio wybierając:
 0 – paleta wbudowana (narzucona)
 1 – odcienie szarości
 2 – paleta dedykowana
 Esc – aby przejść dalej
o wybieramy odpowiednio czy chcemy zastosować dithering tak jak w poprzednio w trybie
podlądu:
 0 – dithering wyłączony
 1 – dithering włączony
 Esc – przejdź dalej
```

```
o Następuje kompresja obrazka, dostajemy informację jakiego algorytmu użyła aplikacja, a
następnie nowo powstały plik jest zapisywany w tej samej lokalizacji z rozszrzeniem .bcdj
 wybierając 2 wyświetlamy obrazek z autorskim rozszerzeniem
o podajemy nazwę pliku wraz z rozszerzeniem
o obrazek zostaje wyświetlony, jeśli chcemy zakończyć wciskamy Esc
```
### Przykładowa obsługa aplikacji

1. Uruchamiamy aplikację
2. Wybieramy 1- chcemy dokonać konwersji obrazka
3. Należy podać nazwę obrazka, na którym chcemy dokonań konwersji wybieramy obrazek4.bmp


Zostały wyświetlone informacje na temat obrazka (jego szerokość i wysokość), menu wyboru a także
stworzona dla niego dedykowana paleta kolorów. Otwarte został też nowe okno z podglądem obrazka na
którym będziemy operować.

4. Wybieramy 0 – paleta wbudowana a następnie wciskamy Esc aby przejść dalej.

W głównej części aplikacji pojawiła się informacja o naszym wyborze a także kolejne opcje aplikacji.


5. Wybieramy 1 – włączamy dithering, następnie wciskamy Esc aby przejść dalej

Okno podglądu zostaje zamknięte, a w konsoli wyświetlają się informacje o naszym wyborze a także
informacja jaki algorytm kompresji został wykorzystany do zapisu pliku.

Ponownie pojawia się główne menu wyboru dlatego teraz otworzymy stworzony przez aplikację obrazek z
rozszerzeniem bcdj.

6. Wybieramy 2 aby otworzyć plik i wpisujemy jego nazwę obrazek4.bcdj

Zostały wyświetlone informacje zawarte w nagłówku o naszym pliku między innymi szerokość wysokość,
informacje o wybranej palecie, algorytmie kompresji oraz czy dithering został zastosowany. Otwiera się
także nowe okno w którym zostaje wyświetlony nasz obrazek.

Ponownie, aby wyjść wciskamy Esc i wrócimy do głównej pętli programu, jeżeli chcemy zakończyć
działanie aplikacji wystarczy wybrać 0.




