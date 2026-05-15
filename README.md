# fsutils

Utilitat per mirar imatges de sistemes de fitxers EXT2 i FAT16.

## Estructura del projecte

- `main.c`: obre la imatge, detecta si es EXT2 o FAT16 i crida la comanda.
- `ext2.h`: estructures i funcions per EXT2 (`--info`, `--tree`, `--cat`).
- `fat16.h`: estructures i funcions per FAT16 (`--info`, `--tree`, `--cat`).
- `basic_func.h`: funcions auxiliars compartides.
- `makefile`: compila el programa.

Per compilar el projecte només cal executar: make


./fsutils --info <arxiu>
./fsutils --tree <arxiu>
./fsutils --cat <arxiu> <fitxer>
