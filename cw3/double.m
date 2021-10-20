# pkt. 5 - zamiana typu zmiennej na uint8 i boolean
clear all;
x=7;
class(x)

y= uint8(x)
class(y)

z=logical(x)
class(z)

# Any non-zero values will be converted to true (1) 
# while zero values will be converted to false (0).
