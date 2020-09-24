#include <ArduinoJson.h>
#include <Thermistor.h>
#include <Servo.h>


#define LUZ_QUARTO 3 
#define BOTAO_CHAVE 4 
#define LED_CHAVE 5        
#define LDR_QUARTO A4
#define Thermistor_quarto A3
#define LDR_CLOSET A1
#define LUZ_CLOSET 7         
#define LDR_BANHEIRO A2
#define LUZ_BANHEIRO 6       
#define SERVO A0
#define IRledPin 12
#define R A5
#define G 11
#define B 9
#define W 8

StaticJsonDocument<200> doc; 
Servo s; // Variável Servo
Thermistor temp_quarto(Thermistor_quarto); //VARIÁVEL DO TIPO THERMISTOR, INDICANDO O PINO ANALÓGICO (A2) EM QUE O TERMISTOR ESTÁ CONECTADO


bool trancada = false; 
int trancar = 0, destrancar = 180;
bool statusLumi; 
bool coloredStatus = false;


unsigned long millise = 0;
unsigned long auxMillis = 0;
unsigned long constMillis = 50;

int sinal, temperature_quarto;
int cont = 0, valor = 255;

int valorR = 255, valorG = 255, valorB = 255;
void setup() {
    Serial.begin(115200);
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(W, OUTPUT);
    pinMode(LUZ_QUARTO,OUTPUT);
    pinMode(LUZ_CLOSET,OUTPUT);
    pinMode(LUZ_BANHEIRO,OUTPUT);
    pinMode(IRledPin, OUTPUT);

    digitalWrite(W, 1);
    analogWrite(R, valor);
    analogWrite(G, valor);
    analogWrite(B, valor);

  
    if(getStatusLuzQuarto()){
        digitalWrite(LUZ_QUARTO,!digitalRead(LUZ_QUARTO));
    }
    if(getStatusCloset()){
        digitalWrite(LUZ_CLOSET,!digitalRead(LUZ_CLOSET));
    }
    if(getStatusBanheiro()){
        digitalWrite(LUZ_BANHEIRO,!digitalRead(LUZ_BANHEIRO));
    }


    doc["Quarto"]["Status"] = 0;
    doc["Quarto"]["Temperatura"] = 0;
    doc["Closet"]["Status"] = 0;
    doc["Banheiro"]["Status"] = 0;
    doc["Quarto"]["Luminaria"] = 0;
    

}

void loop(){
    if(coloredStatus){
       colored();
    }
    sinal = Serial.read();
    if (sinal != -1){
        if (sinal >= 10 && sinal <= 25){
            controleSky(sinal);
        }else{
            switch (sinal){
            case 1:                                //1
                digitalWrite(LUZ_QUARTO,!digitalRead(LUZ_QUARTO));
                break;
            case 2 :                               //2
                digitalWrite(LUZ_CLOSET,!digitalRead(LUZ_CLOSET));
                break;
            case 3:                                 //3
                digitalWrite(LUZ_BANHEIRO,!digitalRead(LUZ_BANHEIRO));
                break;
            case 4:                                //4
                toggleTranca();
            break;
            case 7:
              coloredStatus = !coloredStatus;
              if(coloredStatus == false){
                  statusLumi = allOff();
                }                      
                  millise = 0;
                  auxMillis = 0;             
                  valorR = 255;
                  valorB = 255;
                  valorG = 255;
              break;
            case 5:                                 //5
                if(statusLumi == false){
                    whiteOn();
                    statusLumi = true;
                }else{
                    allOff();
                    statusLumi = false;
                }
                break;
            case 115:
                doc["Quarto"]["Status"] = getStatusLuzQuarto();
                doc["Banheiro"]["Status"] = getStatusBanheiro();
                doc["Closet"]["Status"] = getStatusCloset();
                doc["Quarto"]["Temperatura"] = temp_quarto.getTemp(); //VARIÁVEL DO TIPO INTEIRO QUE RECEBE O VALOR DE TEMPERATURA CALCULADO PELA BIBLIOTECA
                doc["Tranca"] = getServo();
                doc["Quarto"]["Luminaria"] = statusLumi;
                char json[350];
                serializeJson(doc, json);
                Serial.println(json);
                break;

            case 6:  
                statusLumi = true;         
                String cor = Serial.readString();
                deserializeJson(doc, cor);
                int red = doc["red"];
                int green = doc["green"];
                int blue = doc["blue"];
                color(red,green,blue);
                break;

            }
        }
    }
}
int aumentarLumi(int porta){
        millise = millis(); 
       if((millise - auxMillis) > 50){
          auxMillis = millise;
        analogWrite(porta, valor);
        valor = valor - 5;
     }
       if(valor != 0){
          return 255;
          }
    return 0;
   }
