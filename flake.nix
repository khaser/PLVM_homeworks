{
  description = "PLVM course flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs-unstable.url = "github:NixOS/nixpkgs/nixos-24.11";
  };

  outputs = { self, nixpkgs, nixpkgs-unstable, flake-utils }:
    flake-utils.lib.eachDefaultSystem ( system:
    let
      pkgs = import nixpkgs { inherit system; };
      pkgs-unstable = import nixpkgs-unstable { inherit system; };
      buildDunePackage = pkgs.ocamlPackages.callPackage ./dune.nix {};
      logger-p5 = pkgs.callPackage ./logger.nix {};
      GT = pkgs.callPackage ./GT.nix { inherit buildDunePackage logger-p5; };
      ostap = pkgs.callPackage ./ostap.nix { inherit buildDunePackage GT; };
      graalvm = pkgs-unstable.graalvm-ce;
    in {
      devShell = pkgs.mkShell {
        name = "plvm-lama";

        nativeBuildInputs = with pkgs; [
          gdb gcc_multi
          clang-tools_16
          graalvm
          (pkgs-unstable.gradle.override {
            java = graalvm;
          })
          jetbrains.idea-community
        ] ++ (with pkgs.ocamlPackages; [
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
