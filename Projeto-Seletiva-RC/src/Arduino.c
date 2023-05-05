/* //chanel A e B servem para leitura do encoder
#define Chanel_A 2
#define Chanel_B 3

//pinos para controle do motor
#define motorInput1 10
#define motorInput2 11
#define speedA 9

//parametros fisicos
#define RAIO_RODA 3
#define PI 3
#define aceleracao 3

//varia de 0 a 255 e quanto maior mais rápido ele gira
float motorPWM = 25;
volatile long encoderTicks = 0;
unsigned long lastResetTime = 0;
const unsigned long RESET_INTERVAL = 1000;
String leituraVel;
float velLinear = 0, velDesejada = 10, velDif, conversor = 17, ticksVolta = 40965;

//configurando portas para controle do motor
void setup(){
  pinMode(speedA, OUTPUT);
  pinMode(motorInput1,OUTPUT); 
  pinMode(motorInput2,OUTPUT);
  pinMode(Chanel_A,INPUT_PULLUP);
  pinMode(Chanel_B,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Chanel_A), handleEncoderInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Chanel_B), handleEncoderInterrupt, CHANGE);
  Serial.begin(9600); 
 
}


void loop(){
  
  // ver a velocidade desejada
  if (Serial.available() > 0) {
    // lê a entrada da porta serial e armazena na variável inputString
    leituraVel = Serial.readString();
    // converte a string em um número inteiro e armazena na variável motorPWM
    velDesejada = leituraVel.toInt();
  }

  //Calcular velocidade atual
  velLinear = (encoderTicks/ ticksVolta) * 2 * PI * RAIO_RODA * 10;
  velDif = velDesejada - velLinear;
  if(velDesejada >= 0){
    digitalWrite(motorInput1, LOW); 
    digitalWrite(motorInput2, HIGH);
    motorPWM += (velDif * conversor);
    if(motorPWM > 250){
      motorPWM = 250;
    }
  }else if(velDesejada < 0){
    digitalWrite(motorInput1, HIGH); 
    digitalWrite(motorInput2, LOW);
    motorPWM -= (velDif * conversor);
    if(motorPWM > 250){
      motorPWM = 250;
    }
  }


  //Acelerar ou diminuir com aceleracao contante
  //if(velLinear > velDesejada){
  //  motorPWM -= aceleracao;
  //}else if(velLinear < velDesejada){
  //  motorPWM += aceleracao;
  //  if(motorPWM > 255){
  //    motorPWM -=aceleracao;
  //  }
  //}

  //Controla o sentido do motor
  //controla a velocidade do motor
  analogWrite(speedA, motorPWM);


  PROPORÇÃO RODA E TRANMIÇÃO 80/16, UMA VOLTA DA RODA = 5 VOLTAS DO MOTOR

  Serial.println("-----------------------------------");
  Serial.print("Velocidade desejada: ");
  Serial.println(velDesejada);
  Serial.print("Velocidade atual: ");
  Serial.println(velLinear);
  Serial.print("Potencia do Motor: ");
  Serial.println(motorPWM);
  Serial.print("Ticks por segundo: ");
  Serial.println(encoderTicks);
  if (milslis() - lastResetTime >= RESET_INTERVAL) {
    encoderTicks = 0;
    lastResetTime = millis();
  }
  
  delay(1000); // delay para não sobrecarregar o serial monitor

  
}



void handleEncoderInterrupt() {
  
  static int encoderState = 0;
  static int lastEncoderState = 0;
  int encoderA = digitalRead(Chanel_A);
  int encoderB = digitalRead(Chanel_B);
  encoderState = (encoderA << 1) | encoderB;
  if (encoderState != lastEncoderState) {
    if ((lastEncoderState == 0b00 && encoderState == 0b01) ||
        (lastEncoderState == 0b01 && encoderState == 0b11) ||
        (lastEncoderState == 0b11 && encoderState == 0b10) ||
        (lastEncoderState == 0b10 && encoderState == 0b00)) {
      // clockwise rotation
      encoderTicks++;
    } else if ((lastEncoderState == 0b00 && encoderState == 0b10) ||
               (lastEncoderState == 0b10 && encoderState == 0b11) ||
               (lastEncoderState == 0b11 && encoderState == 0b01) ||
               (lastEncoderState == 0b01 && encoderState == 0b00)) {
      // counter-clockwise rotation
      encoderTicks--;
    }
    lastEncoderState = encoderState;
  }
} */