int diminuirLumi(int porta){
        millise = millis();
        if((millise - auxMillis) > 50){
        auxMillis =  millise;
        analogWrite(porta, valor);
        valor = valor + 5; 
        }
        if(valor != 255){
          return 0;
          }
    return 255;
           
    }

    
void colored(){      
    if (valorR == 255 && valorB == 255 && valorG == 255){
    valorR = aumentarLumi(R);
    }
    if (valorB == 255 && valorR == 0 && valorG == 255){
    valorB = aumentarLumi(B);

    }
    if (valorR == 0 && valorB == 0 && valorG == 255){
        valorR = diminuirLumi(R);
    }

    if (valorG == 255 && valorB == 0 && valorR == 255){
    valorG = aumentarLumi(G);
    }
    if (valorB == 0 && valorG == 0 && valorR == 255){
        valorB = diminuirLumi(B);
    }
    if (valorR == 255 && valorG == 0 && valorB == 255){
    valorR = aumentarLumi(R);
    }
    if (valorG == 0 && valorR == 0 && valorB == 255){
        valorG = diminuirLumi(G);
    } 
  
 }


void color(int r, int g, int b){
  analogWrite(R, r);
  analogWrite(G, g);
  analogWrite(B, b);
  }

 bool whiteOn(){
    if(coloredStatus){
      coloredStatus = !coloredStatus;
    }
    analogWrite(R,0);
    analogWrite(G, 0);
    analogWrite(B, 0);
    digitalWrite(W, 0);
    return true;
  }

 bool allOff(){
    analogWrite(R,255);
    analogWrite(G, 255);
    analogWrite(B, 255);
    digitalWrite(W, 1);
    return false;
  }
int getServo(){
    s.attach(SERVO);
    int posicao = s.read();
    s.detach();
    return posicao;
  }

void controleSky(int botao){
    switch (botao)
    {
    case 10: //Power
        skyPower();
        break;
    case 11:
        skyChUp(); // Subir Canal
        break;
    case 12:
        skyChDown(); // Descer Canal
        break;
    case 13:    
        skyVolUp(); // Subir Volume
        break;
    case 14:
        skyVolDown(); // Descer Volume
        break;
    case 15:
        skyMute(); // Mutar
        break;
    case 16:
        skyRetornar(); // Retornar Canal
        break;
    case 17:
        skyInfo(); // Info
        break;
    case 18:
        skyPlus(); // Botao + para sinopse e legenda(precisa de setas)
        break;
    }   
    delay(600);
}

bool getStatusLuzQuarto(){
    if (analogRead(LDR_QUARTO) > 600){
        return false;
    }else{
        return true;
    }
}
bool getStatusCloset(){
    if (analogRead(LDR_CLOSET) > 600){
        return false;
    }else{
        return true;
    }
}
bool getStatusBanheiro(){
    if (analogRead(LDR_BANHEIRO) > 900){
        return false;
    }else{
        return true;
    }
}
void toggleTranca(){
      s.attach(SERVO);
      delay(500);
    if(trancada){
        s.write(destrancar);
    }else{
        s.write(trancar);
    }
    trancada = !trancada;
    s.detach();
}
void pulseIR(long microsecs) {
    // we'll count down from the number of microseconds we are told to wait
    cli();  // this turns off any background interrupts
    while (microsecs > 0) {
        // 38 kHz is about 13 microseconds high and 13 microseconds low
        digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
        delayMicroseconds(10);         // hang out for 10 microseconds
        digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
        delayMicroseconds(10);         // hang out for 10 microseconds
        // so 26 microseconds altogether
        microsecs -= 26;
    }
    sei();  // this turns them back on
}

