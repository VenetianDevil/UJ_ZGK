# pkt. 3 - przerabianianie kolorowego obrazu na odcienie szarosci 
clear all;
MANDRIL = imread('test_img/mandril_color.png');

W1 = (MANDRIL(:,:,1)/3+MANDRIL(:,:,2)/3+MANDRIL(:,:,3)/3);
# jak najpierw dodam i podziele calosc to nie zadziala, bo dostaniemy max. dla
# zakresu 255, kiedy wykroczymy za zakres dodajac najpierw dane z 330/3 zrobi
# sie 255/3 - dlatego najpierw podzielic, potem dodac
# LUB
# konwersja dzielac przez 255, potem mnozac i zmienic typ danych na UINT8
W2 = (0.299*MANDRIL(:,:,1)+0.587*MANDRIL(:,:,2)+0.114*MANDRIL(:,:,3)); 
# W2 mocniejszy kontrast, widoczne więcej cieni niż w W1


imwrite(W1, 'colorToGray1.png');
imwrite(W2, 'colorToGray2.png');