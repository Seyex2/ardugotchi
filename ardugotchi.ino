int DORMIR_BUTTON = 4, BRINCAR_BUTTON = 3, COMER_BUTTON = 2, LDR = A0;
int margem_aleatoria_dormir, margem_aleatoria_brincar, margem_aleatoria_comer;

const unsigned long INTERVALO_DORMIR = 600000 / 3;       // 10 minutos/3
const unsigned long INTERVALO_BRINCAR = 180000 / 3;      // 3 minutos/3
const unsigned long INTERVALO_COMER = 240000 / 3;        // 4 minutos/3
const unsigned long INTERVALO_BONIFICACAO = 15000;       // 15 segundos
const unsigned long INTERVALO_EM_QUE_DORME = 300000 / 3; //5 minutos/3
const unsigned long intervalo_ldr = 60000/3;             // 1 minuto/3

unsigned long ultimoTempo_dormir_Pressionado, ultimoTempo_brincar_Pressionado, ultimoTempo_comer_Pressionado;
unsigned long tempoled_dormir_ligou, tempoled_brincar_ligou, tempoled_comer_ligou;

bool criar_aleatorio_dormir, criar_aleatorio_brincar, criar_aleatorio_comer;
bool led_DORMIR_ligou, led_BRINCAR_ligou, led_COMER_ligou;

const unsigned long debounceDelay = 10; 
unsigned long last_COMER_BUTTON_state, last_BRINCAR_BUTTON_state, last_DORMIR_BUTTON_state;

unsigned long lastDebounceTime_dormir = 0, lastDebounceTime_brincar = 0, lastDebounceTime_comer= 0;

int indice_vetor, soma_ldr;
int valorldr, media_ldr, valores_ldr[6];
unsigned long ultimo_ldr_check, ldr_millis_momento;

const int GAMA_ACD = 1023;
const float BAIXA_LUZ_AMBIENTE = float(GAMA_ACD) * float(2)/float(3);

int contador_penalizacoes;

bool acordado, acabar, nasceu, quer_dormir_ldr; 

const int ALTERACAO_INTENSIDADE = 5, TEMPO_ALTERACAO = 6;

unsigned long novo_minuto; 

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(LDR, INPUT);
  for (int i = 5; i <= 11; i++) {
    if (i == 5 || i == 6 || i == 9 || i == 10 || i == 11){
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
    }
  }
  pinMode(DORMIR_BUTTON, INPUT_PULLUP);
  pinMode(BRINCAR_BUTTON, INPUT_PULLUP);
  pinMode(COMER_BUTTON, INPUT_PULLUP);

  ultimoTempo_dormir_Pressionado = 0, ultimoTempo_brincar_Pressionado = 0, ultimoTempo_comer_Pressionado = 0;
  criar_aleatorio_dormir = true, criar_aleatorio_brincar = true, criar_aleatorio_comer = true;
  led_DORMIR_ligou = true, led_BRINCAR_ligou = true, led_COMER_ligou = true;
  last_COMER_BUTTON_state = HIGH, last_BRINCAR_BUTTON_state = HIGH, last_DORMIR_BUTTON_state = HIGH;
  indice_vetor = 0,  soma_ldr = 0;
  contador_penalizacoes = 0;
  acordado = true, acabar = false, nasceu = true, quer_dormir_ldr = false;
}

void fade_in(int num_led) {
  int intensidade_led = 0;
  unsigned long tempo_momento_anterior = 0;
  while (intensidade_led <= 255) {
    unsigned long tempo_momento_atual = millis();
    if (tempo_momento_atual - tempo_momento_anterior >= TEMPO_ALTERACAO) {
      analogWrite(num_led, intensidade_led);
      intensidade_led += ALTERACAO_INTENSIDADE;
      tempo_momento_anterior = tempo_momento_atual;
    }
  }
}

