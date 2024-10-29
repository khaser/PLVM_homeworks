{
  description = "PLVM course flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem ( system:
    let
      pkgs = import nixpkgs { inherit system; };
      buildDunePackage = pkgs.ocamlPackages.callPackage ./dune.nix {};
      logger-p5 = pkgs.callPackage ./logger.nix {};
      GT = pkgs.callPackage ./GT.nix { inherit buildDunePackage logger-p5; };
      ostap = pkgs.callPackage ./ostap.nix { inherit buildDunePackage GT; };
    in {
      devShell = pkgs.mkShell {
        name = "plvm-lama";

        nativeBuildInputs = [ pkgs.gcc_multi ] ++ (with pkgs.ocamlPackages; [
          findlib
          ocaml
          dune_3
          camlp5
          ostap
          GT
        ]);

      };
    });
}

        