/* 
#include "mbed.h"

#define direPin PB_6
#define veloPin PB_5
#define encoderA PA_8
#define encoderB PA_9


PwmOut velocidade(veloPin);
DigitalOut direcao(direPin);
InterruptIn entradaA(encoderA);
InterruptIn entradaB(encoderB);

Ticker timer1, timer2, timer3;

volatile long encoderTicks = 0;
int stateB;
float velDesejada = 0.002, velAtual, taxaErro, transmissao = 5, constanteErro = 2, potenciaPWM;
float taxaAtualizacao = 0.1, raio_roda = 0.048, PI = 3.14;
int ticksVolta = 4096, voltasPS = 0;
char entradaVel[5];
float velocidades[10] = {0.001, 0.0002, -0.001, -0.003, 0.0014, 0.0077, -0.009, 0.009, 0.00, 0.1};
int vel = 0;



void atualiza_velocidade();
void contador_ticks_encoder();
void proxima_velocidade();
static BufferedSerial serial_port(USBTX, USBRX, 115200);
FileHandle *mbed::mbed_override_console(int fd){
    return &serial_port;
}


int main(void){
    entradaA.rise(&contador_ticks_encoder);
    timer1.attach(&atualiza_velocidade, 200ms);
    //timer2.attach(&proxima_velocidade, 1000ms);
    velocidade.period(1.0/1000);

    while (1){
        printf("----------------\n");
        printf("velDesejada: %d   ", (int)(velDesejada*10000));
        printf("velAtual: %d   ", (int)(velAtual*10000));
        //printf("Contador Ticks:%ld   ", encoderTicks);
        printf("Potencia:%d\n", (int)(potenciaPWM*100)); 
            //ThisThread::sleep_for(400);

        /* for (float i = 0; i <= 1; i+=0.1){
            velocidade = i;
        } 
    }
}

void proxima_velocidade (){
    vel++;
    if(vel > 3){
        vel = 0;
    }
    velDesejada = velocidades[vel];
}

void atualiza_velocidade(){
    velAtual = (encoderTicks/ticksVolta) * 10;
    if(velDesejada >= 0){
        direcao = 0;
        taxaErro = fabs(velDesejada - velAtual);
        if(velDesejada > velAtual){
            if(potenciaPWM + (taxaErro * constanteErro) > 1){
                potenciaPWM = 1;
                velocidade.write(potenciaPWM);
            }else{
                potenciaPWM += (taxaErro * constanteErro);
                velocidade.write(potenciaPWM);
            }
        }else{
            potenciaPWM -= (taxaErro * constanteErro);
            if(potenciaPWM < 0){
                potenciaPWM = 0;
            }
            velocidade.write(potenciaPWM);
        }
    }else{
        direcao = 1;
        taxaErro = fabs(velDesejada - velAtual);
        if(velDesejada > velAtual){
            if(potenciaPWM + (taxaErro * constanteErro) > 1){
                potenciaPWM = 1;
                velocidade.write(potenciaPWM);
            }else{
                potenciaPWM += (taxaErro * constanteErro);
                velocidade.write(potenciaPWM);
            }
        }else{
            potenciaPWM -= (taxaErro * constanteErro);
            if(potenciaPWM < 0){
                potenciaPWM = 0;
            }
            velocidade.write(potenciaPWM);
        }
    }
    encoderTicks = 0;
}

void contador_ticks_encoder() {
  stateB = DigitalIn(encoderB);
  if(stateB == 0 ){
    encoderTicks++;
  }else {
    encoderTicks--;
  }
}
 */