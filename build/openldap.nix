{ 
    pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11"){},
}:

let
    openssl_static = import ./openssl.nix { };
in
pkgs.stdenv.mkDerivation {

    name = "openldap";
    version = "2.5.19";

    src = pkgs.fetchurl {
        url = "https://www.openldap.org/software/download/OpenLDAP/openldap-release/openldap-2.5.19.tgz";
        sha256 = "VuKTbHFpqnVHz8k9XIfbRqoF6Y3uYyFZDDrakuH7tmw=";
    };

    buildInputs = [
        openssl_static
    ];

    nativeBuildInputs = [
        pkgs.groff
    ];

    unpackPhase = "tar -xzf $src";
    sourceRoot = "./openldap-2.5.19";
    configurePhase = ''
        CFLAGS='-fPIC -pie -DPIC -I${openssl_static}/include -L${openssl_static}/lib'   \
        CPPFLAGS='-I${openssl_static}/include -L${openssl_static}/lib'                  \
        LDFLAGS='-I/${openssl_static}/include -L${openssl_static}/lib'                  \
        ./configure --prefix=$out --enable-static --with-gnu-ld
    '';
    buildPhase = "make -j$(nproc)";
    installPhase = "make install";
}