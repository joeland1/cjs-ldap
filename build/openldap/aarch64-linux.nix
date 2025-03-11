{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11",
    version ? "2.5.19"
}:

let

    pkgs_cross = import nixpkgs { crossSystem = { config = "aarch64-unknown-linux-gnu"; }; };
    pkgs_native = import nixpkgs { };
    openssl_static = import ../openssl/import.nix { target = "aarch64-unknown-linux-gnu"; };
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

        CFLAGS='-fPIC -pie -DPIC -I${openssl_static}/include -L${openssl_static}/lib -static-libgcc -static-libstdc++'  \
        CPPFLAGS='-I${openssl_static}/include -L${openssl_static}/lib'                                                  \
        LDFLAGS='-I/${openssl_static}/include -L${openssl_static}/lib'                                                  \
        ./configure                                                                                                     \
            ac_cv_func_memcmp_working=yes                                                                               \
            --prefix=$out --enable-static --with-gnu-ld --host=aarch64-unknown-linux-gnu --with-yielding_select=yes

    '';
    buildPhase = "make -j$(nproc)";
    installPhase = "make install";
}