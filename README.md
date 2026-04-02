Um interpretador de CHIP-8 escrito em C, rodando diretamente no terminal!

Como compilar:
```
gcc main.c gfx.c input.c -o chip8
```

Instruções para rodar:

```
./chip8 <diretorio_rom> <instrucoes_por_segundo>
```

obs: devido a vários workarounds pra pegar o input no terminal de forma eficaz, não é recomendável rodar em instruções fora de **450 < instruções < 750**

- Suporta somente sistemas operacionais UNIX, devido a divergência de bibliotecas pra input
- Sem suporte para som (beeps), inicialmente estava no planejamento, usando a biblioteca [miniaudio.h](https://github.com/mackron/miniaudio), mas deixei de lado 

Se você tiver interesse em contribuir ao projeto, sinta-se livre!