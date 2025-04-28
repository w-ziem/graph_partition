# graph partition

- dodałem miejsca w strukturach na przechowywanie długosci, bo rzeczywiscie to sie przyda zdecydowanie
- przeewaluowałem całe parsowanie csrrg i tworzenie grafu na graf.csrrg i graf6.csrrg, działa, przy wielkim grafie graf1.csrrg nie wiem jak sobie z tym poradzic dzieja sie dziwne rzeczy, tam nie wystarcza tego buforu bo linijka ma ze 200tyś znaków. Idk, działa dla mniejszych
- poradziłem sobie z leakami w spectral_partition i 2 przykładach działało (czasem nawet dzilił do końca, czasem dobrze, czasem bez zachowania odpowiednich % marginesu - ale o tym informował). Na ten moment segfault rzuca evaluate_partition dla obu grafów (partition.c:312)

Co trzeba zrobić na ten moment, żeby w ogóle coś oddać:

-----Pon 28.04 19:38----- 
DONE - dodać obsługe argumentów wywołania (flag getopt), żeby mógł dzielić na więcej części
DONE (nie wiem jeszcze jak zweryfikowac poprawnosc tego binarnego ale wazne ze cos jest)- zapis do pliku tekstowo i binarnie


- ogarnąć segfaulta o którym wyżej


Tak na prawde jakby to działało to już sie da pokazać coś i dostać punkty. (A i trzeba zrobić dokumentacje końcową C i pierwszą JAVA XD)