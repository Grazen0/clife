{
  description = "An implementation of Conway's Game of Life in C using a Quadtree.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let

      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      inherit (pkgs) lib;
    in
    {

      packages.${system} = {
        clife = pkgs.stdenv.mkDerivation {
          pname = "clife";
          version = "main";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            meson
            ninja
            pkg-config
          ];
          buildInputs = with pkgs; [ raylib ];

          meta = with lib; {
            description = "An implementation of Conway's Game of Life in C using a Quadtree.";
            homepage = "https://github.com/Grazen0/clife";
            license = licenses.gpl3;
          };
        };

        default = self.packages.${system}.clife;
      };

      devShells.${system}.default = pkgs.mkShell {
        inputsFrom = [ self.packages.${system} ];

        packages = with pkgs; [ valgrind ];

        hardeningDisable = [ "fortify" ];
      };
    };
}
