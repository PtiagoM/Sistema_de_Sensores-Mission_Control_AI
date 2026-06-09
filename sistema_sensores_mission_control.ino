#include <LiquidCrystal.h>

/*
  MISSION CONTROL IA - SISTEMA DE SENSORES DA CAPSULA
  Missao: Artemis Deep Scan


  Mapa de pinos:
  A0 -> TMP36 temperatura interna
  A1 -> TMP36 temperatura do modulo de energia/bateria
  A2 -> LDR / Photoresistor incidencia solar
  A3 -> Force Sensor impacto estrutural

  D2 -> Tilt Sensor eixo X
  D3 -> Tilt Sensor eixo Y

  D4 -> RGB vermelho
  D5 -> RGB verde
  D6 -> RGB azul

  D7 -> Buzzer

  LCD 16x2:
  RS -> D8
  E  -> D9
  D4 -> D10
  D5 -> D11
  D6 -> D12
  D7 -> D13
*/

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Sensores analogicos
const int PIN_TEMP_INTERNA = A0;
const int PIN_TEMP_ENERGIA = A1;
const int PIN_LDR = A2;
const int PIN_FORCE = A3;

// Sensores digitais
const int PIN_TILT_X = 2;
const int PIN_TILT_Y = 3;

// Saidas
const int PIN_RGB_R = 4;
const int PIN_RGB_G = 5;
const int PIN_RGB_B = 6;
const int PIN_BUZZER = 7;

// Codigos de status
const int STATUS_NORMAL = 0;
const int STATUS_ATENCAO = 1;
const int STATUS_CRITICO = 2;

// Limites operacionais
const float TEMP_INTERNA_ATENCAO = 35.0;
const float TEMP_INTERNA_CRITICA = 45.0;

const float TEMP_ENERGIA_ATENCAO = 40.0;
const float TEMP_ENERGIA_CRITICA = 50.0;

const int SOLAR_ATENCAO = 35;
const int SOLAR_CRITICO = 15;

const int IMPACTO_ATENCAO = 35;
const int IMPACTO_CRITICO = 70;

const int LIMITE_TURBULENCIA = 3;

// Controle de tempo
unsigned long ultimoCiclo = 0;
unsigned long ultimoLCD = 0;

const unsigned long INTERVALO_CICLO = 1000;
const unsigned long INTERVALO_LCD = 2000;

// Controle do LCD
int telaLCD = 0;

// Controle mecanico
int eventosMecanicos = 0;
int impactoAnterior = 0;
int tiltXAnterior = LOW;
int tiltYAnterior = LOW;

// Ultimas leituras
float tempInterna = 0.0;
float tempEnergia = 0.0;
int luzSolar = 0;
int impacto = 0;
int tiltX = LOW;
int tiltY = LOW;

int statusTermico = STATUS_NORMAL;
int statusSolar = STATUS_NORMAL;
int statusMecanico = STATUS_NORMAL;
int statusGeral = STATUS_NORMAL;
int alertaPrincipal = 0;

/*
  Alertas:
  0 = Operacao nominal
  1 = Monitorar temperatura
  2 = Temperatura critica
  3 = Baixa luz solar
  4 = Luz solar critica
  5 = Monitorar vibracao
  6 = Instabilidade mecanica
*/

// ==========================
// Conversoes
// ==========================

float lerTMP36Celsius(int pino) {
  int raw = analogRead(pino);
  float tensao = raw * (5.0 / 1023.0);
  float celsius = (tensao - 0.5) * 100.0;
  return celsius;
}

int lerPercentualAnalogico(int pino) {
  int raw = analogRead(pino);
  int percentual = map(raw, 0, 1023, 0, 100);

  if (percentual < 0) {
    percentual = 0;
  }

  if (percentual > 100) {
    percentual = 100;
  }

  return percentual;
}

// ==========================
// Textos
// ==========================

const char* textoStatus(int statusCode) {
  if (statusCode == STATUS_CRITICO) {
    return "CRITICO";
  }

  if (statusCode == STATUS_ATENCAO) {
    return "ATENCAO";
  }

  return "NORMAL";
}

const char* textoTilt(int valor) {
  if (valor == HIGH) {
    return "ACIONADO";
  }

  return "NORMAL";
}

const char* textoAlerta(int alertaCode) {
  if (alertaCode == 1) {
    return "Monitorar temp";
  }

  if (alertaCode == 2) {
    return "Temp critica";
  }

  if (alertaCode == 3) {
    return "Baixa luz solar";
  }

  if (alertaCode == 4) {
    return "Luz solar crit";
  }

  if (alertaCode == 5) {
    return "Monitorar vib";
  }

  if (alertaCode == 6) {
    return "Instab mecanica";
  }

  return "Operacao normal";
}

// ==========================
// RGB e buzzer
// ==========================

