/**
 * @file main.cpp
 * @brief Projekt JPO - Foto-Planer AI.
 * * WYMAGANIA PROJEKTOWE:
 * 1. Obiektowość: Implementacja logiki w klasach OllamaClient i PhotoApp.
 * 2. GUI: Wykorzystanie frameworka Qt6 do budowy interfejsu użytkownika.
 * 3. Integracja: Komunikacja z modelem Llama3 przez protokół HTTP/JSON.
 * 4. Dokumentacja: Komentarze zgodne ze standardem Doxygen.
 * * @author Mateusz Blatkiewicz
 * @version 1.6
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QMessageBox>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

/**
 * @class OllamaClient
 * @brief Klasa odpowiedzialna za komunikację z silnikiem AI (Ollama).
 * * Realizuje wymaganie separacji logiki biznesowej (komunikacja AI) 
 * od warstwy prezentacji (interfejsu graficznego).
 */
class OllamaClient {
public:
    /**
     * @brief Wysyła zapytanie do modelu AI i przetwarza odpowiedź JSON.
     * * Funkcja realizuje zapis do pliku tymczasowego, wywołanie systemowe curl
     * oraz parsowanie otrzymanego formatu JSON.
     * * @param prompt Tekstowa instrukcja dla modelu AI.
     * @return string Wygenerowana przez AI odpowiedź lub komunikat o błędzie.
     */
    string fetchResponse(const string& prompt) {
        json body;
        body["model"] = "llama3";
        body["prompt"] = prompt;
        body["stream"] = false;

        // Serializacja obiektu JSON do pliku (wymaganie: obsługa plików)
        ofstream out("request.json");
        if (!out) return "Błąd zapisu pliku request.json";
        out << body.dump();
        out.close();

        // Integracja systemowa: wywołanie zewnętrznego narzędzia curl przez API
        string cmd = "curl -s -X POST http://localhost:11434/api/generate -d @request.json > response.json";
        if (system(cmd.c_str()) != 0) {
            return "Błąd: Upewnij się, że serwer Ollama działa w tle!";
        }

        // Deserializacja odpowiedzi (wymaganie: przetwarzanie danych JSON)
        ifstream in("response.json");
        if (!in.is_open()) return "Błąd: Brak pliku odpowiedzi od AI.";
        
        try {
            json resp = json::parse(in);
            in.close();
            if (resp.contains("response")) return resp["response"].get<string>();
        } catch (...) {
            return "Błąd: Niepoprawny format danych odebranych od AI.";
        }
        return "Błąd: Nieznany problem z modelem AI.";
    }
};

/**
 * @class PhotoApp
 * @brief Główna klasa interfejsu graficznego (GUI).
 * * Dziedziczy po QWidget, implementując wzorzec okna aplikacji.
 * Zarządza komponentami Qt i obsługuje interakcję z użytkownikiem.
 */
class PhotoApp : public QWidget {
private:
    QLineEdit *topicInput;    ///< Pole wprowadzania tematu sesji.
    QLineEdit *locationInput; ///< Pole wprowadzania lokalizacji.
    QTextEdit *resultArea;    ///< Obszar wyświetlania wygenerowanego planu.
    QPushButton *generateBtn; ///< Przycisk uruchamiający proces AI.
    OllamaClient aiClient;    ///< Obiekt klasy logicznej do komunikacji z AI.

public:
    /**
     * @brief Konstruktor klasy PhotoApp.
     * * Inicjalizuje układ elementów, tworzy widżety oraz definiuje 
     * połączenia sygnałów i slotów (kluczowy mechanizm Qt).
     */
    PhotoApp(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Foto-Planer AI (Projekt JPO)");
        setMinimumSize(550, 650);

        // Tworzenie pionowego układu graficznego (Layout Management)
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(25, 25, 25, 25);
        layout->setSpacing(10);

        // Nagłówek i pola tekstowe
        layout->addWidget(new QLabel("Temat sesji (np. Cyberpunk, Portret biznesowy):"));
        topicInput = new QLineEdit();
        layout->addWidget(topicInput);

        layout->addWidget(new QLabel("Lokalizacja i styl (np. Las, Studio, Filmowy):"));
        locationInput = new QLineEdit();
        layout->addWidget(locationInput);

        // Przycisk akcji
        generateBtn = new QPushButton("Generuj Plan Sesji");
        generateBtn->setMinimumHeight(40);
        generateBtn->setStyleSheet("font-weight: bold;");
        layout->addWidget(generateBtn);

        layout->addWidget(new QLabel("Wygenerowana propozycja:"));
        resultArea = new QTextEdit();
        resultArea->setReadOnly(true); // Ochrona przed przypadkową edycją
        layout->addWidget(resultArea);

        // Połączenie sygnału kliknięcia z funkcją obsługi (Slot)
        connect(generateBtn, &QPushButton::clicked, this, &PhotoApp::onGenerate);
    }

private slots:
    /**
     * @brief Slot obsługujący zdarzenie kliknięcia przycisku.
     * * Pobiera dane, blokuje UI na czas generowania i wyświetla wynik.
     */
    void onGenerate() {
        if (topicInput->text().isEmpty()) {
            QMessageBox::warning(this, "Błąd", "Wpisz temat sesji przed generowaniem!");
            return;
        }

        // Wizualna informacja o pracy programu
        generateBtn->setEnabled(false);
        generateBtn->setText("AI analizuje... Czekaj");
        resultArea->setText("Trwa łączenie z modelem Llama3...");
        
        // Odświeżenie interfejsu (wymagane przy operacjach blokujących)
        qApp->processEvents();

        // Przygotowanie promptu dla modelu językowego
        string prompt = "Działaj jako profesjonalny fotograf. Zaplanuj kreatywną sesję. "
                        "Temat: " + topicInput->text().toStdString() + 
                        ", Miejsce/Styl: " + locationInput->text().toStdString() + 
                        ". Podaj rekwizyty i 3 konkretne pomysły na kadry. Odpowiedz po polsku.";

        // Pobranie odpowiedzi z klasy logicznej
        string response = aiClient.fetchResponse(prompt);
        
        // Wyświetlenie wyniku i odblokowanie przycisku
        resultArea->setText(QString::fromStdString(response));
        generateBtn->setEnabled(true);
        generateBtn->setText("Generuj Plan Sesji");
    }
};

/**
 * @brief Funkcja główna.
 * * Inicjalizuje pętlę zdarzeń Qt i wyświetla okno.
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Ustawienie nowoczesnego stylu interfejsu
    a.setStyle("Fusion");

    PhotoApp window;
    window.show();
    
    return a.exec();
}