void fade_out(int num_led) {
  int intensidade_led = 255;
  unsigned long tempo_momento_anterior = 0;
  while (intensidade_led >= 0) {
    unsigned long tempo_momento_atual = millis();
    if (tempo_momento_atual - tempo_momento_anterior >= TEMPO_ALTERACAO) {
      analogWrite(num_led, intensidade_led);
      intensidade_led -= ALTERACAO_INTENSIDADE;
      tempo_momento_anterior = tempo_momento_atual;
    }
  }
}

void fade_in_and_out(int num_led){
  fade_in(num_led);
  fade_out(num_led);
}

void fade_total(){
  for (int i = 5; i <= 11; i++) {
    if (i == 5 || i == 6 || i == 9 || i == 10 || i == 11){
      fade_in(i);
    }
  }
  for (int i = 5; i <= 11; i++) {
    if (i == 5 || i == 6 || i == 9 || i == 10 || i == 11){
      fade_out(i);
    }
  }
}

void nascer(){
  Serial.println("Nascer");
  fade_total();
  nasceu = false;
}

void dormir(){
  if (criar_aleatorio_dormir == true) {
    margem_aleatoria_dormir = random(0, 100001) - 60000;
    criar_aleatorio_dormir = false;
  }
  
  ldr_millis_momento  = millis();
  if ( ldr_millis_momento  - ultimo_ldr_check >= intervalo_ldr) {
      ultimo_ldr_check =  ldr_millis_momento ;
      valorldr = analogRead(LDR);
      valores_ldr[indice_vetor] = valorldr;
      indice_vetor = (indice_vetor + 1) % 6;
      
      soma_ldr = 0;
      for (int i = 0; i < 6; i++) {
        soma_ldr += valores_ldr[i];
      }
      float media_ldr = (float)soma_ldr / 6;
      if (media_ldr >  BAIXA_LUZ_AMBIENTE){
          quer_dormir_ldr = true;
      }
  }

  if ((millis() - ultimoTempo_dormir_Pressionado >= INTERVALO_DORMIR + margem_aleatoria_dormir) || quer_dormir_ldr == true) {
      fade_in_and_out(11);

    if (led_DORMIR_ligou == true) {
      tempoled_dormir_ligou = millis();
      led_DORMIR_ligou = false;
    }

    if (millis() - lastDebounceTime_dormir >= debounceDelay) {
      int DORMIR_BUTTON_state = digitalRead(DORMIR_BUTTON);

      if (DORMIR_BUTTON_state != last_DORMIR_BUTTON_state) {
        lastDebounceTime_dormir = millis();
        last_DORMIR_BUTTON_state = DORMIR_BUTTON_state;

        if (DORMIR_BUTTON_state == LOW) {
          Serial.println("CARREGOU");
          ultimoTempo_dormir_Pressionado = millis();
          for (int i = 9; i <= 10; i++) {
            digitalWrite(i, LOW);
          }
          fade_out(11);

        if (ultimoTempo_dormir_Pressionado - tempoled_dormir_ligou <= INTERVALO_BONIFICACAO) {
          if (contador_penalizacoes >= 5){
            contador_penalizacoes += -5;
          fade_in_and_out(6);
          }
        }
        else {
          contador_penalizacoes += 5 * int((millis() - tempoled_dormir_ligou) / 60000);
          if (int((millis() - tempoled_dormir_ligou) / 60000) >= 1){
          fade_in_and_out(5);
          }
        }
          led_DORMIR_ligou = true;
          acordado = false;

          Serial.println("Dormir");
        }
      }
    }
  }
}

