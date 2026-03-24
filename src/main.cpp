#include <WiFi.h>
#include <WebServer.h>

void handle_root();

const char* ssid = "POCO X7 Pro";
const char* password = "Ian12345";

WebServer server(80);

const int interruptPin = 18;

volatile uint32_t lastMicros = 0;
volatile bool newData = false;

#define QUEUE_SIZE 10
uint32_t periods[QUEUE_SIZE] = {0};
uint8_t head = 0;
uint8_t count = 0;

float fMax = 0.0;
float fMed = 0.0;
float fMin = 0.0;

void updateFrequencies() {
  if (count == 0) return;
  uint32_t minP = UINT32_MAX, maxP = 0;
  uint64_t sumP = 0;
  for (uint8_t i = 0; i < count; i++) {
    uint32_t p = periods[i];
    if (p < minP) minP = p;
    if (p > maxP) maxP = p;
    sumP += p;
  }
  float avg = sumP / (float)count;
  fMax = 1000000.0 / minP;
  fMin = 1000000.0 / maxP;
  fMed = 1000000.0 / avg;
}

void IRAM_ATTR isr() {
  uint32_t now = micros();
  if (lastMicros != 0) {
    uint32_t periodo = now - lastMicros;
    periods[head] = periodo;
    head = (head + 1) % QUEUE_SIZE;
    if (count < QUEUE_SIZE) count++;
    newData = true;
    Serial.printf("¡INTERRUPCIÓN! Período = %u us\n", periodo);
  }
  lastMicros = now;
}

const char HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="refresh" content="2">
  <title>Medidor de Frecuencia ESP32</title>
  <style>
    body{font-family:Arial;text-align:center;background:#f0f0f0;}
    .container{max-width:600px;margin:50px auto;padding:30px;background:white;border-radius:15px;box-shadow:0 0 15px rgba(0,0,0,0.1);}
    h1{color:#0066ff;}
    .freq{font-size:36px;font-weight:bold;margin:15px 0;}
    .max{color:green;} .med{color:blue;} .min{color:red;}
  </style>
</head>
<body>
  <div class="container">
    <h1>📏 Medidor de Frecuencia (Cola Circular)</h1>
    <div class="freq max">Fmax: %.1f Hz</div>
    <div class="freq med">Fmed: %.1f Hz</div>
    <div class="freq min">Fmin: %.1f Hz</div>
    <p>Actualizando cada 2 segundos • Cola de 10 períodos</p>
  </div>
</body>
</html>
)rawliteral";

void handle_root() {
  if (newData) {
    updateFrequencies();
    newData = false;
  }
  char page[800];
  snprintf(page, sizeof(page), HTML, fMax, fMed, fMin);
  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), isr, RISING);   // Cambia a FALLING si no detecta

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("\n✅ WiFi OK - IP: " + WiFi.localIP().toString());

  server.on("/", handle_root);
  server.begin();
  Serial.println("🚀 Página web iniciada - Abre la IP");
}

void loop() {
  server.handleClient();

  if (newData) {
    updateFrequencies();
    newData = false;
    Serial.printf("→ Fmax: %.1f Hz | Fmed: %.1f Hz | Fmin: %.1f Hz\n", fMax, fMed, fMin);
  }
}