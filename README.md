# CG AB2 - Lesson Room

Cena 3D interativa em **OpenGL 3.3 puro (C11)**, no estilo dos *Resident Evil* clássicos:
câmeras fixas sobre fundos pré-renderizados, com objetos 3D modelados pela equipe,
iluminados e texturizados em tempo real por cima deles.

Trabalho prático de Computação Gráfica (AB2), sem motores ou frameworks: só
GLFW (janela/teclado), GLAD (carregador do OpenGL), stb_image (leitura de imagens) e miniaudio (som).
A matemática (vetores, matrizes, projeção) é nossa, em `src/mathlib.c`.

Relatório das técnicas: [RELATORIO.md](RELATORIO.md).

## Como compilar (Windows)

1. Instale o GCC (MinGW-w64). Com o [Scoop](https://scoop.sh):
   ```
   scoop install gcc
   ```
   Qualquer outro MinGW-w64 64 bits também serve (MSYS2, w64devkit...), desde que o `gcc` esteja no PATH.
2. Na pasta do projeto:
   ```
   .\build.bat
   ```
   (no PowerShell o `.\` é obrigatório; no Prompt de Comando, `build.bat` também funciona)
   Isso gera `cg-ab2.exe`. GLFW, GLAD e stb já estão em `third_party/`, sem nada para baixar.

## Fundos (obrigatório na primeira vez)

As imagens de fundo **não estão no repositório**, porque são arte da Capcom.
Siga [assets/backgrounds/README.md](assets/backgrounds/README.md) para baixá-las do mod
*Seamless HD Project* e colocá-las em `assets/backgrounds/`. Sem elas, o programa roda
com fundo cinza.

## Sons (opcional)

Os sons também são do jogo original e ficam fora do repositório; veja
[assets/sounds/README.md](assets/sounds/README.md). São tocados com a
[miniaudio](https://miniaud.io) (`third_party/miniaudio`, só áudio). Sem eles, o jogo roda sem som.

## Como rodar

Rode **a partir da pasta do projeto** (os caminhos de `shaders/` e `assets/` são relativos):

```
.\cg-ab2.exe
```

Opções de linha de comando: `--cam N` (ângulo inicial), `--calib` (já abre em calibração),
`--cameras arquivo.txt` (outro arquivo de câmeras), `--player X Z YAW` (posição inicial) e
`--screenshot arquivo.png`
(desenha um quadro, salva e fecha; ferramenta de desenvolvimento para conferir o visual).

## Controles

| Tecla | Ação |
|---|---|
| W / S | Andar para frente / trás (controle "tanque") |
| A / D | Girar |
| E | Interagir com o objeto mais próximo (gaveta, cadeira, globo, abajur, lareira) |
| 1-4 / Tab | Trocar ângulo de câmera na mão (a troca normal é automática, por zonas do chão) |
| F1 | Liga/desliga o modo de calibração |
| F11 | Tela cheia |
| Esc | Sair |

### Câmeras do jogo original

As câmeras da Lesson Room vêm do próprio RE1: foram lidas do arquivo da sala (`ROOM70C0.RDT`)
e salvas em `assets/cameras.txt` (posição, alvo e FOV de 60,2° de cada câmera) e
`assets/zones.txt` (zonas do chão que trocam de câmera). Assim a perspectiva 3D coincide com a
dos fundos pré-renderizados. O modo de calibração abaixo serve para conferir ou ajustar.

### Modo de calibração (F1)

Uma grade amarela é desenhada no chão (y = 0): cada quadrado tem o tamanho de um azulejo
(`TILE_SIZE` em `src/calib.h`). O eixo X aparece em vermelho, o Z em azul, e há uma haste verde de 2 m na origem.
O objetivo é mover a câmera até a grade coincidir com o piso xadrez da imagem.

| Tecla | Ação |
|---|---|
| W / A / S / D | Mover a câmera no plano |
| Q / E | Descer / subir |
| Setas | Girar (yaw / pitch) |
| Z / X | Diminuir / aumentar o FOV |
| Shift (segurado) | Ajuste fino |
| G | Grade reta ou girada 45° (os azulejos da Lesson Room estão na diagonal das paredes) |
| F5 | Salvar em `assets/cameras.txt` |
| F9 | Recarregar `assets/cameras.txt` |

Os valores atuais aparecem no título da janela. As zonas de troca de câmera aparecem no chão:
verde = saem da câmera atual, vermelho = das outras. A caixa ciano são os limites da sala
(`assets/room.txt`), que funcionam como paredes invisíveis; as caixas magenta são os móveis do
fundo (piano, mesinha, lareira), que também bloqueiam o personagem.

Dica: primeiro acerte o **yaw** (direção das linhas), depois **pitch e altura** (inclinação),
e por último o **FOV** (tamanho dos azulejos ao longe).

## Objetos modelados

Cada objeto é montado em código como uma **hierarquia de peças** (`node_box`, `node_cylinder`)
em `src/objects/`. Mover ou girar um nó move todos os filhos: a gaveta, por exemplo, é um
grupo com frente, fundo, laterais e puxador, e só o grupo é transladado ao abrir.

| Objeto | Arquivo | Peças / técnica | Materiais | Interação |
|---|---|---|---|---|
| Escrivaninha | `desk.c` | tampo, pernas com pé, saia, nicho, gaveta em grupo + puxador | madeira envernizada, latão | **E**: gaveta desliza (translação) e empurra o personagem se ele estiver na frente |
| Caixote | `crate.c` | miolo, montantes, 16 ripas, tábuas, diagonais a 45° | madeira crua | obstáculo (bloqueia o personagem) |
| Escadinha | `stepladder.c` | tubos inclinados (cilindro entre 2 pontos), degraus com frisos | metal pintado, aço, borracha | obstáculo (bloqueia o personagem) |
| Cadeira | `chair.c` | encosto como sub-grupo inclinado, almofada em 2 níveis | madeira escura, couro | **E**: sai de baixo da mesa deslizando (translação) e empurra o personagem se ele estiver na frente |
| Quadro-negro | `chalkboard.c` | cavalete em A, lousa inclinada, giz e apagador | madeira clara, lousa, giz, feltro, aço | - |
| Chave | `key.c` | argola em **toro**, haste, palhetão; pendurada no nó da gaveta (anda junto) | latão envelhecido | aparece ao abrir a gaveta |
| Globo | `globe.c` | pedestal **torneado**, anel, **esfera** num grupo inclinado 23,5° | madeira, latão, mapa | **E**: gira e freia (rotação) |
| Abajur | `lamp.c` | base de cerâmica torneada, soquete, lâmpada, corrente, cúpula em casca torneada | cerâmica, latão, linho, vidro | **E**: liga → luz pontual e cúpula acesa (iluminação) |
| Fogo da lareira | `fire.c` | grelha, toras irregulares (torno), carvões; chamas em billboards com **shader de fogo procedural** (`shaders/fire.frag`) | ferro, lenha carbonizada com mapa de emissão, carvão | **E**: o fogo cresce (escala) + luz que tremula |

### Personagem (importado)

O personagem é o **Wesker**, importado do RE1 original (o único modelo importado da cena).
O arquivo `.EMD` do jogo foi convertido para um OBJ com as partes do corpo, a hierarquia
(`skeleton.txt`) e a textura. Em C, `src/character.c` lê esses
arquivos e monta cada parte como um nó do grafo de cena; `src/player.c` faz o controle tanque e
a **animação de andar** (rotação de coxas, canelas, pés e braços nas articulações, em seno).
Os arquivos do jogo não vão para o repositório; veja
[assets/characters/README.md](assets/characters/README.md).

Primitivas geradas em `src/mesh.c`: caixa, cilindro, toro, superfície de revolução (torno) e
esfera (semicírculo girado no torno, com UV de mapa-múndi).

Todas as texturas são procedurais (ruído de valor, anéis senoidais, máscaras), geradas por
`python tools/gen_textures.py`: madeiras, metal pintado, couro, lousa com giz, mapa antigo, linho e lenha carbonizada
(com mapa de emissão das rachaduras em brasa). As UVs das caixas são em metros, então a textura não estica
em peças de tamanhos diferentes.

## Estrutura

```
src/
  main.c         janela, loop, entrada
  character.c/h  leitor do personagem importado (OBJ por partes + esqueleto)
  player.c/h     controle tanque e animação de andar do Wesker
  zones.c/h      troca automática de câmera (assets/zones.txt)
  room.c/h       paredes invisíveis da sala (assets/room.txt)
  sound.c/h      trilha, passos e sons das interações (miniaudio)
  shadow.c/h     sombras "blob" no chão (multiplicadas sobre o fundo)
  mathlib.c/h    vec3, mat4, translação/rotação/escala, perspectiva, lookAt
  shader.c/h     carregar/compilar shaders, uniforms
  texture.c/h    carregar texturas (stb_image), capturas de tela
  mesh.c/h       VAO/VBO, cubo
  camera.c/h     ângulos de câmera fixos (assets/cameras.txt)
  background.c/h fundo pré-renderizado em tela cheia
  calib.c/h      modo de calibração (grade)
  renderer.c/h   shader iluminado + struct Material (Phong)
  node.c/h       grafo de cena: peças com transformação relativa ao pai
  materials.c/h  materiais da cena (madeira envernizada, crua, latão...)
  scene.c/h      cria/anima/desenha os objetos e trata a interação (E)
  objects/       um arquivo por objeto modelado
    *.c          um objeto por arquivo (ver tabela acima)
shaders/         GLSL (fundo, linhas, iluminação Blinn-Phong, fogo procedural, sombras)
assets/          cameras.txt, fundos (fora do Git), texturas
tools/           gen_textures.py (gera as texturas procedurais)
third_party/     GLFW 3.4 (binário MinGW), GLAD 2 (GL 3.3 core), stb, miniaudio
```

## Créditos

- Fundos: *Resident Evil* (1996) © Capcom, upscale do fan mod **Resident Evil - Seamless HD Project** (equipe RESHDP). Não incluídos no repositório.
- [GLFW](https://www.glfw.org) (zlib), [miniaudio](https://miniaud.io) (domínio público / MIT-0), [GLAD](https://github.com/Dav1dde/glad) (MIT), [stb](https://github.com/nothings/stb) (domínio público / MIT).
