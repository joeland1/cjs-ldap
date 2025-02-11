{ 
    pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11"){},
}:

let
    openssl_static = import ./openssl.nix { };
in
pkgs.stdenv.mkDerivation {

    name = "node-addon-api";
    version = "8.3.0";

    src = pkgs.fetchurl {
        url = "https://github.com/nodejs/node-addon-api/archive/refs/tags/v8.3.0.tar.gz";
        hash = "sha256-pd2758SgSqTUOCBeL5C/xHYEKVHo692saIPxI6fojK4=";
    };

    srcRoot = "./node-addon-api-8.3.0";
    installPhase = "cp -r . $out";
}