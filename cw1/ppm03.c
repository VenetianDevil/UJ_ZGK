/* ***************************************************************************

Zmodyfikowana biblioteka z danymi przechowywanymi w "raster scan order"
i możliwością tworzenia wirtualnych podobrazów.

Pamięć z danymi obrazu będzie teraz zorganizowana tak, aby piksele z tego
samego wiersza sąsiadowały ze sobą. Ten sposób organizacji danych jest
powszechnie wykorzystywany, bo odpowiada kolejności w jakiej wiązka elektronów
przebiega ekran kineskopu, w jakiej zapisywane są dane w popularnych formatach
graficznych, itp. Wygodnie jest móc wziąć wskaźnik na początek obszaru danych
i po prostu zwiększać go o jeden gdy trzeba przejść do następnego piksela
wymagającego naszej uwagi.

Dla rastra 5x4 potrzebne będzie jak poprzednio 60 bajtów (5 kolumn * 4 wiersze
* 3 bajty na piksel), ale upakowanie pikseli będzie teraz wyglądało tak:
r[0][0] r[1][0] r[2][0] r[3][0] r[4][0] r[0][1] ..... r[2][3] r[3][3] r[4][3].

Mając "uj_rgb * dane" wskazujący na początek przydzielonego obszaru pamięci
adres piksela w x-tej kolumnie rastra i y-tym wierszu obliczać się będzie
teraz jako "dane + x + y * 5".

Liczba występująca w powyższym wzorze mówi nam o ile elementów tablicy trzeba
przesunąć wskaźnik aby trafić na piksel znajdujący się jeden wiersz niżej.
Dla zwykłych obrazów ta liczba jest równa liczbie kolumn, ale dla podobrazów
sprawa będzie wyglądała trochę inaczej.

Niech wirtualny podobraz ma wymiary 2x2 i znajduje się w prawym górnym rogu
oryginalnego obrazu 5x4. Oznacza to że piksele podobrazu są tak naprawdę
pikselami r[3][0] r[4][0] r[3][1] r[4][1]. "uj_image_get_pixel(subimg, 0, 0)"
powinno zwrócić wartość elementu r[3][0], itd.

Jeśli wprowadzić zmienną "uj_rgb * dane_podobrazu = dane + 3", to adresy
pikseli podobrazu można obliczać jako "dane_podobrazu + x + y * 5". Proszę
zwrócić uwagę że rozerwał się związek między liczbą kolumn (podobraz ma 2
kolumny) a liczbą przez którą mnożymy współrzędną y (5, odziedziczone po
oryginalnym obrazie z którym współdzielimy pamięć danych rastra). Ta druga
liczba znana jest jako "row stride" (spotyka się też nazwę "row step").

Przy implementowaniu podobrazów trzeba też zwrócić uwagę na to, że pamięć
danych rastra jest własnością oryginalnego obrazu. Co za tym idzie, wywołanie
"uj_image_destroy(subimg)" nie ma prawa jej zwolnić.

Tak trochę na boku: jeśli dla obrazu 5x4 i wskaźnika "uj_rgb * dane" adres
piksela oblicza się jako "dane + x + y * 5", to jak by to wyglądało gdyby
zmienić typ wskaźnika na "unsigned char * dane"?
Odpowiedź: "dane + x * 3 + y * 15". Dodatkowo, "dane[x * 3 + y * 15]" dawałoby
dostęp do składowej R tego piksela, "dane[1 + x * 3 + y * 15]" do składowej G,
a "dane[2 + x * 3 + y * 15]" do B.

Uwaga: przykład dydaktyczny, biblioteka nie sprawdza czy użytkownik wywołujący
jej funkcje podaje sensowne wartości argumentów.

*************************************************************************** */



/* ---- nagłówek biblioteki, mógłby się nazywać np. <uj/image.h> ---- */

#include <stdbool.h>
#include <stdio.h>

struct uj_rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

typedef struct uj_rgb uj_rgb;

struct uj_image;

typedef struct uj_image uj_image;

uj_image * uj_image_create(int width, int height);

/**
 * Stwórz wirtualny podobraz, odwołujący się do tych samych danych rastra co
 * oryginalny obraz. W razie braku wolnej pamięci zwraca NULL. Wszystkie
 * podobrazy muszą zostać zniszczone zanim będzie można zniszczyć oryginał.
 */
uj_image * uj_image_create_subimage(uj_image * image, int x, int y,
                int width, int height);

/**
 * Stwórz strukturę reprezentującą obraz o podanych wymiarach, ale nie alokuj
 * pamięci na dane rastra. Zamiast tego użyj wskazanego bufora, który musi
 * mieć co najmniej 3 * width * height bajtów. Bufor nie jest inicjowany. Przy
 * niszczeniu obrazu zwolniona będzie tylko struktura go reprezentująca; za
 * zwolnienie bufora odpowiada ten, kto go stworzył. Zwraca NULL w razie braku
 * wolnej pamięci na zaalokowanie struktury uj_image.
 */
uj_image * uj_image_create_from_buffer(int width, int height, void * buffer);

void uj_image_destroy(uj_image * image);

int uj_image_get_width(uj_image * image);

int uj_image_get_height(uj_image * image);

uj_rgb uj_image_get_pixel(uj_image * image, int x, int y);

void uj_image_set_pixel(uj_image * image, int x, int y, uj_rgb color);

