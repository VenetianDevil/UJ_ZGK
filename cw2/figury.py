#!/usr/bin/python3
# -*- coding: utf-8 -*-

import cairo
import cmath as math

def translate(ctx, tx, ty):
    ctx.translate(tx, ty)

def rysuj(ctx):
    # diament niebieski    
    ctx.set_source_rgb(0, 0.3, 1)
    ctx.move_to(50, 50)
    ctx.line_to(100, 50)
    ctx.line_to(120, 75)
    ctx.line_to(75, 100)
    ctx.line_to(30, 75)
    ctx.close_path()
    ctx.stroke()

    # cwiartka
    pattern = cairo.LinearGradient(200, 0, 100, 0)
    pattern.add_color_stop_rgb(0.0, 0.0, 1.0, 0.0)
    pattern.add_color_stop_rgb(1.0, 0, 0, 0.0)
    ctx.set_source(pattern)
    ctx.move_to(150, 50)
    ctx.arc(150, 50, 50, 0, math.pi/2)
    ctx.close_path()
    ctx.fill()


# SVG, 5 x 2 cale (360 x 144 pt)
surf = cairo.SVGSurface("figury.svg", 360, 144)
ctx=cairo.Context(surf)
ctx.scale(.5, .5)

rysuj(ctx)
translate(ctx, 200, 0)
rysuj(ctx)
translate(ctx, -200, 100)
rysuj(ctx)
translate(ctx, 200, 0)
rysuj(ctx)

surf.finish()
