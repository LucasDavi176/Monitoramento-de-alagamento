
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ==========================================
// SENSOR ULTRASSONICO
// ==========================================
const int trigPin = 7;
const int echoPin = 6;

// ==========================================
// COMPONENTES
// ==========================================
const int buzzer = 8;
const int servoPin = 9;

const int ledVerde = 10;
const int ledAmarelo = A0;
const int ledVermelho = A1;

// ==========================================
// LCD
// ==========================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==========================================
// SERVO MOTOR
// ==========================================
Servo cancela;

// ==========================================
// VARIAVEIS
// ==========================================
float distancia;
int nivelAtual = -1;

// ==========================================
// FUNCAO PARA MEDIR A DISTANCIA
// ==========================================
float medirDistancia() {

  // Envia um pulso para o sensor ultrassonico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Recebe o tempo de retorno do sinal
  long duracao = pulseIn(echoPin, HIGH, 30000);

  // Retorna 999 caso nao receba resposta
  if (duracao == 0) {
    return 999;
  }

  // Converte o tempo em distancia (cm)
  return duracao * 0.0343 / 2;
}

// ==========================================
// CONFIGURACAO INICIAL DO SISTEMA
// ==========================================
void setup() {

  // Configura os pinos do sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configura o buzzer
  pinMode(buzzer, OUTPUT);

  // Configura os LEDs
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // Inicializa o servo na posicao inicial
  cancela.attach(servoPin);
  cancela.write(0);

  // Inicializa o LCD
  lcd.init();
  lcd.backlight();

  // Exibe a mensagem inicial
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA DE");
  lcd.setCursor(0, 1);
  lcd.print("ALAGAMENTOS");

  delay(2000);
  lcd.clear();
}

// ==========================================
// FUNCIONAMENTO PRINCIPAL DO SISTEMA
// ==========================================
void loop() {

  // Realiza a leitura da distancia
  distancia = medirDistancia();

  // ========================================
  // NIVEL NORMAL (ACIMA DE 7 CM)
  // ========================================
  if (distancia > 7) {

    // Acende o LED verde
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, LOW);

    // Desliga o buzzer
    noTone(buzzer);

    // Mantem a cancela fechada
    cancela.write(0);

    // Atualiza o LCD quando o nivel muda
    if (nivelAtual != 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NIVEL: NORMAL");
      nivelAtual = 0;
    }
  }

  // ========================================
  // NIVEL DE ATENCAO (ENTRE 5 E 7 CM)
  // ========================================
  else if (distancia > 5) {

    // Acende o LED amarelo
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, HIGH);
    digitalWrite(ledVermelho, LOW);

    // Desliga o buzzer
    noTone(buzzer);

    // Mantem a cancela fechada
    cancela.write(0);

    // Atualiza o LCD quando o nivel muda
    if (nivelAtual != 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NIVEL: ATENCAO");
      nivelAtual = 1;
    }
  }

  // ========================================
  // NIVEL DE PERIGO (5 CM OU MENOS)
  // ========================================
  else {

    // Acende o LED vermelho
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, HIGH);

    // Ativa o buzzer
    tone(buzzer, 1000);

    // Abre a cancela
    cancela.write(90);

    // Atualiza o LCD quando o nivel muda
    if (nivelAtual != 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! PERIGO !!!");
      nivelAtual = 2;
    }
  }

  // ========================================
  // EXIBICAO DA DISTANCIA NO LCD
  // ========================================
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");

  // Verifica se houve leitura do sensor
  if (distancia == 999) {
    lcd.print("--");
  } else {
    lcd.print((int)distancia);
  }

  // Exibe a unidade de medida
  lcd.print(" cm       ");

  // Intervalo entre as leituras
  delay(200);
}
