# Projeto Integrador: Controle de OLED e Matriz 5×5 com Raspberry Pi Pico

Este projeto implementa uma interface interativa no Raspberry Pi Pico que integra:

- **Joystick analógico** (eixo X/Y) e **botões** para navegação e seleção  
- **Display OLED SSD1306** (I²C) para menu e visualização gráfica  
- **Matriz 5×5 de LEDs WS2812** (PIO) para mapeamento de LED único e troca de cor  
- **LEDs PWM** (vermelho e azul) para demonstração de controle de brilho analógico  
- **Buzzer** via PWM para feedback sonoro  
- **Reset em modo USB‑bootloader** via botão B  

Além de ser uma demo técnica, o projeto auxilia estudantes de sistemas embarcados a mapear cada LED da matriz 5×5 (linha, coluna e cor) de forma prática.

---

## Funcionalidades

1. **Menu inicial**  
   - Joystick em Y alterna entre “Display OLED” e “Matriz 5×5”  
   - Feedback sonoro a cada troca de opção  
   - Botão de confirmação (SWITCH_PIN) entra no modo selecionado  

2. **Modo OLED**  
   - Move um quadrado de 8×8 pixels no display conforme posições X/Y do joystick  
   - Ajusta brilho de dois LEDs (RGB R/B) proporcionalmente aos eixos X (azul) e Y (vermelho)  

3. **Modo Matriz 5×5**  
   - Converte leituras ADC do joystick em índices de linha/coluna (0–4)  
   - Inverte serpenteamento em linhas pares  
   - Botão A cicla um vetor de cores predefinidas  
   - Acende apenas o LED apontado na cor selecionada  
   - Exibe no OLED linha, coluna e índice de cor para mapeamento didático  

4. **Botão B**  
   - Reinicia o Pico em modo USB‑bootloader (para flash rápido de novo firmware)  

---

## Requisitos de Hardware

- **Raspberry Pi Pico**  
- **Joystick analógico** (VRx, VRy + GND + VCC)  
- **2 × botões** (A e B) + 1 × botão/chave de seleção  
- **Display OLED SSD1306** (I²C)  
- **Matriz WS2812 5×5** (um cabo de dados)  
- **2 × LEDs simples** (vermelho e azul)  
- **Buzzer piezo**  
- **Resistores de pull‑up** (três, para botões)  
- Fios e protoboard

### Conexões sugeridas

| Componente       | Pino Pico       |
|------------------|-----------------|
| OLED SDA         | GP14 (I²C1 SDA) |
| OLED SCL         | GP15 (I²C1 SCL) |
| WS2812 data      | GP7             |
| LED vermelho     | GP11 (PWM)      |
| LED azul         | GP13 (PWM)      |
| Buzzer           | GP21 (PWM)      |
| Botão A          | GP5             |
| Botão B          | GP6             |
| Chave de menu    | defina em `SWITCH_PIN` |

---

## Estrutura de Pastas

```
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── projeto_integrador.c
├── ws2812.pio
├── .gitignore
└── lib
    ├── display
    │   ├── fonth.h
    │   ├── ssd1306.c
    │   └── ssd1306.h
    └── joystick
        ├── joystick.c
        └── joystick.h
```

---

## Como Clonar e Compilar

1. **Clone o repositório**  
   ```bash
   git clone https://github.com/caiquedebrito/pixel_plot.git
   cd pixel_plot
   ```

2. **Prepare o Pico SDK**  
   Se ainda não tiver, clone o Pico SDK em paralelo ou como submódulo:
   ```bash
   git submodule update --init --recursive
   ```

3. **Crie o diretório de build**  
   ```bash
   mkdir build
   cd build
   ```

4. **Configure com CMake**  
   Ajuste `PICO_SDK_PATH` se seu SDK estiver em outra pasta:
   ```bash
   cmake .. -DPICO_SDK_PATH=../pico-sdk -DCMAKE_BUILD_TYPE=Release
   ```

5. **Compile**  
   ```bash
   make -j4
   ```

---

## Como Gravar no Pico

1. Pressione e segure **BOOTSEL** no Pico e conecte-o via USB ao PC.  
2. O Pico aparece como **RPI-RP2** na sua máquina.  
3. Copie o arquivo gerado (por exemplo `build/projeto_integrador.uf2`) para o drive **RPI-RP2**.  
4. O Pico reinicia automaticamente e seu projeto começa a rodar.

---

## Uso

- **Mover joystick em Y**: navega no menu.  
- **Pressionar SWITCH_PIN**: seleciona o modo.  
- **Em “Display OLED”**: mover joystick → move quadrado; dois LEDs PWM ajustam brilho.  
- **Em “Matriz 5×5”**: mover joystick → aponta LED; pressionar A → muda cor; OLED mostra linha/coluna/cor.  
- **Pressionar B**: reinicia em modo USB‑bootloader para novo flash.

---

## Licença

Este projeto está disponível sob a [MIT License](LICENSE).  

> ℹ️ **Observação**: não esqueça de definir o pino `SWITCH_PIN` no início de `projeto_integrador.c` para o botão de confirmação do menu.