void skyPower(){
    delayMicroseconds(54792);
    pulseIR(9280);
    delayMicroseconds(4560);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(620);
    delayMicroseconds(40940);
    pulseIR(9260);
    delayMicroseconds(2280);
    pulseIR(620);
}
void skyVolUp(){
    delayMicroseconds(47304);
    pulseIR(9300);
    delayMicroseconds(4540);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(640);
    delayMicroseconds(1660);
    pulseIR(640);
    delayMicroseconds(520);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(640);
    delayMicroseconds(520);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(640);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(1660);
    pulseIR(560);
    delayMicroseconds(1760);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(640);
    delayMicroseconds(1660);
    pulseIR(640);
    delayMicroseconds(520);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(640);
    delayMicroseconds(1660);
    pulseIR(640);
    delayMicroseconds(520);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(560);
    delayMicroseconds(1740);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(560);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(40920);
    pulseIR(9220);
    delayMicroseconds(2340);
    pulseIR(540);
    delayMicroseconds(33064);
    pulseIR(9220);
    delayMicroseconds(2340);
    pulseIR(620);
}
void skyVolDown(){
    delayMicroseconds(13952);
    pulseIR(9240);
    delayMicroseconds(4600);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(640);
    delayMicroseconds(1660);
    pulseIR(600);
    delayMicroseconds(1720);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(640);
    delayMicroseconds(520);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(40960);
    pulseIR(9220);
    delayMicroseconds(2320);
    pulseIR(560);
    delayMicroseconds(33064);
    pulseIR(9300);
    delayMicroseconds(2240);    
}
void skyChUp(){
    delayMicroseconds(29788);
pulseIR(440);
delayMicroseconds(6040);
pulseIR(600);
delayMicroseconds(1940);
pulseIR(400);
delayMicroseconds(7360);
pulseIR(340);
delayMicroseconds(13540);
pulseIR(360);
delayMicroseconds(4340);
pulseIR(320);
delayMicroseconds(8340);
pulseIR(340);
delayMicroseconds(420);
pulseIR(280);
delayMicroseconds(2380);
pulseIR(560);
delayMicroseconds(1800);
pulseIR(560);
delayMicroseconds(10460);
pulseIR(300);
delayMicroseconds(42980);
pulseIR(480);
delayMicroseconds(18700);
pulseIR(300);
delayMicroseconds(6020);
pulseIR(380);
delayMicroseconds(27812);
pulseIR(9200);
delayMicroseconds(4620);
pulseIR(620);
delayMicroseconds(540);
pulseIR(620);
delayMicroseconds(1680);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(580);
delayMicroseconds(580);
pulseIR(580);
delayMicroseconds(560);
pulseIR(580);
delayMicroseconds(560);
pulseIR(580);
delayMicroseconds(580);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(620);
delayMicroseconds(1680);
pulseIR(620);
delayMicroseconds(1680);
pulseIR(620);
delayMicroseconds(1680);
pulseIR(620);
delayMicroseconds(540);
pulseIR(600);
delayMicroseconds(1700);
pulseIR(580);
delayMicroseconds(560);
pulseIR(620);
delayMicroseconds(540);
pulseIR(600);
delayMicroseconds(1700);
pulseIR(600);
delayMicroseconds(1700);
pulseIR(600);
delayMicroseconds(1700);
pulseIR(580);
delayMicroseconds(580);
pulseIR(560);
delayMicroseconds(580);
pulseIR(580);
delayMicroseconds(560);
pulseIR(580);
delayMicroseconds(580);
pulseIR(580);
delayMicroseconds(560);
pulseIR(580);
delayMicroseconds(580);
pulseIR(580);
delayMicroseconds(560);
pulseIR(580);
delayMicroseconds(560);
pulseIR(600);
delayMicroseconds(1720);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(580);
delayMicroseconds(1720);
pulseIR(560);
delayMicroseconds(40980);
pulseIR(9280);
delayMicroseconds(2260);
pulseIR(620);
delayMicroseconds(8744);
pulseIR(340);
delayMicroseconds(23900);
pulseIR(9280);
delayMicroseconds(2220);
pulseIR(660);
delayMicroseconds(45004);
pulseIR(320);
delayMicroseconds(56732);
pulseIR(600);
delayMicroseconds(9560);
pulseIR(6880);
delayMicroseconds(10740);
pulseIR(2960);
delayMicroseconds(2140);
pulseIR(1420);
delayMicroseconds(8176);
pulseIR(300);
delayMicroseconds(2360);
pulseIR(280);
delayMicroseconds(680);
pulseIR(500);
delayMicroseconds(14344);
pulseIR(1540);
delayMicroseconds(920);
pulseIR(760);
delayMicroseconds(340);
pulseIR(320);
delayMicroseconds(3180);
pulseIR(400);
delayMicroseconds(260);
pulseIR(320);
delayMicroseconds(48180);
pulseIR(680);
delayMicroseconds(4940);
pulseIR(380);
delayMicroseconds(21060);
pulseIR(320);
delayMicroseconds(5352);
pulseIR(520);
delayMicroseconds(10200);
pulseIR(500);
delayMicroseconds(10400);
pulseIR(7420);
delayMicroseconds(200);
pulseIR(400);
delayMicroseconds(960);
pulseIR(2000);
delayMicroseconds(200);
pulseIR(460);
}
void skyChDown(){
    delayMicroseconds(54084);
    pulseIR(9260);
    delayMicroseconds(4560);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(40940);
    pulseIR(9260);
    delayMicroseconds(2280);
    pulseIR(620);
}
void skyMute(){
    delayMicroseconds(10160);
    pulseIR(9240);
    delayMicroseconds(4600);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(40980);
    pulseIR(9240);
    delayMicroseconds(2300);
    pulseIR(620);
    delayMicroseconds(32984);
    pulseIR(9240);
    delayMicroseconds(2320);
    pulseIR(580);
}
void skyInfo(){
    delayMicroseconds(45608);
    pulseIR(9260);
    delayMicroseconds(4580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(580);
    delayMicroseconds(1740);
    pulseIR(580);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(560);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(1720);
    pulseIR(580);
    delayMicroseconds(580);
    pulseIR(580);
    delayMicroseconds(40960);
    pulseIR(9300);
    delayMicroseconds(2260);
    pulseIR(540);
}
void skyPlus(){
    delayMicroseconds(47620);
    pulseIR(9280);
    delayMicroseconds(4560);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(540);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(620);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(1700);
    pulseIR(600);
    delayMicroseconds(40940);
    pulseIR(9280);
    delayMicroseconds(2280);
    pulseIR(600);
    delayMicroseconds(33004);
    pulseIR(9220);
    delayMicroseconds(2340);
    pulseIR(540);
}
void skyRetornar(){
    delayMicroseconds(43972);
    pulseIR(9280);
    delayMicroseconds(4560);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(560);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(1740);
    pulseIR(640);
    delayMicroseconds(520);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(600);
    delayMicroseconds(560);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(560);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(620);
    delayMicroseconds(520);
    pulseIR(560);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(560);
    delayMicroseconds(580);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(540);
    delayMicroseconds(600);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(620);
    delayMicroseconds(1680);
    pulseIR(540);
    delayMicroseconds(1760);
    pulseIR(560);
    delayMicroseconds(1740);
    pulseIR(560);
    delayMicroseconds(600);
    pulseIR(580);
    delayMicroseconds(40960);
    pulseIR(9220);
    delayMicroseconds(2340);
    pulseIR(540);
}