void definirRGB(bool vermelho, bool verde, bool azul) {
  // RGB catodo comum: HIGH acende o canal
  digitalWrite(PIN_RGB_R, vermelho ? HIGH : LOW);
  digitalWrite(PIN_RGB_G, verde ? HIGH : LOW);
  digitalWrite(PIN_RGB_B, azul ? HIGH : LOW);
}

void aplicarStatusVisual(int statusCode) {
  if (statusCode == STATUS_NORMAL) {
    // Verde
    definirRGB(false, true, false);
    noTone(PIN_BUZZER);
  } else if (statusCode == STATUS_ATENCAO) {
    // Amarelo = vermelho + verde
    definirRGB(true, true, false);
    noTone(PIN_BUZZER);
  } else {
    // Vermelho + alerta sonoro
    definirRGB(true, false, false);
    tone(PIN_BUZZER, 1200);
  }
}

// ==========================
// Classificacoes
// ==========================

int classificarTermico(float tempInt, float tempEner) {
  if (tempInt >= TEMP_INTERNA_CRITICA || tempEner >= TEMP_ENERGIA_CRITICA) {
    return STATUS_CRITICO;
  }

  if (tempInt >= TEMP_INTERNA_ATENCAO || tempEner >= TEMP_ENERGIA_ATENCAO) {
    return STATUS_ATENCAO;
  }

  return STATUS_NORMAL;
}

int classificarSolar(int luz) {
  if (luz <= SOLAR_CRITICO) {
    return STATUS_CRITICO;
  }

  if (luz <= SOLAR_ATENCAO) {
    return STATUS_ATENCAO;
  }

  return STATUS_NORMAL;
}

int classificarMecanico(int impactoAtual, int tiltAtualX, int tiltAtualY) {
  bool tiltXAtivo = tiltAtualX == HIGH;
  bool tiltYAtivo = tiltAtualY == HIGH;
  bool umTiltAtivo = tiltXAtivo || tiltYAtivo;
  bool doisTiltsAtivos = tiltXAtivo && tiltYAtivo;

  bool impactoAtencao = impactoAtual >= IMPACTO_ATENCAO;
  bool impactoCritico = impactoAtual >= IMPACTO_CRITICO;

  bool variacaoImpacto = abs(impactoAtual - impactoAnterior) > 20;
  bool variacaoTilt = tiltAtualX != tiltXAnterior || tiltAtualY != tiltYAnterior;

  if (impactoAtencao || umTiltAtivo || variacaoImpacto || variacaoTilt) {
    eventosMecanicos++;
  } else {
    eventosMecanicos = 0;
  }

  impactoAnterior = impactoAtual;
  tiltXAnterior = tiltAtualX;
  tiltYAnterior = tiltAtualY;

  if (impactoCritico) {
    return STATUS_CRITICO;
  }

  if (doisTiltsAtivos) {
    return STATUS_CRITICO;
  }

  if (impactoAtencao && umTiltAtivo) {
    return STATUS_CRITICO;
  }

  if (eventosMecanicos >= LIMITE_TURBULENCIA) {
    return STATUS_CRITICO;
  }

  if (impactoAtencao || umTiltAtivo) {
    return STATUS_ATENCAO;
  }

  return STATUS_NORMAL;
}

int classificarGeral(int termico, int solar, int mecanico) {
  if (termico == STATUS_CRITICO || solar == STATUS_CRITICO || mecanico == STATUS_CRITICO) {
    return STATUS_CRITICO;
  }

  if (termico == STATUS_ATENCAO || solar == STATUS_ATENCAO || mecanico == STATUS_ATENCAO) {
    return STATUS_ATENCAO;
  }

  return STATUS_NORMAL;
}

int definirAlertaPrincipal(int termico, int solar, int mecanico) {
  if (mecanico == STATUS_CRITICO) {
    return 6;
  }

  if (termico == STATUS_CRITICO) {
    return 2;
  }

  if (solar == STATUS_CRITICO) {
    return 4;
  }

  if (mecanico == STATUS_ATENCAO) {
    return 5;
  }

  if (termico == STATUS_ATENCAO) {
    return 1;
  }

  if (solar == STATUS_ATENCAO) {
    return 3;
  }

  return 0;
}

// ==========================
// Leitura dos sensores
// ==========================

void lerSensores() {
  tempInterna = lerTMP36Celsius(PIN_TEMP_INTERNA);
  tempEnergia = lerTMP36Celsius(PIN_TEMP_ENERGIA);

  luzSolar = lerPercentualAnalogico(PIN_LDR);
  impacto = lerPercentualAnalogico(PIN_FORCE);

  tiltX = digitalRead(PIN_TILT_X);
  tiltY = digitalRead(PIN_TILT_Y);

  statusTermico = classificarTermico(tempInterna, tempEnergia);
  statusSolar = classificarSolar(luzSolar);
  statusMecanico = classificarMecanico(impacto, tiltX, tiltY);

  statusGeral = classificarGeral(statusTermico, statusSolar, statusMecanico);
  alertaPrincipal = definirAlertaPrincipal(statusTermico, statusSolar, statusMecanico);
}

