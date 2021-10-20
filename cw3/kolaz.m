# pkt. 2 - kolaz 2 obrazkow
clear all;
HOUSE = imread('test_img/house.png');
JESTPLANE = imread('test_img/jetplane.png');

[liczba_wierszy, liczba_kolumn, liczba_warstw] = size(HOUSE);

for y = 1:liczba_wierszy
  for x = 1:liczba_kolumn/2
    W(y,x) = HOUSE(y,x);
  end
   for x = liczba_kolumn/2:liczba_kolumn
    W(y,x) = JESTPLANE(y,x);
  end
end

imwrite(W, 'kolaz.png');
