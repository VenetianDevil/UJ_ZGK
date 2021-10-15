#!/usr/bin/python3
# -*- coding: utf-8 -*-


import cairo
import cmath as math
import sys

N=500
level=4

def sierpinski(ctx, level):
    if level==0:
      ctx.move_to(250, 0)
      ctx.line_to(N, N)
      ctx.line_to(0, N)
      ctx.close_path()
      ctx.stroke()

    if level > 0:
      ctx.scale(.5, .5)
      ctx.translate(N/2, 0)
      sierpinski(ctx, level-1)
      
      ctx.translate(-N/2, N)
      sierpinski(ctx, level-1)

      ctx.translate(N, 0)
      sierpinski(ctx, level-1)

      ctx.scale(2, 2)
      ctx.translate(-N/2, -N/2)
    
surf = cairo.SVGSurface("sierpinski.svg", N, N)
ctx=cairo.Context(surf)

ctx.set_source_rgb(0, 0, 0)

sierpinski(ctx, level)

surf.finish()
