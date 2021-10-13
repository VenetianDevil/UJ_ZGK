#include <stdio.h>
#include <gd.h>

int main(int argc, char * argv[])
{
    FILE * f;
    gdImagePtr img;
    int color;

    if (argc < 2) {
        fprintf(stderr, "Nie podano argumentu!\n");
        return 1;
    }

    f = fopen(argv[1], "rb");
    img = gdImageCreateFromPng(f);
    fclose(f);
    if (img == NULL) {
        fprintf(stderr, "Blad przy wczytywaniu pliku!\n");
        return 1;
    }
    printf("Rozmiary obrazka: %i x %i\n", gdImageSX(img), gdImageSY(img));
    color = gdImageGetPixel(img, 0, 0);
    printf("Skladowe RGB piksela w lewym gornym rogu: %i, %i, %i\n",
                gdImageRed(img, color), gdImageGreen(img, color),
                gdImageBlue(img, color));
    gdImageDestroy(img);

    return 0;
}
