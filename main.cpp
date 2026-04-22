/**
 * @file main.cpp
 * @brief Projekt JPO - Foto-Planer AI. 
 * Aplikacja wykorzystuje Qt6 oraz model Llama3 do planowania sesji zdjęciowych.
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
#include "json.hpp"

using json = nlohmann::json;

/**
 * @brief Klasa odpowiedzialna za połączenie z serwerem Ollama (AI).
 */
class OllamaClient {
public:
    /**
     * @brief Pobiera odpowiedź od AI na podstawie promptu.
     */
    std::string fetchResponse(const std::string& prompt) {
        json body;
        body["model"] = "llama3";
        body["prompt"] = prompt;
        body["stream"] = false;

        std::ofstream out("request.json");
        if (!out) return "Błąd zapisu pliku request.json";
        out << body.dump();
        out.close();

        // Uruchomienie curl w systemie (upewnij się, że masz curl w terminalu)
        std::string cmd = "curl -s -X POST http://localhost:11434/api/generate -d @request.json > response.json";
        if (std::system(cmd.c_str()) != 0) {
            return "Błąd: Nie udało się połączyć z AI. Sprawdź czy Ollama działa.";
        }

        std::ifstream in("response.json");
        if (!in.is_open()) return "Błąd: Brak odpowiedzi od serwera.";
        
        try {
            json resp = json::parse(in);
            in.close();
            if (resp.contains("response")) return resp["response"].get<std::string>();
        } catch (...) {
            return "Błąd: Niepoprawny format JSON.";
        }
        return "Błąd: Nieznany problem AI.";
    }
};

/**
 * @brief Klasa główna okna aplikacji (Interfejs Graficzny).
 */
class PhotoApp : public QWidget {
private:
    QLineEdit *topicInput;    
    QLineEdit *locationInput; 
    QTextEdit *resultArea;    
    QPushButton *generateBtn; 
    OllamaClient aiClient;    

public:
    PhotoApp(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Foto-Planer AI (Projekt JPO)");
        setMinimumSize(500, 600);

        auto *layout = new QVBoxLayout(this);
        
        layout->addWidget(new QLabel("Temat sesji (np. Euforia):"));
        topicInput = new QLineEdit();
        layout->addWidget(topicInput);

        layout->addWidget(new QLabel("Lokalizacja (np. Las, Studio):"));
        locationInput = new QLineEdit();
        layout->addWidget(locationInput);

        generateBtn = new QPushButton("Generuj Plan Sesji");
        layout->addWidget(generateBtn);

        layout->addWidget(new QLabel("Plan wygenerowany przez AI:"));
        resultArea = new QTextEdit();
        resultArea->setReadOnly(true);
        layout->addWidget(resultArea);

        // Połączenie przycisku z funkcją generowania
        connect(generateBtn, &QPushButton::clicked, this, &PhotoApp::onGenerate);
    }

private slots:
    /**
     * @brief Funkcja wywoływana po kliknięciu przycisku.
     */
    void onGenerate() {
        if (topicInput->text().isEmpty()) {
            QMessageBox::warning(this, "Błąd", "Wpisz temat sesji!");
            return;
        }

        generateBtn->setEnabled(false);
        resultArea->setText("AI analizuje dane... proszę czekać.");
        
        // Odświeżenie UI przed ciężkim zadaniem
        qApp->processEvents();

        std::string prompt = "Zaplanuj kreatywną sesję zdjęciową. Temat: " + 
                             topicInput->text().toStdString() + 
                             ", Miejsce: " + locationInput->text().toStdString() + 
                             ". Podaj rekwizyty i 3 kadry. Odpowiedz po polsku.";

        std::string response = aiClient.fetchResponse(prompt);
        resultArea->setText(QString::fromStdString(response));
        generateBtn->setEnabled(true);
    }
};

/**
 * @brief Start programu.
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Ustawienie stylu Fusion, żeby okno wyglądało nowocześnie
    a.setStyle("Fusion");

    PhotoApp window;
    window.show();
    
    return a.exec();
}