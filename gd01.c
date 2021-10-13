#include <stdio.h>
#include <gd.h>

int main(int argc, char * argv[])
{
    gdImagePtr img;
    int color;
    FILE * f;

    img = gdImageCreateTrueColor(100, 100);
    color = gdTrueColor(255, 255, 255);
    gdImageLine(img, 10, 80, 89, 80, color);
    color = gdTrueColor(255, 100, 255);
    gdImageLine(img, 0, 0, 89, 80, color);
    color = gdTrueColor(255, 100, 7);
    gdImageSetThickness(img, 10);
    gdImageSetAntiAliased(img, gdTrueColorAlpha(0, 0, gdBlueMax, gdAlphaOpaque));
    gdImageLine(img, 89, 0, 0, 80, gdAntiAliased);
    gdImageEllipse(img, 44, 40, 40, 20, color);
    f = fopen("wynik.png", "wb");
    gdImagePng(img, f);
    fclose(f);
    gdImageDestroy(img);

    return 0;
}
