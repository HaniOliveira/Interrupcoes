# Contador com Matriz de LEDs WS2812

## Descrição

Este projeto implementa um contador digital (0-9) utilizando uma matriz de LEDs WS2812 5x5, controlada por um Raspberry Pi Pico W presente na BitDogLab. O sistema inclui botões para incrementar e decrementar o número exibido e um LED RGB para indicação visual.

## Conexões

- **Matriz WS2812**: Pino 7
- **Botão Incremento**: Pino 5
- **Botão Decremento**: Pino 6
- **LED RGB**:
  - Vermelho: Pino 13
  - Verde: Pino 11
  - Azul: Pino 12

## Funcionalidades

1. **Display de Números**:

   - Exibe números de 0 a 9 na matriz de LEDs
   - Cada número é representado em uma matriz 5x5

2. **Controles**:

   - Botão 0: Incrementa o número atual
   - Botão 1: Decrementa o número atual

3. **Indicadores**:
   - LED vermelho piscando 5x por segundo
   - Proteção contra bouncing nos botões

## Link do Vídeo

https://drive.google.com/file/d/1G1Q5ee2e16kt3EPaacAD3kXtmW2EUlXn/view?usp=sharing

PS: No video eu disse que o led fica sempre piscando, mas na verdade ele pisca 5x por segundo.
