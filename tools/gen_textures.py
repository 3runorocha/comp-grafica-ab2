"""Gera as texturas procedurais do projeto.

Uso: python tools/gen_textures.py
"""
import os

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

SIZE = 512
OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")


def value_noise(rng, cells_x, cells_y, size=SIZE):
    """Ruido de valor tileavel."""
    grid = rng.random((cells_y, cells_x))
    xs = np.arange(size) * cells_x / size
    ys = np.arange(size) * cells_y / size
    x0 = np.floor(xs).astype(int)
    y0 = np.floor(ys).astype(int)
    fx = xs - x0
    fy = ys - y0
    fx = fx * fx * (3 - 2 * fx)
    fy = fy * fy * (3 - 2 * fy)
    x1 = (x0 + 1) % cells_x
    y1 = (y0 + 1) % cells_y
    a = grid[np.ix_(y0, x0)]
    b = grid[np.ix_(y0, x1)]
    c = grid[np.ix_(y1, x0)]
    d = grid[np.ix_(y1, x1)]
    top = a + (b - a) * fx[None, :]
    bot = c + (d - c) * fx[None, :]
    return top + (bot - top) * fy[:, None]


def fbm(rng, cells_x, cells_y, octaves=4):
    """Ruido fractal."""
    total = np.zeros((SIZE, SIZE))
    amp, norm = 1.0, 0.0
    for _ in range(octaves):
        total += amp * value_noise(rng, cells_x, cells_y)
        norm += amp
        amp *= 0.5
        cells_x *= 2
        cells_y *= 2
    return total / norm


def wood(seed, dark, light, ring_count, wobble, fiber, dirt=0.0):
    """Madeira procedural."""
    rng = np.random.default_rng(seed)
    y = np.linspace(0, 1, SIZE, endpoint=False)[:, None]

    # aneis
    warp = fbm(rng, 2, 4) * wobble
    rings = np.sin((y + warp) * ring_count * 2 * np.pi) * 0.5 + 0.5
    rings = rings ** 2.5

    # fibras
    fibers = fbm(rng, 4, 128, octaves=3)

    t = np.clip(0.55 * rings + fiber * fibers + 0.15 * fbm(rng, 8, 8), 0, 1)
    color = dark[None, None, :] + (light - dark)[None, None, :] * t[..., None]

    # sujeira
    if dirt > 0:
        stains = np.clip((fbm(rng, 4, 4) - 0.45) * 3.0, 0, 1)
        color *= (1 - dirt * stains)[..., None]

    return Image.fromarray(np.clip(color * 255, 0, 255).astype(np.uint8), "RGB")


def painted_metal(seed, paint, steel, rust):
    """Metal pintado e gasto."""
    rng = np.random.default_rng(seed)
    scratches = fbm(rng, 4, 128, octaves=2)                   # riscos
    chip_noise = fbm(rng, 16, 16, octaves=5)
    chips = np.clip((chip_noise - 0.66) * 25.0, 0, 1)         # lascas
    rust_mask = np.clip((chip_noise - 0.60) * 6.0, 0, 1) * np.clip((fbm(rng, 6, 6) - 0.45) * 4.0, 0, 1)
    shade = 0.85 + 0.15 * fbm(rng, 3, 3) + 0.05 * (scratches - 0.5)

    color = paint[None, None, :] * shade[..., None]
    color = color * (1 - rust_mask[..., None]) + rust[None, None, :] * rust_mask[..., None]
    color = color * (1 - chips[..., None]) + steel[None, None, :] * (0.8 + 0.4 * scratches[..., None]) * chips[..., None]
    return Image.fromarray(np.clip(color * 255, 0, 255).astype(np.uint8), "RGB")


def leather(seed, base):
    """Couro gasto."""
    rng = np.random.default_rng(seed)
    pores = fbm(rng, 64, 64, octaves=2)
    wrinkles = np.abs(fbm(rng, 8, 8, octaves=4) - 0.5) * 2.0       # rugas
    wear = np.clip((fbm(rng, 3, 3) - 0.5) * 2.5, 0, 1)
    shade = 0.75 + 0.25 * pores - 0.25 * np.clip(0.12 - wrinkles, 0, 1) / 0.12 + 0.25 * wear
    color = base[None, None, :] * shade[..., None]
    return Image.fromarray(np.clip(color * 255, 0, 255).astype(np.uint8), "RGB")


