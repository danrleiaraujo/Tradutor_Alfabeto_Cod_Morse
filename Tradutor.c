#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "lib/ssd1306.h" // Display
#include "lib/font.h" // Caracteres para o display

#include "pico/bootrom.h" // Para o bootload

// Comunicacao I2C para Display ssd1306
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Led RGB
#define LED_G 11
#define LED_B 12
#define LED_R 13

#define BUZZER 21 // GPIO do Buzzer

#define BOTAO_A 5 // GPIO para botão A
#define BOTAO_B 6 // GPIO para botão B
#define BOTAO_JOYSTICK 22 // GPIO para botão do Joystick

#define DEBOUNCE_TIME 200000 // 200ms em us de debounce
uint32_t last_time = 0;      // captura o tempo do ultimo acionamento do botão para o debounce

bool select = false; // Variavel que ativa quando seleciona a letra

int letra = 0; // Contador para percorrer o alfabeto
const char alfabeto[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char morse[28][5] = { // Representacao do alfabeto em morse
  ".-",   // A
  "-...", // B
  "-.-.", // C
  "-..",  // D
  ".",    // E
  "..-.", // F
  "--.",  // G
  "....", // H
  "..",   // I
  ".---", // J
  "-.-",  // K
  ".-..", // L
  "--",   // M
  "-.",   // N
  "---",  // O
  ".--.", // P
  "--.-", // Q
  ".-.",  // R
  "...",  // S
  "-",    // T
  "..-",  // U
  "...-", // V
  ".--",  // W
  "-..-", // X
  "-.--", // Y
  "--.."  // Z
};

// Funcoes
void buzzerBeep(uint duration); // Ativacao do Buzzer
void traco(); // Representacao sonora e luminosa
void ponto(); // Representacao sonora e luminosa
void gpio_irq_handler(uint gpio, uint32_t events); // Interrupcoes dos botoes

int main(){
  gpio_init(BOTAO_A);
  gpio_set_dir(BOTAO_A, GPIO_IN);
  gpio_pull_up(BOTAO_A);
  gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  
  gpio_init(BOTAO_B);
  gpio_set_dir(BOTAO_B, GPIO_IN);
  gpio_pull_up(BOTAO_B);
  gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // Para ser utilizado o modo BOOTSEL com botão do joystick
  gpio_init(BOTAO_JOYSTICK);
  gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
  gpio_pull_up(BOTAO_JOYSTICK); 
  gpio_set_irq_enabled_with_callback(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  gpio_init(LED_R);
  gpio_set_dir(LED_R, GPIO_OUT);


  // Inicializando pino do buzzer
  gpio_init(BUZZER);
  gpio_set_dir(BUZZER, GPIO_OUT);

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);
  
  bool cor = true;
  while (true){
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_line(&ssd, 3, 25, 123, 25, cor); // Desenha uma linha
    ssd1306_line(&ssd, 3, 37, 123, 37, cor); // Desenha uma linha   
    ssd1306_draw_string(&ssd, "TRADUTOR PARA", 8, 6); // Desenha uma string
    ssd1306_draw_string(&ssd, "COD. MORSE", 20, 16); // Desenha uma string
    ssd1306_draw_string(&ssd, "SELEC. A LETRA", 9, 28); // Desenha uma string 
    ssd1306_draw_char(&ssd, alfabeto[letra], 64, 41); // Desenha uma string       
    ssd1306_draw_string(&ssd, "TRADUCAO", 8, 52); // Desenha uma string    
    ssd1306_draw_string(&ssd, morse[letra], 84, 52); // Desenha uma string 
    ssd1306_send_data(&ssd); // Atualiza o display

    if(select){
      char c1;
      for (int i=0; i<5; i++){
        c1 = morse[letra][i];
        if(c1 == '-'){
          traco();
        }
        else if(c1 == '.'){
          ponto();
        }
        sleep_ms(125);
      }
      select = false;
    }   

    sleep_ms(100);
  }
}

// Função alternativa para gerar som SEM PWM (Buzzer Toggling)
void buzzerBeep(uint duration) {
  for (int i = 0; i < duration; i++) {
      gpio_put(BUZZER, 1);
      sleep_ms(5);
      gpio_put(BUZZER, 0);
      sleep_ms(5);
  }
}

void traco(){
  gpio_put(LED_R, 1); 
  buzzerBeep(80);
  gpio_put(LED_R, 0);
}

void ponto(){  
  gpio_put(LED_R, 1);
  buzzerBeep(20);
  gpio_put(LED_R, 0);
}

void gpio_irq_handler(uint gpio, uint32_t events){
  uint32_t current_time = to_us_since_boot(get_absolute_time()); // captura o momento do acionamento e converte para microsegundos
  if(current_time - last_time > DEBOUNCE_TIME){
      if (gpio == BOTAO_A) {
        letra ++;
        if(letra > 26){
          letra = 0;
        }
      }
      else if(gpio == BOTAO_B){
        select = true;
      }
      else if(gpio == BOTAO_JOYSTICK){
        reset_usb_boot(0, 0); //func para entrar no modo bootsel 
      }
      last_time = current_time; // Atualiza o tempo para o debounce
  } // debounce
}