// ==========================
// Serial Monitor
// ==========================

void imprimirRelatorioSerial() {
  Serial.println("================================================");
  Serial.println("MISSION CONTROL IA - SISTEMA DE SENSORES");
  Serial.println("Missao: Artemis Deep Siotcan");
  Serial.println("================================================");

  Serial.print("Temperatura interna      : ");
  Serial.print(tempInterna, 1);
  Serial.println(" C");

  Serial.print("Temp. modulo energia     : ");
  Serial.print(tempEnergia, 1);
  Serial.println(" C");

  Serial.print("Incidencia solar         : ");
  Serial.print(luzSolar);
  Serial.println("%");

  Serial.print("Impacto estrutural       : ");
  Serial.print(impacto);
  Serial.println("%");

  Serial.print("Tilt eixo X              : ");
  Serial.println(textoTilt(tiltX));

  Serial.print("Tilt eixo Y              : ");
  Serial.println(textoTilt(tiltY));

  Serial.println("------------------------------------------------");

  Serial.print("Status termico           : ");
  Serial.println(textoStatus(statusTermico));

  Serial.print("Status solar             : ");
  Serial.println(textoStatus(statusSolar));

  Serial.print("Status mecanico          : ");
  Serial.println(textoStatus(statusMecanico));

  Serial.print("Status geral da capsula  : ");
  Serial.println(textoStatus(statusGeral));

  Serial.print("Alerta principal         : ");
  Serial.println(textoAlerta(alertaPrincipal));

  Serial.print("Eventos mecanicos        : ");
  Serial.println(eventosMecanicos);

  Serial.println("================================================");
  Serial.println();
}

// ==========================
// LCD
// ==========================

void telaLCDStatus() {
  lcd.clear();

  if (telaLCD == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Int:");
    lcd.print(tempInterna, 0);
    lcd.print("C Bat:");
    lcd.print(tempEnergia, 0);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Status:");
    lcd.print(textoStatus(statusGeral));
  } else if (telaLCD == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Solar:");
    lcd.print(luzSolar);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Impacto:");
    lcd.print(impacto);
    lcd.print("%");
  } else if (telaLCD == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Tilt X:");
    lcd.print(tiltX == HIGH ? "ON " : "OK ");

    lcd.print("Y:");
    lcd.print(tiltY == HIGH ? "ON" : "OK");

    lcd.setCursor(0, 1);
    lcd.print("Mec:");
    lcd.print(textoStatus(statusMecanico));
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Alerta:");

    lcd.setCursor(0, 1);
    lcd.print(textoAlerta(alertaPrincipal));
  }

  telaLCD++;

  if (telaLCD > 3) {
    telaLCD = 0;
  }
}

// ==========================
// Inicializacao
// ==========================

void iniciarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MISSION CONTROL");
  lcd.setCursor(0, 1);
  lcd.print("Artemis Scan");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensores capsula");
  lcd.setCursor(0, 1);
  lcd.print("Sistema online");
  delay(1500);
}

void testeInicial() {
  definirRGB(false, true, false);
  tone(PIN_BUZZER, 800, 150);
  delay(250);

  definirRGB(true, true, false);
  tone(PIN_BUZZER, 1000, 150);
  delay(250);

  definirRGB(true, false, false);
  tone(PIN_BUZZER, 1200, 250);
  delay(350);

  definirRGB(false, false, false);
  noTone(PIN_BUZZER);
}

// ==========================
// Setup e loop
// ==========================

void setup() {
  Serial.begin(9600);

  pinMode(PIN_TILT_X, INPUT);
  pinMode(PIN_TILT_Y, INPUT);

  pinMode(PIN_RGB_R, OUTPUT);
  pinMode(PIN_RGB_G, OUTPUT);
  pinMode(PIN_RGB_B, OUTPUT);

  pinMode(PIN_BUZZER, OUTPUT);

  lcd.begin(16, 2);

  definirRGB(false, false, false);
  noTone(PIN_BUZZER);

  iniciarLCD();
  testeInicial();

  Serial.println("Sistema de sensores Mission Control IA iniciado.");
  Serial.println("Monitor Serial: 9600 baud.");
  Serial.println();
}

void loop() {
  unsigned long agora = millis();

  if (agora - ultimoCiclo >= INTERVALO_CICLO) {
    ultimoCiclo = agora;

    lerSensores();
    aplicarStatusVisual(statusGeral);
    imprimirRelatorioSerial();
  }

  if (agora - ultimoLCD >= INTERVALO_LCD) {
    ultimoLCD = agora;

    telaLCDStatus();
  }
}