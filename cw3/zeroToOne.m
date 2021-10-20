# pkt. 4 - sprawdzenie czy wartosci w macierzy sa 0<x>1 i zamiana nieprawidlowych
clear all;
A = [[.3, .4,.6]; [1.2, .4, .3]];
A

any(any((A < 0) + ( A > 1 )))
#zwraca 1 jesli cos nie spełnia warunku
#zwraca 0 jesli wszystkie dane spelniaja warunek >0 i <1

A(A > 1) = 1;
A(A < 0) = 0;
A