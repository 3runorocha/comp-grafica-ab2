# Relatório - Lesson Room (CG AB2)

**Equipe:** ______________________

Cena 3D interativa no estilo dos *Resident Evil* clássicos: câmeras fixas sobre fundos
pré-renderizados, com objetos 3D modelados, iluminados e texturizados em tempo real.
Feita em **C11 + OpenGL 3.3 core**, sem motores. Bibliotecas: GLFW (janela e teclado),
GLAD (carregador do OpenGL), stb_image (leitura de PNG) e miniaudio (som).

## Modelagem

Os 9 objetos são gerados em código como **hierarquias de peças** (grafo de cena: cada nó tem
translação, rotação e escala relativas ao pai). Primitivas próprias em `src/mesh.c`: caixa,
cilindro, toro, esfera e **superfície de revolução** (um perfil 2D girado em torno de Y).

| Objeto | Técnica de destaque |
|---|---|
| Escrivaninha | gaveta como sub-grupo que desliza |
| Cadeira | encosto como sub-grupo inclinado |
| Caixote | 20+ ripas e diagonais a 45° |
| Escadinha | tubos inclinados (cilindro entre dois pontos) |
| Quadro-negro | lousa inclinada encostada no cavalete |
| Chave | argola em toro; filha do nó da gaveta |
| Globo | pedestal torneado; esfera num grupo inclinado 23,5° |
| Abajur | base e cúpula torneadas (cúpula em casca) |
| Lareira (lenha) | toras torneadas irregulares + chamas procedurais |

O personagem (Wesker) é o único modelo importado: convertido do arquivo do RE1 para um OBJ por
parte do corpo e a hierarquia, que montamos como nós.

## Transformações

- **Translação:** gaveta e cadeira deslizando; personagem andando.
- **Rotação:** globo girando com atrito; articulações na animação de andar; giro do personagem.
- **Escala:** chamas da lareira crescendo ao acender.

## Navegação e câmeras

Controle tanque com W/A/S/D. As 4 câmeras e as zonas de troca foram **lidas do arquivo da sala
do jogo original**, então a perspectiva 3D coincide com a dos fundos.
A câmera troca sozinha quando o personagem entra numa zona do chão (teste de ponto em polígono).
Colisão por caixas: paredes da sala, móveis do fundo e os objetos 3D.

## Interações (tecla E)

Gaveta (revela a chave), cadeira, globo, abajur e lareira. Gaveta e cadeira **empurram** o
personagem se ele estiver na frente.

## Iluminação e materiais

Shader **Blinn-Phong** (`shaders/lit.frag`) com luz direcional, ambiente e até 4 luzes pontuais
com atenuação (abajur, lareira tremulando e um **spot** que recria a arandela do fundo).
Cada objeto tem material próprio (ambiente, difusa, especular, brilho, emissão): madeira fosca,
latão brilhante, couro, cerâmica, ferro. A luz de base de cada câmera foi ajustada medindo o
brilho do fundo, com cor de lâmpada incandescente (2700 K).

## Texturas

Todas **procedurais** (`tools/gen_textures.py`): madeiras, metal pintado, couro, lousa com giz,
mapa do globo, linho e lenha carbonizada (com **mapa de emissão** nas rachaduras). As UVs são
em metros com `GL_REPEAT` e mipmaps, para a textura não esticar entre peças de tamanhos
diferentes. O fogo é um shader de ruído fractal em *billboards* com mistura aditiva.

## Extras

Sombras *blob* no chão (mistura multiplicativa), sons do jogo original sincronizados com os
passos e as interações.

## Créditos

Fundos e personagem: *Resident Evil* (1996) © Capcom, fundos em HD do mod *Seamless HD Project*.
Não estão no repositório (ver `assets/*/README.md`).