void brincar(){
  if (criar_aleatorio_brincar == true) {
  margem_aleatoria_brincar = random(0, 100001) - 60000;
  criar_aleatorio_brincar = false;
  }

  if (millis() - ultimoTempo_brincar_Pressionado >= INTERVALO_BRINCAR + margem_aleatoria_brincar) {
    fade_in_and_out(10);
    if (led_BRINCAR_ligou == true) {
      tempoled_brincar_ligou = millis();
      led_BRINCAR_ligou = false;
    }

    if (millis() - lastDebounceTime_brincar >= debounceDelay) {
      int  BRINCAR_BUTTON_state = digitalRead(BRINCAR_BUTTON);

      if (BRINCAR_BUTTON_state != last_BRINCAR_BUTTON_state) {
        lastDebounceTime_brincar = millis();
        last_BRINCAR_BUTTON_state = BRINCAR_BUTTON_state;

        if (BRINCAR_BUTTON_state == LOW) {
          Serial.println("CARREGOU");
          ultimoTempo_brincar_Pressionado = millis();
          criar_aleatorio_brincar = true;
          if (ultimoTempo_brincar_Pressionado - tempoled_brincar_ligou <= INTERVALO_BONIFICACAO) {
            if (contador_penalizacoes >= 5){
              contador_penalizacoes += -5;
            fade_in_and_out(6);
            }
          }
          else {
            contador_penalizacoes += 5 * int((millis() - tempoled_brincar_ligou) / 60000);
            if (int((millis() - tempoled_brincar_ligou) / 60000) >= 1){
            fade_in_and_out(5);
            }
          }
        led_BRINCAR_ligou = true;
        } 
      }
    }
  }
}

void comer(){
  if (criar_aleatorio_comer == true) {
  margem_aleatoria_comer = random(0, 100001) - 60000;
  criar_aleatorio_comer = false;
  }
  if (millis() - ultimoTempo_comer_Pressionado >= INTERVALO_COMER + margem_aleatoria_comer) {
  fade_in_and_out(9);
  if (led_COMER_ligou == true) {
    tempoled_comer_ligou = millis();
    led_COMER_ligou = false;
  }
    if (millis() - lastDebounceTime_comer >= debounceDelay) {
    int  COMER_BUTTON_state = digitalRead(COMER_BUTTON);
      if (COMER_BUTTON_state != last_COMER_BUTTON_state) {
        lastDebounceTime_comer = millis();
        last_COMER_BUTTON_state = COMER_BUTTON_state;

        if (COMER_BUTTON_state == LOW) {
          Serial.println("CARREGOU");
          ultimoTempo_comer_Pressionado = millis();
          criar_aleatorio_comer = true;
          if (ultimoTempo_comer_Pressionado - tempoled_comer_ligou <= INTERVALO_BONIFICACAO) {
            if (contador_penalizacoes >= 5){
            contador_penalizacoes += -5;
            fade_in_and_out(6);
            }
          } 
          else {
            contador_penalizacoes += 5 * int((millis() - tempoled_comer_ligou) / 60000);
            if (int((millis() - tempoled_comer_ligou) / 60000) >= 1){
            fade_in_and_out(5);
            }
          }
        led_COMER_ligou = true;
        }
      } 
    }
  } 
}

void loop() {
  if(nasceu == true){
    nascer();
  }
  if (acabar == false) {  
    if (contador_penalizacoes <= 25) {
      unsigned long COMER_BUTTON_state = digitalRead(COMER_BUTTON);
      unsigned long BRINCAR_BUTTON_state = digitalRead(BRINCAR_BUTTON);
      unsigned long DORMIR_BUTTON_state = digitalRead(DORMIR_BUTTON);
        
      if (millis() - novo_minuto >= 60000){
        Serial.println(contador_penalizacoes);
        novo_minuto = millis();
      }

      //acordar
      if (acordado == false && millis() - ultimoTempo_dormir_Pressionado >= INTERVALO_EM_QUE_DORME) {
        Serial.println("Acordar");
        fade_in_and_out(11);
        acordado = true;
        ultimoTempo_brincar_Pressionado = millis();
        ultimoTempo_comer_Pressionado = millis();
        ultimoTempo_dormir_Pressionado = millis();
        criar_aleatorio_dormir = true;
        for (int i = 0; i < 6; i++) {
          valores_ldr[i] = 0;
        } 
        quer_dormir_ldr = false;
      }
      if (acordado == true) {
        dormir();
        brincar();
        comer();
      }
    }
    else {
      Serial.println("Acabou o jogo");
      acabar = true;
      fade_total();
    }
  }
else{} 
}