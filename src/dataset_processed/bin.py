import os
from PIL import Image

def binarize_pngs_in_dir(root_dir):
    """
    Parcourt récursivement root_dir.
    Pour chaque fichier .png :
      - le charge en RGB
      - remplace chaque pixel par noir ou blanc selon la moyenne RGB
      - remplace l'image sur disque.
    """
    threshold = 127.5  # milieu entre 0 et 255

    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.lower().endswith(".png"):
                print(filename)
                img_path = os.path.join(dirpath, filename)

                with Image.open(img_path) as img:
                    img = img.convert("RGB")
                    pixels = img.load()

                    width, height = img.size
                    for y in range(height):
                        for x in range(width):
                            r, g, b = pixels[x, y]
                            m = (r + g + b) / 3.0
                            if abs(255 - m) < abs(0 - m):  # plus proche de 255 que de 0
                                pixels[x, y] = (255, 255, 255)
                            else:
                                pixels[x, y] = (0, 0, 0)

                    img.save(img_path)
if __name__ == "__main__" :
    binarize_pngs_in_dir("/home/ogama/documents/epita/OCR-proj-2025/Spe-Ocr-Project/src/dataset_processed")
