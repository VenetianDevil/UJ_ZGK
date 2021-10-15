/* ***************************************************************************

Próba zaimplementowania biblioteki graficznej pozwalającej tworzyć obrazy
rastrowe o dowolnych wymiarach i zapisywać je do plików PPM. Pamięć na
przechowywanie danych rastra przydzielana jest dynamicznie i zorganizowana
tak jak w poprzednim programie, czyli tak jak dwuwymiarowa tablica języka C.

W C tak naprawdę nie ma wielowymiarowych tablic, "struct uj_rgb t[5][4]"
oznacza pięcioelementową tablicę zawierającą czteroelementowe tablice
zawierające struktury uj_rgb. Taka tablica zajmuje w pamięci dokładnie
5 * 4 * sizeof(struct uj_rgb) bajtów, jej elementy ułożone są tak:
t[0][0] t[0][1] t[0][2] t[0][3] t[1][0] t[1][1] ..... t[4][2] t[4][3].

Tyle samo pamięci zajmowałaby jednowymiarowa tablica "struct uj_rgb a[20]".
Jej elementy ułożone byłyby oczywiście tak: a[0] a[1] a[2] ..... a[18] a[19].
Jak widać, a[2] trafiłby w to samo miejsce co t[0][2], a[4] tam gdzie t[1][0],
a[18] tam gdzie t[4][2]. To ważne, bo dostęp do dynamicznie przydzielanych
obszarów pamięci uzyskuje się za pośrednictwem wskaźników, a arytmetyka na
wskaźnikach jest jednowymiarowa.

Jeśli wskaźnik "struct uj_rgb * dane" pokazuje na początek przydzielonego
obszaru pamięci, to aby obliczyć adres elementu pod indeksami [x][y] trzeba
użyć wyrażenia "dane + x * 4 + y". Można też napisać "dane[x * 4 + y]" i od
razu uzyskać dostęp do struktury pod tym adresem.

Bardzo istotny jest typ wskaźnika. Dla "char * dane" adres elementu trzeba
byłoby obliczać jako "dane + (x * 4 + y) * sizeof(struct uj_rgb)", a wyrażenie
"dane[(x * 4 + y) * sizeof(struct uj_rgb)]" dawałoby dostęp do pierwszego
bajtu w strukturze uj_rgb, czyli do składowej R.

Uwaga: przykład dydaktyczny, biblioteka nie sprawdza czy użytkownik wywołujący
jej funkcje podaje sensowne wartości argumentów.

*************************************************************************** */



/* ---- nagłówek biblioteki, mógłby się nazywać np. <uj/image.h> ---- */

#include <stdbool.h>
#include <stdio.h>

/**
 * Struktura reprezentująca trójkę RGB. W odróżnieniu od poprzedniego programu
 * zamiast typu "uint8_t" użyto "unsigned char". Powód: "uint8_t" pojawił się
 * dopiero w C99 i nie jest obowiązkowy (na bardzo nietypowych architekturach
 * sprzętowych może nie być dostępny). "char" był zaś w języku C od zawsze
 * i z definicji odpowiada najmniejszej adresowalnej komórce pamięci RAM.
 */
struct uj_rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

/**
 * Alias pozwalający pisać "uj_rgb" zamiast "struct uj_rgb". Sztuczka
 * powszechnie stosowana przez programistów C. W C++ nie trzeba jej używać,
 * bo tam kompilator automatycznie definiuje takie aliasy.
 */
typedef struct uj_rgb uj_rgb;

/**
 * Reprezentująca obraz rastrowy główna struktura danych biblioteki.
 * W pliku nagłówkowym jest tylko jej deklaracja, bez definicji. Dzięki temu
 * użytkownicy nie mogą grzebać w jej wnętrznościach bo nie wiedzą jakie pola
 * w niej są.
 */
struct uj_image;

typedef struct uj_image uj_image;

/**
 * Funkcja alokująca ze sterty pamięć niezbędną do reprezentowania obrazu
 * o podanych wymiarach. Zwraca wskaźnik na zaalokowaną strukturę; w razie
 * braku wolnej pamięci zwraca NULL. Zwrócony wskaźnik gra rolę uchwytu (ang.
 * handle), podaje go się jako pierwszy argument wszystkich innych funkcji.
 */
uj_image * uj_image_create(int width, int height);

/**
 * Zwolnij pamięć przydzieloną dla danego obrazu. Zwalnianie zasobów w inny
 * sposób, np. przez własnoręczne wywołanie "free(image)", nie jest dozwolone
 * i może mieć nieoczekiwane konsekwencje.
 */
void uj_image_destroy(uj_image * image);

int uj_image_get_width(uj_image * image);

int uj_image_get_height(uj_image * image);

uj_rgb uj_image_get_pixel(uj_image * image, int x, int y);

void uj_image_set_pixel(uj_image * image, int x, int y, uj_rgb color);

void uj_image_fill_rectangle(uj_image * image, int x1, int y1,
        int x2, int y2, uj_rgb color);

void uj_image_draw_line(uj_image * image, int x1, int y1, int x2, int y2, uj_rgb color);

/**
 * Zapisuje obraz w formacie PPM do podanego strumienia. Zwraca "true" jeśli
 * operacja się powiodła, "false" jeśli po drodze wystąpił błąd we/wy.
 */
bool uj_image_save_stream(uj_image * image, FILE * stream);

/**
 * Zapisuje obraz w formacie PPM do pliku o podanej nazwie. Zwraca "true"
 * jeśli operacja się powiodła, "false" jeśli po drodze wystąpił błąd we/wy.
 */
