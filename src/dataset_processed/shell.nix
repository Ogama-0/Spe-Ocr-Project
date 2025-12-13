{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  packages = with pkgs; [ (python3.withPackages (ps: with ps; [ pillow ])) ];

  shellHook = ''
    echo "Environnement Python avec Pillow prêt."
    echo "Tu peux lancer : python ton_script.py"
  '';
}

