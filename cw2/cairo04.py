#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Pythonowa wersja przykładu cairo02.c. Trochę krótszy kod, bo pythonowy moduł
cairo opakowuje napisaną w C bibliotekę Cairo w sposób inteligentny, dodając
m.in. automatyczne zarządzanie pamięcią (nie trzeba więc wywoływać funkcji
*_destroy(), odśmiecacz Pythona to zrobi) oraz sygnalizowanie wystąpienia
błędów przy pomocy wyjątków (nie trzeba samemu wywoływać funkcji *_status()).
"""

import cairo

def rysuj(ctx):
    # trójkąt rysowany czerwoną linią
    ctx.set_source_rgb(1.0, 0.0, 0.0)
    ctx.move_to(50, 50)
    ctx.line_to(100, 50)
    ctx.line_to(50, 100)
    ctx.close_path()
    ctx.stroke()

    # wypełniony prostokąt cieniowany poziomo od zielonego do żółtego
    pattern = cairo.LinearGradient(200, 0, 300, 0)
    pattern.add_color_stop_rgb(0.0, 0.0, 1.0, 0.0)
    pattern.add_color_stop_rgb(1.0, 1.0, 1.0, 0.0)
    ctx.set_source(pattern)
    ctx.move_to(200, 50)
    ctx.line_to(300, 50)
    ctx.line_to(300, 100)
    ctx.line_to(200, 100)
    ctx.close_path()
    ctx.fill()

# raster 640 x 480 pikseli, zapisany potem na dysk w formacie PNG
surf = cairo.ImageSurface(cairo.FORMAT_RGB24, 640, 480)
rysuj(cairo.Context(surf))
surf.write_to_png("wynik.png")

# PDF na kartce A4 (210 × 297 mm to mniej więcej 595 x 842 pt)
surf = cairo.PDFSurface("wynik.pdf", 595, 842)
rysuj(cairo.Context(surf))
surf.show_page()
surf.finish()

# EPS, max rozmiar 10 x 10 cali (1 postscriptowy punkt == 1/72 cala)
surf = cairo.PSSurface("wynik.eps", 720, 720)
surf.set_eps(True)
rysuj(cairo.Context(surf))
surf.show_page()
surf.finish()

# SVG, 5 x 2 cale (360 x 144 pt)
surf = cairo.SVGSurface("wynik.svg", 360, 144)
rysuj(cairo.Context(surf))
surf.finish()
