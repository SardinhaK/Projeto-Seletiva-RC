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

int static_variable1 = 30;
int static_variable2 = -30;

volatile long encoderTicks = 0;
int stateB;
int velDesejada = 20, velAtual = 0;
float taxaErro = 0.00 , constanteErro = 0.04, potenciaPWM;
int ticksVolta = 4096;
int velocidades[10] = {10, 2, 20, -10, 14, 17, -19, -9, -20, 0};
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
    timer2.attach(&proxima_velocidade, 3000ms);
    velocidade.period(1.0/1000);
    //velocidade = 1;

    while (1){
        printf("%d ", velDesejada);
        if(velDesejada > 0){
            printf("%d\n", velAtual);
        }else{
            printf("%d\n", (-1*velAtual));
        }
        /* printf("----------------\n");
        printf("velDesejada: %d   ", (int)(velDesejada*100));
        printf("velAtual: %d   ", (int)(velAtual*100));
        printf("Contador Ticks:%ld   ", encoderTicks);
        printf("Potencia:%d\n", (int)(potenciaPWM*100)); 
         */    //ThisThread::sleep_for(400);

        /* for (float i = 0; i <= 1; i+=0.1){
            velocidade = i;
        } */
    }
}

void proxima_velocidade (){
    vel++;
    if(vel > 9){
        vel = 0;
    }
    velDesejada = velocidades[vel];
}

void atualiza_velocidade(){
    velAtual = (encoderTicks/ticksVolta);
    taxaErro = fabs(velDesejada) - fabs(velAtual);
    if(potenciaPWM + (taxaErro * constanteErro) > 1){
        potenciaPWM = 1;
        velocidade.write(potenciaPWM);
    }else if(potenciaPWM + (taxaErro * constanteErro) < 0){
        potenciaPWM = 0;
        velocidade.write(potenciaPWM);
    }
    else{
        potenciaPWM += (taxaErro * constanteErro);
        velocidade.write(potenciaPWM);
    }
    if(velDesejada > 0){
        direcao = 0;
    }else{
        direcao = 1;
    }
    encoderTicks = 0;
}

void contador_ticks_encoder() {
    encoderTicks++;
}
