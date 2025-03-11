{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11",
    version ? "2.5.19"
}:

let

    pkgs_cross = import nixpkgs { crossSystem = { config = "x86_64-unknown-linux-gnu"; }; };
    pkgs_native = import nixpkgs { };
    openssl_static = import ../openssl/import.nix { target = "x86_64-unknown-linux-gnu"; };
in

pkgs_cross.stdenv.mkDerivation {

    name = "openldap";
    version = "2.5.19";

    src = pkgs_native.fetchurl {
        url = "https://www.openldap.org/software/download/OpenLDAP/openldap-release/openldap-2.5.19.tgz";
        sha256 = "VuKTbHFpqnVHz8k9XIfbRqoF6Y3uYyFZDDrakuH7tmw=";
    };

    buildInputs = [
        openssl_static
    ];

    nativeBuildInputs = [
        pkgs_native.groff
    ];

    unpackPhase = "tar -xzf $src";
    sourceRoot = "./openldap-2.5.19";
    configurePhase = ''
        CFLAGS='-fPIC -pie -DPIC -I${openssl_static}/include -L${openssl_static}/lib'   \
        CPPFLAGS='-I${openssl_static}/include -L${openssl_static}/lib'                  \
        LDFLAGS='-I/${openssl_static}/include -L${openssl_static}/lib'                  \
        ./configure --prefix=$out --enable-static --with-gnu-ld --host=x86_64-unknown-linux-gnu
    '';
    buildPhase = "make -j$(nproc)";
    installPhase = "make install";
}