def load_font(size):
    """Carrega fonte manuscrita."""
    for name in ("segoepr.ttf", "comic.ttf", "arial.ttf", "DejaVuSans.ttf"):
        try:
            return ImageFont.truetype(name, size)
        except OSError:
            continue
    return ImageFont.load_default(size=size)


def chalkboard(seed, w=1024, h=740):
    """Lousa com escrita a giz."""
    rng = np.random.default_rng(seed)
    base = np.array([0.13, 0.19, 0.16])
    grain = value_noise(rng, 48, 48, size=max(w, h))[:h, :w]
    board = base[None, None, :] * (0.85 + 0.3 * grain)[..., None]

    # marcas de apagador
    smudge = Image.new("L", (w, h), 0)
    d = ImageDraw.Draw(smudge)
    for _ in range(9):
        x, y = rng.integers(0, w), rng.integers(0, h)
        rx, ry = rng.integers(80, 260), rng.integers(30, 90)
        d.ellipse([x - rx, y - ry, x + rx, y + ry], outline=int(rng.integers(70, 130)), width=int(rng.integers(18, 40)))
    smudge = np.asarray(smudge.filter(ImageFilter.GaussianBlur(18)), dtype=float) / 255.0

    # escrita e cubo
    ink = Image.new("L", (w, h), 0)
    d = ImageDraw.Draw(ink)
    font_big = load_font(130)
    d.text((70, 150), "tá joia?", font=font_big, fill=255)
    cube = [(640, 420), (840, 420), (840, 620), (640, 620)]
    back = [(x + 80, y - 70) for x, y in cube]
    for poly in (cube, back):
        d.line(poly + [poly[0]], fill=255, width=7)
    for a_, b_ in zip(cube, back):
        d.line([a_, b_], fill=255, width=7)
    ink = np.asarray(ink.filter(ImageFilter.GaussianBlur(1.2)), dtype=float) / 255.0

    # falhas do giz
    breaks = np.clip((value_noise(rng, 160, 160, size=max(w, h))[:h, :w] - 0.25) * 2.5, 0, 1)
    chalk = np.clip(ink * (0.55 + 0.45 * breaks) * 0.9 + smudge * 0.3, 0, 1)

    # composicao final
    chalk_color = np.array([0.86, 0.88, 0.85])
    color = board * (1 - chalk[..., None]) + chalk_color[None, None, :] * chalk[..., None]
    return Image.fromarray(np.clip(color * 255, 0, 255).astype(np.uint8), "RGB")


