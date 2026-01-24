#!/usr/bin/env python3
"""
Bitmap font generator for RPG Seed.

Generates an 8x8 pixel bitmap font image from a TTF font.
Output: 128x48 PNG (16 chars per row, 6 rows, ASCII 32-127)

Usage:
    python generate_font.py [--font FONT_PATH] [--output OUTPUT_PATH] [--size SIZE]

Examples:
    python generate_font.py
    python generate_font.py --font /path/to/font.ttf
    python generate_font.py --font "Monaco" --size 10
"""

import argparse
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("Error: Pillow is required. Install with: pip install Pillow")
    sys.exit(1)


CHAR_WIDTH = 8
CHAR_HEIGHT = 8
CHARS_PER_ROW = 16
FIRST_CHAR = 32
TOTAL_CHARS = 96
ROWS = TOTAL_CHARS // CHARS_PER_ROW

IMAGE_WIDTH = CHAR_WIDTH * CHARS_PER_ROW
IMAGE_HEIGHT = CHAR_HEIGHT * ROWS

DEFAULT_OUTPUT = Path(__file__).parent.parent / "assets" / "fonts" / "font.png"

COMMON_MONOSPACE_FONTS = [
    "/System/Library/Fonts/Monaco.ttf",
    "/System/Library/Fonts/Menlo.ttc",
    "/System/Library/Fonts/Courier.ttc",
    "/Library/Fonts/Courier New.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
    "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
]


def find_system_font() -> str | None:
    """Find a suitable monospace system font."""
    for font_path in COMMON_MONOSPACE_FONTS:
        if Path(font_path).exists():
            return font_path
    return None


def load_font(font_path: str | None, size: int) -> ImageFont.FreeTypeFont | ImageFont.ImageFont:
    """Load a font from path or find a system font."""
    if font_path:
        if Path(font_path).exists():
            return ImageFont.truetype(font_path, size)
        try:
            return ImageFont.truetype(font_path, size)
        except OSError:
            print(f"Warning: Could not load font '{font_path}', trying system fonts...")

    system_font = find_system_font()
    if system_font:
        print(f"Using system font: {system_font}")
        return ImageFont.truetype(system_font, size)

    print("Warning: No TTF font found, using default bitmap font")
    return ImageFont.load_default()


def generate_font_image(font: ImageFont.FreeTypeFont | ImageFont.ImageFont) -> Image.Image:
    """Generate the bitmap font image."""
    image = Image.new("RGBA", (IMAGE_WIDTH, IMAGE_HEIGHT), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)

    for i in range(TOTAL_CHARS):
        char = chr(FIRST_CHAR + i)
        col = i % CHARS_PER_ROW
        row = i // CHARS_PER_ROW

        x = col * CHAR_WIDTH
        y = row * CHAR_HEIGHT

        bbox = font.getbbox(char)
        if bbox:
            char_width = bbox[2] - bbox[0]
            char_height = bbox[3] - bbox[1]
            offset_x = (CHAR_WIDTH - char_width) // 2 - bbox[0]
            offset_y = (CHAR_HEIGHT - char_height) // 2 - bbox[1]
        else:
            offset_x = 0
            offset_y = 0

        draw.text((x + offset_x, y + offset_y), char, fill=(255, 255, 255, 255), font=font)

    return image


def main():
    parser = argparse.ArgumentParser(description="Generate bitmap font for RPG Seed")
    parser.add_argument("--font", "-f", help="Path to TTF font file or font name")
    parser.add_argument("--output", "-o", default=str(DEFAULT_OUTPUT), help="Output PNG path")
    parser.add_argument("--size", "-s", type=int, default=8, help="Font size in points (default: 8)")
    parser.add_argument("--preview", "-p", action="store_true", help="Show preview after generation")
    args = parser.parse_args()

    print(f"Generating {IMAGE_WIDTH}x{IMAGE_HEIGHT} bitmap font...")
    print(f"  Character size: {CHAR_WIDTH}x{CHAR_HEIGHT}")
    print(f"  Characters: ASCII {FIRST_CHAR}-{FIRST_CHAR + TOTAL_CHARS - 1} ({TOTAL_CHARS} chars)")

    font = load_font(args.font, args.size)
    image = generate_font_image(font)

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    image.save(output_path, "PNG")

    print(f"Saved to: {output_path}")

    if args.preview:
        image.show()


if __name__ == "__main__":
    main()
