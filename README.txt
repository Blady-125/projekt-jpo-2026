Aplikacja desktopowa napisana w języku C++ z wykorzystaniem frameworka Qt6, 
która służy do automatycznego planowania kreatywnych sesji zdjęciowych. 
Program wykorzystuje lokalny model językowy Llama3 (poprzez API Ollama) do generowania list rekwizytów, 
ustawień oświetlenia oraz propozycji kadrów.


Funkcje:

Personalizowane generowanie planów sesji na podstawie tematu, lokalizacji i stylu.

Interfejs graficzny (GUI) zbudowany w Qt6.

Integracja z lokalnym modelem AI (bezpieczeństwo danych i brak kosztów API).

Pełna dokumentacja techniczna wygenerowana w systemie Doxygen.


Wymagania:

Ollama z pobranym modelem llama3 (ollama run llama3).

Biblioteka Qt6 (rekomendowane środowisko MSYS2 UCRT64).

Kompilator C++ wspierający standard C++17 (np. GCC/MinGW).

CMake (do budowania projektu).


Uruchamianie:

Przed uruchomieniem aplikacji upewnij się, że serwer Ollama działa w tle:
ollama run llama3


W terminalu MSYS2 UCRT64 przejdź do folderu projektu i wykonaj:
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .


Po poprawnej kompilacji uruchom plik wykonywalny:
./FotoPlaner.exe



Autor:

Mateusz Blatkiewicz indeks: 155924
Kierunek: Teleinformatyka, semestr 4