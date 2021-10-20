# pkt. 6 - przekroczenie max wart. typu uint8
clear all;
x=7;
class(x)

y = uint8(x)
class(y)
intmax(class(y))

intmax(class(y))+45
# przekraczajac za wartosc max, dostaniemy max, bo nie mozemy uzyskac
# nic wiekszego dla takiego typu zapisu.