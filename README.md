# programing_in_linux_uniproject


# Program 1: poszukiwacz

Program przyjmuje jeden argument: liczbę całkowitą dodatnią z opcjonalnie dodaną jednostką (Ki=1024, Mi=10242). Ponadto wymaga, aby jego standardowe wejście było podpięte do pliku typu potok. (Warunek musi być zweryfikowany na początku działania.)

Argument uruchomienia określa ilość liczb 2-bajtowych, jakie mają być wczytywane ze standardowego wejścia. Dla każdej liczby, która pojawiła się po raz pierwszy, program generuje rekord wyniku, który wysyła na standardowe wyjście. Rekord jest binarny i składa się z dwóch pól: znalezionej liczby i PID tego procesu.

Po zakończeniu przeszukiwania program kończy działanie ze statusem informującym o ilości powtórzeń w danych:

    0 - brak powtórzeń,
    1 - do 10% (włącznie),
    2 - do 20% (włącznie),
    ...

Jeżeli pojawiły się inne błędy (np. niepoprawne wywołanie), to zwracany status ma być większy od 10. 



# Program 2: kolekcjoner

Program kolekcjoner zatrudnia poszukiwaczy do przeszukiwania danych. Znalazców pierwszych egzemplarzy honoruje przez wpisy do księgi pamiątkowej. Prowadzi również notatki dotyczące pracowników i „jakości” danych.

Parametry uruchomienia

    -d <źródło> - ścieżka do pliku z danymi do pobrania,
    -s <wolumen> - liczba całkowita dodatnia, z opcjonalnie dodaną jednostką (Ki=1024, Mi=10242),
    ilość danych do pobrania ze źródła,
    -w <blok> - liczba całkowita dodatnia, z opcjonalnie dodaną jednostką (Ki=1024, Mi=10242),
    ilość danych do przetworzenia przez każdego potomka,
    -f <sukcesy> - ścieżka do plik do odnotowywania osiągnięć,
    -l <raporty> - ścieżka do plik z raportami o potomkach,
    -p <prac> - liczba całkowita dodatnia, maksymalna liczba potomków.

Parametry <wolumen> i <blok> określiają ilość liczb 2-bajtowych. Czyli obszary o wielkości  2·<wolumen> (2·<blok>) bajtów.
Działanie

Program tworzy <prac> potomków i do pewnego momentu pilnuje, by każdego, który zginął zastąpić nowym. (Nie dotyczy to przypadku, gdy potomki kończą działanie z powodu błędu.) Każdy potomek wykonuje program `poszukiwacz`, z parametrem o wartości <blok>.
Do komunikacji z potomkami program używa dwóch potoków anonimowych, jednego do czytania, drugiego do pisania. Każdy utworzony potomek ma do tych potoków podpinane standardowe wejście i wyjście.

Główny program kopiuje, z pliku wskazanego parametrem <źródło>, do potoku wyjściowego dane w ilości 2·<wolumnen> bajtów. Ponieważ taka operacja może się wymagać wielu podejść, to kolejne podejścia mają być wykonywane w przeplocie z pozostałymi działaniami.
Pozostałe działania to: odczytywanie wyników dostarczanych przez potomków oraz odnotowywanie ich śmierci. Obie operacje mają być wykonywane w trybie nieblokującym i mogą powodować wykonanie dalszych operacji. Jeżeli obie operacje zakończyły się niepowodzeniem, to przed kolejnym obejściem cyklu należy wprowadzić opóźnienie długości 0,48 sekundy.
Odczytywanie wyników

Proces odczytuje po jednym rezultacie na raz, tj. po jednej strukturze, opisanej w specyfikacji programu poszukiwacz. Na podstawie jej zawartości modyfikuje plik osiągnięć w taki sposób, że w komórce określonej pierwszą wartością w strukturze, wpisuje numer PID z drugiej pozycji. Uwagi. Zawartość pliku należy interpretować jako tablicę komórek o rozmiarach odpowiadających typowi pid_t. Komórki, które nie miały wpisanej wartości, mają zawierać 0. Raz zapisana komórka nie może być już więcej modyfikowana.
Odczytywanie statusów zadań i nowe potomki

Status zadania ma być odczytany możliwie najszybciej po jego zakończeniu. Do momentu, aż plik z osiągnięciami nie zostanie wypełniony w 75%, za każdego zakończonego potomka generowany jest nowy. Wyjątek stanowi zakończenie z innych powodów niż „naturalne” (status powyżej 10).
Informacje o zdarzeniach związanych z potomkami mają być odnotowywane w pliku logów. Każdy wpisy ma zawierać: odczyt zegara MONOTONIC, rodzaj zdarzenia (zakończenie lub utworzenie) oraz PID procesu, którego dotyczy. 
