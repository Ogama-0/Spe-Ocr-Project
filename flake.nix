{
  inputs = { nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05"; };

  outputs = { self, nixpkgs }:
    let
      inherit (nixpkgs.lib) genAttrs;

      forAllSystems =
        genAttrs [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" ];

      pkgs = forAllSystems (system:
        import nixpkgs {
          inherit system;
          overlays = [ ];
        });
    in {
      formatter = forAllSystems (system: pkgs.${system}.nixpkgs-fmt);

      devShells = forAllSystems (system:
        let
          pkgsForSystem = pkgs.${system};
          lib = pkgsForSystem.lib;
          mkClangShell = pkgsForSystem.mkShell.override {
            stdenv = pkgsForSystem.clangStdenv;
          };
        in {
          default = mkClangShell rec {
            nativeBuildInputs = with pkgsForSystem;
              [ clang-tools pkg-config ] ++ (with llvmPackages; [ clang lldb ]);

            buildInputs = with pkgsForSystem; [
              SDL2
              # sdl3
              gdk-pixbuf
              gdk-pixbuf.dev
            ];

            LD_LIBRARY_PATH = lib.makeLibraryPath buildInputs;
            PKG_CONFIG_PATH = lib.makeSearchPath "lib/pkgconfig" buildInputs;
          };
        });
    };
}