void uj_image_fill_rectangle(uj_image * image, int x1, int y1,
        int x2, int y2, uj_rgb color);

/**
 * Zapisuje obraz w formacie PPM do podanego strumienia. Strumień musi być
 * strumieniem binarnym. Zwraca "true" jeśli operacja się powiodła, "false"
 * jeśli po drodze wystąpił błąd we/wy.
 */
bool uj_image_save_stream(uj_image * image, FILE * stream);

bool uj_image_save_file(uj_image * image, const char * filename);



/* ---- implementacja biblioteki, kompilowałaby się do libuj.so ---- */

// #include <uj/image.h>

#include <stdlib.h>
#include <string.h>

struct uj_image {
    uj_rgb * data;
    int row_stride;
    int width;
    int height;
    bool data_belongs_to_me;
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

    p->row_stride = width;
    p->width = width;
    p->height = height;
    p->data_belongs_to_me = true;

    return p;
}

uj_image * uj_image_create_subimage(uj_image * image, int x, int y,
                int width, int height)
{
    uj_image * p;

    p = malloc(sizeof(uj_image));
    if (p == NULL) {
        return NULL;
    }
    memset(p, 0, sizeof(uj_image));

    p->data = image->data + x + y * image->row_stride;
    p->row_stride = image->row_stride;
    p->width = width;
    p->height = height;
    p->data_belongs_to_me = false;

    return p;
}

uj_image * uj_image_create_from_buffer(int width, int height, void * buffer)
{
    uj_image * p;

    p = malloc(sizeof(uj_image));
    if (p == NULL) {
        return NULL;
    }
    memset(p, 0, sizeof(uj_image));

    p->data = buffer;
    p->row_stride = width;
    p->width = width;
    p->height = height;
    p->data_belongs_to_me = false;

    return p;
}

void uj_image_destroy(uj_image * image)
{
    if (image->data_belongs_to_me) {
        free(image->data);
    }
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
    return image->data[ x + y * image->row_stride ];
}

void uj_image_set_pixel(uj_image * image, int x, int y, uj_rgb color)
{
    image->data[ x + y * image->row_stride ] = color;
}

void uj_image_fill_rectangle(uj_image * image, int x1, int y1,
        int x2, int y2, uj_rgb color)
{
    // Nowa, korzystająca ze wskaźników wersja algorytmu.
    for (int y = y1; y <= y2; ++y) {
        uj_rgb * p = image->data + x1 + y * image->row_stride;
        uj_rgb * q = image->data + x2 + y * image->row_stride;
        while (p <= q) {
            *p++ = color;
        }
    }
}

bool uj_image_save_stream(uj_image * image, FILE * stream)
{
    // Nowa wersja procedury zapisującej obraz w formacie PPM. Zamiast
    // tekstowego wariantu P3 użyto binarnego P6 (powstają mniejsze pliki).
    // Dzięki temu że dane jednego wiersza obrazu w tej wersji biblioteki
    // zajmują spójny obszar pamięci można cały wiersz zapisać jednym fwrite.
    if (fprintf(stream, "P6 %i %i 255\n", image->width, image->height) < 0) {
        return false;
    }
    uj_rgb * p = image->data;
    uj_rgb * e = image->data + image->height * image->row_stride;
    while (p < e) {
        if (fwrite(p, 3, image->width, stream) != image->width) {
            return false;
        }
        p = p + image->row_stride;
    }
    return true;
}

bool uj_image_save_file(uj_image * image, const char * filename)
{
    FILE * f;

    if ((f = fopen(filename, "wb")) == NULL) {
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

int main(void)
{
    uj_rgb bialy = { 255, 255, 255 };
    uj_rgb pomaranczowy = { 255, 165, 0 };
    uj_rgb morski = { 46, 139, 87 };
    uj_rgb fioletowy = { 238, 130, 238 };

    uj_image * img;
    uj_image * subimg;

    img = uj_image_create(320, 160);

    uj_image_fill_rectangle(img, 0, 0, 319, 159, bialy);
    uj_image_fill_rectangle(img, 160, 0, 319, 79, pomaranczowy);
    uj_image_fill_rectangle(img, 0, 80, 159, 159, morski);
    uj_image_save_file(img, "wynik01.ppm");

    subimg = uj_image_create_subimage(img, 40, 20, 160, 80);

    uj_image_fill_rectangle(subimg, 20, 10, 99, 49, fioletowy);
    uj_image_fill_rectangle(img, 180, 90, 259, 129, fioletowy);
    uj_image_save_file(subimg, "wynik02.ppm");
    uj_image_save_file(img, "wynik03.ppm");

    uj_image_destroy(subimg);
    uj_image_destroy(img);

    // Drugi eksperyment, z zewnętrznie alokowanym buforem.

    unsigned char * buf = malloc(640 * 480 * 3);
    memset(buf, 127, 640 * 480 * 3);

    img = uj_image_create_from_buffer(640, 480, buf);

    uj_image_fill_rectangle(img, 20, 20, 299, 459, pomaranczowy);
    uj_image_fill_rectangle(img, 340, 20, 619, 459, morski);
    uj_image_save_file(img, "wynik04.ppm");

    unsigned char * p = buf + 40 * 640 * 3;
    while (p < buf + 60 * 640 * 3) {
        *p++ = 238;
        *p++ = 130;
        *p++ = 238;
    }
    uj_image_save_file(img, "wynik05.ppm");

    uj_image_destroy(img);
    free(buf);

    return 0;
}
