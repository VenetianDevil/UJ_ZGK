/* ***************************************************************************

Program demonstrujący generowanie plików rastrowych w formacie PPM, a także
pierwszy przykład z serii pokazującej jak można zaimplementować bibliotekę
graficzną.

PPM to bardzo prosty format, pozwalający na zapisanie danych rastra (tzn.
trójek RGB odpowiadających kolejnym pikselom), i tyle -- żadnej kompresji
ani metadanych. Pod http://netpbm.sourceforge.net/doc/ppm.html dostępna jest
specyfikacja formatu. Jak widać, wariant "P3" jest zwykłym plikiem tekstowym.

Dzięki swojej prostocie PPM (a także jego bliźniacze formaty, PBM i PGM) jest
przydatny w zastosowaniach dydaktycznych oraz w sytuacjach gdy z jakiegoś
powodu nie mamy dostępu do bibliotek obsługujących formaty PNG i JPEG.

Uwaga: przykład dydaktyczny, brak obsługi błędów!

*************************************************************************** */

#include <stdint.h>
#include <stdio.h>

#define SZEROKOSC 32
#define WYSOKOSC 16

struct rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgb obraz[SZEROKOSC][WYSOKOSC];

void rysuj(void)
{
    struct rgb bialy = { 255, 255, 255 };
    struct rgb pomaranczowy = { 255, 165, 0 };
    struct rgb morski = { 46, 139, 87 };

    int x, y;

    // Wypełnienie całego obrazu białym kolorem.
    for (x = 0; x < SZEROKOSC; ++x) {
        for (y = 0; y < WYSOKOSC; ++y) {
            obraz[x][y] = bialy;
        }
    }
    // Prostokąty w prawej górnej i lewej dolnej ćwiartce obrazu.
    for (x = SZEROKOSC / 2; x < SZEROKOSC; ++x) {
        for (y = 0; y < WYSOKOSC / 2; ++y) {
            obraz[x][y] = pomaranczowy;
        }
    }
    for (x = 0; x < SZEROKOSC / 2; ++x) {
        for (y = WYSOKOSC / 2; y < WYSOKOSC; ++y) {
            obraz[x][y] = morski;
        }
    }
}

void zapisz(FILE * f)
{
    int x, y;

    // Nagłówek PPM.
    fprintf(f, "P3\n%i %i\n255\n", SZEROKOSC, WYSOKOSC);
    // Dane kolejnych pikseli, wyrównane aby ładnie wyglądały.
    for (y = 0; y < WYSOKOSC; ++y) {
        x = 0;
        while (x < SZEROKOSC) {
            fprintf(f, "%3i %3i %3i", obraz[x][y].r,
                            obraz[x][y].g, obraz[x][y].b);
            ++x;
            // Linie w pliku PPM powinny mieć nie więcej niż 70 znaków, więc
            // w linii tekstu zapisujemy co najwyżej pięć pikseli.
            if (x % 5 == 0 || x == SZEROKOSC) {
                fprintf(f, "\n");
            } else {
                fprintf(f, "   ");
            }
        }
    }
}

int main(void)
{
    FILE * f;

    rysuj();

    // zapisz(stdout);

    f = fopen("wynik.ppm", "w");
    zapisz(f);
    fclose(f);

    return 0;
}
