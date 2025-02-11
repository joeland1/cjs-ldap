{ 
    pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11"){}
}:

pkgs.stdenv.mkDerivation {

    name = "openssl";
    version = "3.4.0";

    src = pkgs.fetchurl {
        url = "https://github.com/openssl/openssl/releases/download/openssl-3.4.0/openssl-3.4.0.tar.gz";
        sha256 = "4V3agv4v6BOdwqwho21MoB1TE8dfmfRsTooncJtylL8=";
        curlOpts = "-L";               #follow redirects cuz github 302
    };

    buildInputs = [
        pkgs.perl
    ];

    unpackPhase = "tar -xzf $src";
    sourceRoot = "./openssl-3.4.0";
    configurePhase = "CFLAGS='-fPIC -pie -DPIC ' ./Configure --prefix=$out";
    buildPhase = "make -j$(nproc)";
    installPhase = "make install";
}