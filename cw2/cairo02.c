/* ***************************************************************************

Program demonstrujący użycie biblioteki Cairo do generowania plików wynikowych
w różnych formatach graficznych.

Cairo używa typu cairo_surface_t do reprezentowania miejsca, gdzie trafiają
narysowane figury (zazwyczaj jest to bufor w pamięci RAM, ale może też być
plik grafiki wektorowej), a typu cairo_t do reprezentowania tzw. kontekstu
graficznego, który udostępnia operacje rysowania i pamięta jakie są aktualnie
obowiązujące parametry tych operacji (kolor, grubość linii, itp.).

Mając powierzchnię można stworzyć kontekst pozwalający po niej rysować. Układ
współrzędnych użytkownika w świeżo stworzonym kontekście ma (0,0) w lewym
górnym rogu, oś X skierowana jest w prawo, a oś Y w dół. W przypadku buforów
rastrowych domyślną jednostką długości jest piksel, dla plików wektorowych
jest to 1/72 cala (tak jak w PostScripcie).

Program zawiera tylko cząstkową obsługę błędów. Ogólnie mówiąc, powierzchnie
i konteksty Cairo mogą być poprawne albo popsute. Próba wykonania jakiejś
operacji na popsutym obiekcie nie przerywa działania aplikacji, jest to
traktowane jako operacja pusta.

*************************************************************************** */

#include <stdio.h>

#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-svg.h>

void rysuj(cairo_t * ctx)
{
    cairo_pattern_t * pat;

    // trójkąt rysowany czerwoną linią
    cairo_set_source_rgb(ctx, 1.0, 0.0, 0.0);
    cairo_move_to(ctx, 50, 50);
    cairo_line_to(ctx, 100, 50);
    cairo_line_to(ctx, 50, 100);
    cairo_close_path(ctx);
    cairo_stroke(ctx);

    // wypełniony prostokąt cieniowany poziomo od zielonego do żółtego
    pat = cairo_pattern_create_linear(200, 0, 300, 0);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.0, 1.0, 0.0);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 1.0, 1.0, 0.0);
    cairo_set_source(ctx, pat);
    cairo_move_to(ctx, 200, 50);
    cairo_line_to(ctx, 300, 50);
    cairo_line_to(ctx, 300, 100);
    cairo_line_to(ctx, 200, 100);
    cairo_close_path(ctx);
    cairo_fill(ctx);
    // przestań korzystać z gradientu i go zniszcz
    cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
    cairo_pattern_destroy(pat);
}

void narysuj_cos_na_powierzchni(cairo_surface_t * surf)
{
    if (cairo_surface_status(surf) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Jako argument podano uszkodzoną powierzchnię.\n");
        return;
    }

    cairo_t * ctx = cairo_create(surf);
    if (cairo_status(ctx) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Nie udało się stworzyć kontekstu.\n");
        goto cleanup;
    }

    rysuj(ctx);
    if (cairo_status(ctx) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Podczas rysowania coś poszło nie tak.\n");
    }

cleanup:
    cairo_destroy(ctx);
}

int main(int argc, char * argv[])
{
    cairo_surface_t * surf;

    // raster 640 x 480 pikseli, zapisany potem na dysk w formacie PNG
    surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 640, 480);
    narysuj_cos_na_powierzchni(surf);
    cairo_surface_write_to_png(surf, "wynik.png");
    cairo_surface_destroy(surf);

    // PDF na kartce A4 (210 × 297 mm to mniej więcej 595 x 842 pt)
    surf = cairo_pdf_surface_create("wynik.pdf", 595, 842);
    narysuj_cos_na_powierzchni(surf);
    cairo_surface_show_page(surf);
    // finish() uzupełnia plik tak, aby był poprawnym PDF-em i go zamyka
    // cairo_surface_finish(surf);
    // nie trzeba jej jawnie wywoływać, destroy() zrobi to automatycznie
    cairo_surface_destroy(surf);

    // EPS, max rozmiar 10 x 10 cali (1 postscriptowy punkt == 1/72 cala)
    surf = cairo_ps_surface_create("wynik.eps", 720, 720);
    cairo_ps_surface_set_eps(surf, 1);
    narysuj_cos_na_powierzchni(surf);
    cairo_surface_show_page(surf);
    // cairo_surface_finish(surf);
    cairo_surface_destroy(surf);

    // SVG, 5 x 2 cale (360 x 144 pt)
    surf = cairo_svg_surface_create("wynik.svg", 360, 144);
    narysuj_cos_na_powierzchni(surf);
    // cairo_surface_finish(surf);
    cairo_surface_destroy(surf);

    return 0;
}

// gcc -o cairo02 cairo02.c -I/usr/include/cairo -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/pixman-1 -I/usr/incpng16 -I/usr/include/freetype2 -I/usr/include/libpng16 -lcairo
// ./cairo02