bool uj_image_save_file(uj_image * image, const char * filename);



/* ---- implementacja biblioteki, kompilowałaby się do libuj.so ---- */

// #include <uj/image.h>

#include <stdlib.h>
#include <string.h>

// W strukturze uj_image przechowywane są metadane obrazu. Właściwe dane
// rastra są w odrębnym bloku pamięci, patrz pole "data".
struct uj_image {
    uj_rgb * data;
    int width;
    int height;
};

uj_image * uj_image_create(int width, int height)
{
    uj_image * p;

    p = malloc(sizeof(uj_image));
    if (p == NULL) {
        return NULL;
    }
    memset(p, 0, sizeof(uj_image));

    p->data = malloc(width * height * sizeof(uj_rgb));
    if (p->data == NULL) {
        free(p);
        return NULL;
    }
    memset(p->data, 0, width * height * sizeof(uj_rgb));

    p->width = width;
    p->height = height;

    return p;
}

void uj_image_destroy(uj_image * image)
{
    free(image->data);
    free(image);
}

int uj_image_get_width(uj_image * image)
{
    return image->width;
}

int uj_image_get_height(uj_image * image)
{
    return image->height;
}

uj_rgb uj_image_get_pixel(uj_image * image, int x, int y)
{
    return image->data[ x * image->height + y ];
}

void uj_image_set_pixel(uj_image * image, int x, int y, uj_rgb color)
{
    image->data[ x * image->height + y ] = color;
}

void uj_image_fill_rectangle(uj_image * image, int x1, int y1, 
        int x2, int y2, uj_rgb color)
{
    int x, y;
    // wykracza prawo/dół
    if (x1>=image->width || y1>=image->height) return;
    if (x2>=image->width) x2 = image->width-1;
    if (y2>=image->height) y2 = image->height-1;

    // wykracza lawo/góra
    if (x2<0 || y2<0) return;
    if (x1<=0) x1 = 0;
    if (y1<=0) y1 = 0;

    for (x = x1; x <= x2; ++x) {
        for (y = y1; y <= y2; ++y) {
            image->data[ x * image->height + y ] = color;
        }
    }
}

void uj_image_draw_line(uj_image * image, int x1, int y1,
        int x2, int y2, uj_rgb color)
{
    float a = (float)(y1-y2)/(x1-x2);
    float b = (float)(y1-(a*x1));

    // przycinanie poczatku lini (zakladam ze linie zawsze rysujemy od lewej do prawej)
    if(x1<0 || y1<0 || y1>=image->height) {
        x1=0;
        if(b<0){
            y1=0;
            x1=-b/a;
            if(x1>x2){
                y1=image->height-1;
                x1=(y1-b)/a;
            }
        }
        y1=b;
    }
    // przycinanie konca lini
    if(x2>=image->width || y2<0 || y2>=image->height) {
        x2=image->width-1;
        y2=a*x2+b;
        if(y2<0 || y2>=image->height){
            y2=0;
            x2=-b/a;
            if(x2<x1){
                y2=image->height-1;
                x2=(y2-b)/a;
            }
        }
    }

    // Nowa, korzystająca ze wskaźników wersja algorytmu.
    for (int x = x1; x <= x2; ++x) {
        int y=a*x+b;
        image->data[ x * image->height + y ] = color; 
    }
}

bool uj_image_save_stream(uj_image * image, FILE * stream)
{
    int x, y, res;
    uj_rgb c;

    res = fprintf(stream, "P3\n%i %i\n255\n", image->width, image->height);
    if (res < 0) {
        return false;
    }
    for (y = 0; y < image->height; ++y) {
        x = 0;
        while (x < image->width) {
            c = image->data[ x * image->height + y ];
            res = fprintf(stream, "%3i %3i %3i", c.r, c.g, c.b);
            if (res < 0) {
                return false;
            }
            ++x;
            if (x % 5 == 0 || x == image->width) {
                res = fprintf(stream, "\n");
            } else {
                res = fprintf(stream, "   ");
            }
            if (res < 0) {
                return false;
            }
        }
    }
    return true;
}

bool uj_image_save_file(uj_image * image, const char * filename)
{
    FILE * f;

    if ((f = fopen(filename, "w")) == NULL) {
        return false;
    }
    if (! uj_image_save_stream(image, f)) {
        fclose(f);
        return false;
    }
    if (fclose(f) == EOF) {
        return false;
    }
    return true;
}



/* ---- aplikacja korzystająca z biblioteki ---- */

// #include <uj/image.h>

void rysuj(uj_image * image)
{
    uj_rgb bialy = { 255, 255, 255 };
    uj_rgb pomaranczowy = { 255, 165, 0 };
    uj_rgb morski = { 46, 139, 87 };
    uj_rgb pink = { 255, 192, 203 };

    int w = uj_image_get_width(image);
    int h = uj_image_get_height(image);

    uj_image_fill_rectangle(image, -1, 0, w-1, h-1, bialy);
    uj_image_fill_rectangle(image, 10, 0, w+10, h/2-1, pomaranczowy);
    uj_image_fill_rectangle(image, 0, h/2, w/2-1, h-1, morski);
    uj_image_draw_line(image, -3, h/2, w/2-1, h-1, pink);
}

int main(void)
{
    uj_image * img;

    img = uj_image_create(32, 16);
    rysuj(img);

    // uj_image_save_stream(stdout);

    uj_image_save_file(img, "wynik.ppm");

    uj_image_destroy(img);

    return 0;
}
