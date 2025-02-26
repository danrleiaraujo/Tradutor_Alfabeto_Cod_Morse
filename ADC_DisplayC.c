/*
 * Por: Wilton Lacerda Silva
 *    Interface Homem-Máquina (IHM) com o Display OLED
 * 
 * Este programa utiliza o display OLED SSD1306 com resolução de 128x64 pixels
 * e o microcontrolador RP2040 (Raspberry Pi Pico) para exibir informações
 * do conversor analógico-digital (ADC) e do Joystick.
 * Também mostra a leitura dos botões do Joystick e do botão A.
 * 
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "pico/bootrom.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y

#define botao_A 5 // GPIO para botão A
#define botao_B 6 // GPIO para botão B
#define JOYSTICK_PB 22 // GPIO para botão do Joystick

#define DEBOUNCE_TIME 200000 // 200ms em us de debounce
uint32_t last_time = 0;      // captura o tempo do ultimo acionamento do botão para o debounce

const char c[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
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

int letra = 0;

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
      }        
      else if(gpio == JOYSTICK_PB){
        reset_usb_boot(0, 0); //func para entrar no modo bootsel 
      }
      last_time = current_time; // Atualiza o tempo para o debounce
  } // debounce
}


int main(){
  // Para ser utilizado o modo BOOTSEL com botão B
  gpio_init(botao_B);
  gpio_set_dir(botao_B, GPIO_IN);
  gpio_pull_up(botao_B);
  gpio_set_irq_enabled_with_callback(botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  gpio_init(JOYSTICK_PB);
  gpio_set_dir(JOYSTICK_PB, GPIO_IN);
  gpio_pull_up(JOYSTICK_PB); 
  gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


  gpio_init(botao_A);
  gpio_set_dir(botao_A, GPIO_IN);
  gpio_pull_up(botao_A);
  gpio_set_irq_enabled_with_callback(botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


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

  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);  
  
  bool cor = true;
  while (true){
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_line(&ssd, 3, 25, 123, 25, cor); // Desenha uma linha
    ssd1306_line(&ssd, 3, 37, 123, 37, cor); // Desenha uma linha   
    ssd1306_draw_string(&ssd, "TRADUTOR PARA", 8, 6); // Desenha uma string
    ssd1306_draw_string(&ssd, "COD. MORSE", 20, 16); // Desenha uma string
    ssd1306_draw_string(&ssd, "SELEC A LETRA", 9, 28); // Desenha uma string 
    ssd1306_draw_char(&ssd, c[letra], 64, 41); // Desenha uma string       
    ssd1306_draw_string(&ssd, "TRADUCAO", 8, 52); // Desenha uma string    
    ssd1306_draw_string(&ssd, morse[letra], 84, 52); // Desenha uma string 
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(100);
  }
}