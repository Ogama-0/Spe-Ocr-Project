import os
from PIL import Image

def rescale_pngs_in_dir(root_dir):
    """
    Parcourt récursivement un dossier et redimensionne toutes les images PNG à 27x27 pixels.
    Si l'image est plus petite, elle est centrée sur un fond noir sans être étirée.
    Si elle est plus grande, elle est redimensionnée proportionnellement pour tenir dans 27x27.
    L'image originale est alors remplacée.
    """

    target_size = (27, 27)

    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.lower().endswith('.png'):
                img_path = os.path.join(dirpath, filename)
                print(f"{filename}")

                # Ouvrir l'image
                with Image.open(img_path) as img:
                    img = img.convert('L')  # Convertir en niveaux de gris (si besoin)

                    # Calcul du ratio pour conserver les proportions
                    ratio = min(target_size[0] / img.width, target_size[1] / img.height)
                    new_size = (int(img.width * ratio), int(img.height * ratio))

                    # Redimensionner sans étirer
                    resized = img.resize(new_size, Image.Resampling.LANCZOS)

                    # Créer une image noire de fond
                    new_img = Image.new('L', target_size, color=0)

                    # Calculer la position pour centrer
                    offset = ((target_size[0] - new_size[0]) // 2,
                              (target_size[1] - new_size[1]) // 2)
                    new_img.paste(resized, offset)

                    # Sauvegarder en remplaçant l'image originale
                    new_img.save(img_path)

    print(f"Toutes les images PNG dans '{root_dir}' ont été redimensionnées à {target_size}.")


if __name__ == "__main__" :
    rescale_pngs_in_dir("/home/ogama/documents/epita/OCR-proj-2025/Spe-Ocr-Project/src/dataset_processed")

