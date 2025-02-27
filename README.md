<h1 align="center">
  <br>
    <img width="400px" src="https://github.com/danrleiaraujo/Tradutor_Alfabeto_Cod_Morse/tree/main/src/logo.png"> 
  <br>
  PROJETO FINAL - Tradutor de Alfabeto para Código Morse.
  <br>
</h1>
<div align="center">

</div>

<div align="center"> 
  O projeto consiste em um tradutor do alfabeto latino para código Morse utilizando uma
  placa BitDogLab. A representação sonora é feita através de buzzers, enquanto a
  representação luminosa ocorre por meio do LED vermelho disponibilizado pela própria
  placa com o LED RGB. O display SSD1306 com comunicação I2C exibe a letra a ser
  selecionada em conjunto com sua representação em código Morse e os botões A e B
  permitem a navegação e seleção das letras para reprodução em Morse. O
  desenvolvimento foi feito em linguagem C, utilizando a extensão Raspberry Pi Pico no
  Visual Studio Code, com testes realizados no simulador Wokwi.
</div>

<details open="open">
<summary>Sumário</summary>
  
- [Componentes Utilizados](#componentes-utilizados)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Funcionalidades](#funcionalidades)
- [Como Executar](#como-executar)
  - [Pré-requisitos](#pré-requisitos)
  - [Passos para Execução](#passos-para-execução)
- [Vídeo Demonstração](#vídeo-demonstração)
- [Autor](#autor)

</details>


## Componentes Utilizados

1. **Raspberry Pi Pico W**: Microcontrolador para controle do sistema.
2. **Botões**: Botão A Navegação e Botão B seleção da letra.
3. **Display 128 x 64:** Exibição da letra a ser selecionada e sua representação em código Morse.
3. **Botão do Joystick:** Coloca a placa em modo de bootloader;
5. **VS Code**: Ambiente de desenvolvimento para código em linguagem C.
6. **Simulador Wokwi**: Para simulação do circuito.
  <div align="center">
      <img width="800px" src="https://github.com/danrleiaraujo/Tradutor_Alfabeto_Cod_Morse/tree/main/src/image.png" />
      <p>Fig 1. Projeto no simulador .</p>
   </div>
   
## Estrutura do Projeto

- **ssd1306.h**: Funções e configurações do display.
- **ssd1306.c**: Funções e inicialização do display.
- **lib/font.h**: Fontes minusculas, maiusculas, numeros, pontos e traços para exibição no display.
- **hardware/clocks.h**: Biblioteca para saber o clock e o tempo atual da placa.
- **hardware/i2c.h**: Biblioteca para o uso da comunicação i2c.


## Funcionalidades 
- **Interação botão A**: Pressionar o botão A avança para próxima letra do alfabeto.
- **Interação botão B**: Pressionar o botão B seleciona a letra e mostra sua representação em código Morse.
- **Botão do Joystick**: Deve alterar o estado da placa para bootloader.


## Como Executar

### Pré-requisitos

1. **Git**: Certifique-se de ter o Git instalado no seu sistema. 
2. **VS Code**: Instale o Visual Studio Code, um editor de código recomendado para desenvolvimento com o Raspberry Pi Pico.
3. **Pico SDK**: Baixe e configure o SDK do Raspberry Pi Pico, conforme as instruções da documentação oficial.
4. **Simulador Wokwi**: Utilize o simulador de eletrônica Wokwi para simular o projeto, caso não tenha acesso à placa física.
5. **Conexão USB**: Certifique-se de que o Raspberry Pi Pico W esteja conectado via USB quando for testar o código.

### Passos para Execução

1. **Clonar o repositório**: Clone o repositório utilizando o comando Git no terminal:
   
   ```bash
   git clone https://github.com/danrleiaraujo/Tradutor_Alfabeto_Cod_Morse.git
   ```
2. Abrir no VS Code: Abra a pasta clonada no VS Code e no terminal do VS Code, vá até a pasta do projeto.
3. Compilar o código.


   
## Vídeo Demonstração
  https://drive.google.com/file/d/12iysVF7F50Kk2kZcFmNPVY7C6DRdOss1/edit

  
## Autor

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/danrleiaraujo" target="_blank">
        <img src="https://avatars.githubusercontent.com/u/44043273?v=4" width="100px;" alt=""/>
      </a>
      <br /><sub><b> Danrlei Araujo</b></sub>
    </td>
  </tr>
</table>

