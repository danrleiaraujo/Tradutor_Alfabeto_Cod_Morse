#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "lib/ssd1306.h" // Display
#include "lib/font.h" // Caracteres para o display

#include "pico/bootrom.h" // Para o bootload

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Led RGB
const uint LED_G = 11;
const uint LED_B = 12;
const uint LED_R = 13;

#define BUZZER_PIN 21

#define botao_A 5 // GPIO para botão A
#define botao_B 6 // GPIO para botão B
#define botao_Joystick 22 // GPIO para botão do Joystick


#define DEBOUNCE_TIME 200000 // 200ms em us de debounce
uint32_t last_time = 0;      // captura o tempo do ultimo acionamento do botão para o debounce

bool select = false;

int letra = 0;
const char alfabeto[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char morse[28][5] = {
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

// Função alternativa para gerar som SEM PWM (Buzzer Toggling)
void buzzerBeep(uint duration) {
  for (int i = 0; i < duration; i++) {
      gpio_put(BUZZER_PIN, 1);
      sleep_ms(5);
      gpio_put(BUZZER_PIN, 0);
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
      if (gpio == botao_A) {
        letra ++;
        if(letra > 26){
          letra = 0;
        }
      }
      else if(gpio == botao_B){
        select = true;
      }
      else if(gpio == botao_Joystick){
        reset_usb_boot(0, 0); //func para entrar no modo bootsel 
      }
      last_time = current_time; // Atualiza o tempo para o debounce
  } // debounce
}

int main(){
  gpio_init(botao_A);
  gpio_set_dir(botao_A, GPIO_IN);
  gpio_pull_up(botao_A);
  gpio_set_irq_enabled_with_callback(botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  
  gpio_init(botao_B);
  gpio_set_dir(botao_B, GPIO_IN);
  gpio_pull_up(botao_B);
  gpio_set_irq_enabled_with_callback(botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // Para ser utilizado o modo BOOTSEL com botão do joystick
  gpio_init(botao_Joystick);
  gpio_set_dir(botao_Joystick, GPIO_IN);
  gpio_pull_up(botao_Joystick); 
  gpio_set_irq_enabled_with_callback(botao_Joystick, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  gpio_init(LED_B);
  gpio_set_dir(LED_B, GPIO_OUT);

  gpio_init(LED_G);
  gpio_set_dir(LED_G, GPIO_OUT);

  gpio_init(LED_R);
  gpio_set_dir(LED_R, GPIO_OUT);


  // Inicializando pino do buzzer
  gpio_init(BUZZER_PIN);
  gpio_set_dir(BUZZER_PIN, GPIO_OUT);

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