def antique_globe(seed, w=1024, h=512):
    """Mapa antigo do globo."""
    rng = np.random.default_rng(seed)
    # continentes
    big = fbm(rng, 6, 6, octaves=5)
    land_f = np.asarray(Image.fromarray((big * 255).astype(np.uint8)).resize((w, h), Image.BILINEAR),
                        dtype=float) / 255.0
    lat = np.linspace(-1, 1, h)[:, None]
    land_f = land_f - 0.12 * lat ** 4                     # menos terra nos polos
    land = land_f > 0.56

    # contorno da costa
    land_img = Image.fromarray((land * 255).astype(np.uint8))
    coast = np.asarray(land_img.filter(ImageFilter.MaxFilter(5)), dtype=float) / 255.0 - land
    detail = np.asarray(Image.fromarray((fbm(rng, 24, 24) * 255).astype(np.uint8)).resize((w, h)),
                        dtype=float) / 255.0

    # oceano e terra
    ocean = np.array([0.42, 0.52, 0.48]) * (0.9 + 0.15 * detail)[..., None]
    ground = np.array([0.80, 0.70, 0.50]) * (0.85 + 0.25 * detail)[..., None]
    color = np.where(land[..., None], ground, ocean)
    color = color * (1 - 0.8 * coast[..., None]) + np.array([0.30, 0.20, 0.12]) * 0.8 * coast[..., None]

    # meridianos e paralelos
    grid = np.zeros((h, w))
    for k in range(1, 6):
        grid[int(h * k / 6) - 1:int(h * k / 6) + 1, :] = 1
    for k in range(12):
        grid[:, int(w * k / 12):int(w * k / 12) + 2] = 1
    grid[h // 2 - 2:h // 2 + 2, :] = 1                    # equador
    color = color * (1 - 0.45 * grid[..., None]) + np.array([0.35, 0.25, 0.15]) * 0.45 * grid[..., None]

    # manchas de papel velho
    stains = np.asarray(Image.fromarray((fbm(rng, 4, 4) * 255).astype(np.uint8)).resize((w, h)),
                        dtype=float) / 255.0
    color *= (0.8 + 0.25 * stains)[..., None] * np.array([1.0, 0.96, 0.85])
    return Image.fromarray(np.clip(color * 255, 0, 255).astype(np.uint8), "RGB")


def linen(seed, base):
    """Tecido de linho."""
    rng = np.random.default_rng(seed)
    x = np.arange(SIZE)
    warp = 0.5 + 0.5 * np.sin(x * 2 * np.pi / 4.0)          # fios
    weave = 0.5 * (warp[None, :] + warp[:, None])
    slubs = fbm(rng, 4, 64, octaves=2)                        # fios irregulares
    stains = fbm(rng, 3, 3)
    shade = 0.82 + 0.12 * weave + 0.1 * (slubs - 0.5) - 0.1 * np.clip(stains - 0.6, 0, 1)
    color = base[None, None, :] * shade[..., None]
    return Image.fromarray(np.clip(color * 255, 0, 255).astype(np.uint8), "RGB")


def charred_log(seed):
    """Lenha queimada e mapa de brasa."""
    rng = np.random.default_rng(seed)
    fissure_n = fbm(rng, 40, 3, octaves=4)
    fissures = np.clip(1.0 - np.abs(fissure_n - 0.5) * 9.0, 0, 1)   # fissuras
    ash = np.clip((fbm(rng, 6, 6, octaves=5) - 0.5) * 3.0, 0, 1)
    grain = fbm(rng, 64, 16, octaves=2)

    # casca
    bark = np.array([0.075, 0.06, 0.05]) * (0.7 + 0.6 * grain)[..., None]
    bark = bark * (1 - ash[..., None]) + np.array([0.42, 0.40, 0.38]) * (0.8 + 0.3 * grain)[..., None] * ash[..., None]
    bark *= (1 - 0.7 * fissures)[..., None]

    # brasa
    hot = fissures * np.clip((fbm(rng, 5, 5) - 0.35) * 2.5, 0, 1) * (1 - ash)
    glow = np.stack([hot, hot * 0.55, hot * 0.15], axis=-1)
    to_img = lambda a: Image.fromarray(np.clip(a * 255, 0, 255).astype(np.uint8), "RGB")
    return to_img(bark), to_img(glow)


def main():
    """Gera e salva as texturas."""
    os.makedirs(OUT, exist_ok=True)
    textures = {
        # escrivaninha
        "wood_varnish.png": wood(1, np.array([0.20, 0.09, 0.04]), np.array([0.46, 0.24, 0.12]),
                                 ring_count=7, wobble=0.35, fiber=0.35),
        # caixote
        "wood_raw.png": wood(2, np.array([0.36, 0.26, 0.15]), np.array([0.68, 0.54, 0.35]),
                             ring_count=4, wobble=0.25, fiber=0.45, dirt=0.45),
        # escadinha
        "metal_painted.png": painted_metal(3, np.array([0.36, 0.40, 0.38]), np.array([0.55, 0.56, 0.58]),
                                           np.array([0.36, 0.17, 0.07])),
        # assento da cadeira
        "leather.png": leather(4, np.array([0.38, 0.10, 0.08])),
        # quadro-negro
        "chalkboard.png": chalkboard(5),
        # globo
        "globe_map.png": antique_globe(6),
        # cupula do abajur
        "linen.png": linen(7, np.array([0.86, 0.78, 0.62])),
    }
    # lenha da lareira
    textures["charred_log.png"], textures["charred_log_glow.png"] = charred_log(8)
    # salva os arquivos
    for name, img in textures.items():
        path = os.path.join(OUT, name)
        img.save(path, optimize=True)
        print("gerado", os.path.normpath(path))


if __name__ == "__main__":
    main()
