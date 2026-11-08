#include <WiFi.h>
#include <WebServer.h>

// 1. Identifiants du réseau Wi-Fi généré par l'ESP32
const char* ssid = "ESP32-Local";
const char* password = "esp32demo";

// 2. Configuration matérielle
const int LED_PIN = 23;
bool ledOn = false; // Mémorise l'état de la LED

// 3. Création du serveur web sur le port standard HTTP (80)
WebServer server(80);

// --- FONCTION : Création de la page Web ---
String buildPage() {
  // L'utilisation de R"HTML(...)HTML" permet d'écrire du code HTML brut facilement
  String html = R"HTML(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Mon ESP32</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin: 40px 16px; background: #f1f5f9; color: #172033; }
    .card { max-width: 380px; margin: auto; padding: 24px; background: white; border-radius: 18px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
    a { display: inline-block; padding: 14px 20px; margin: 8px; color: white; text-decoration: none; border-radius: 10px; font-weight: bold; }
    .on  { background: #16a34a; }
    .off { background: #dc2626; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Mon ESP32</h1>
    <p>Serveur web local</p>
    <p>État de la LED : <strong>
  )HTML";

  // Insertion dynamique de l'état de la LED dans le HTML
  html += ledOn ? "ALLUMÉE" : "ÉTEINTE";

  html += R"HTML(
    </strong></p>
    <a class="on" href="/on">ALLUMER</a>
    <a class="off" href="/off">ÉTEINDRE</a>
  </div>
</body>
</html>
  )HTML";

  return html;
}

// --- INITIALISATION ---
void setup() {
  Serial.begin(115200);

  // Initialisation de la broche de la LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 1. Création du réseau Wi-Fi (L'ESP32 devient un routeur)
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Erreur : création du Wi-Fi impossible");
    while (true) { delay(1000); } // Bloque le programme en cas d'erreur
  }
  
  Serial.print("Réseau créé ! Adresse IP : ");
  Serial.println(WiFi.softAPIP());

  // 2. Définition des "Routes" (Que fait le serveur selon l'URL demandée ?)
  
  // Page d'accueil
  server.on("/", []() {
    server.send(200, "text/html; charset=utf-8", buildPage());
  });

  // Action : Allumer
  server.on("/on", []() {
    ledOn = true;
    digitalWrite(LED_PIN, HIGH);
    server.sendHeader("Location", "/"); // Redirige vers l'accueil
    server.send(303, "text/plain", "");
  });

  // Action : Éteindre
  server.on("/off", []() {
    ledOn = false;
    digitalWrite(LED_PIN, LOW);
    server.sendHeader("Location", "/"); // Redirige vers l'accueil
    server.send(303, "text/plain", "");
  });

  // 3. Démarrage du serveur
  server.begin();
  Serial.println("Serveur HTTP démarré");
}

// --- BOUCLE PRINCIPALE ---
void loop() {
  // Écoute en permanence les requêtes de votre téléphone
  server.handleClient();
}
