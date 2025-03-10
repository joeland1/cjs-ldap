{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-unstable",
    version ? "2.6.9",
}:

let
    pkgs_cross = import nixpkgs { crossSystem = { config = "x86_64-w64-mingw32"; }; };
    pkgs_native = import nixpkgs { };
    openssl_static = import ../openssl/import.nix { target = "x86_64-w64-mingw32"; };
    regex = import ./fake-gnulib.nix {};
in

# DLLTOOL='x86_64-w64-mingw32-dlltool' CC='x86_64-w64-mingw32-gcc' CFLAGS='-fPIC -pie -DPIC -I/code/regexshit/include -static' CPPFLAGS='-I/code/regexshit/include' LDFLAGS='-L/code/regexshit/lib -static' ./configure --prefix=/code/openldap-2.6.9/zzzzz --host=x86_64-w64-mingw32 --enable-static --disable-slapd --with-gnu-ld

pkgs_cross.stdenv.mkDerivation {

    name = "openldap";
    version = version;
    dontWrap = true;

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

    CC="${pkgs_cross.stdenv.cc}/bin/x86_64-w64-mingw32-gcc-posix";
    CXX="${pkgs_cross.stdenv.cc}/bin/x86_64-w64-mingw32-g++-posix";
    CFLAGS="-fPIC -pie -DPIC -I${regex}/include -static";
    LDFLAGS="-L/code/regexshit/lib -static";

    unpackPhase = "tar -xzf $src";
    sourceRoot = "./openldap-${version}";
    configurePhase = ''
        NIX_DEBUG=1 ./configure --prefix=$out --host=x86_64-w64-mingw32 --enable-static --disable-slapd --disable-shared --with-gnu-ld
        cp /code/openldaptestfuckaround/include/ac/time.h include/ac/time.h
        sed -i 's/#define HAVE_FCNTL 1/\/\/removed have_fcntl/g' include/portable.h
        sed -i 's/#define HAVE_FCNTL_H 1/\/\/removed have_fcntl_h/g' include/portable.h
        #sed -i 's/#define NEED_MEMCMP_REPLACEMENT 1/\/\/removed memcmp_replacement/g' include/portable.h
        #sed -i 's/#define HAVE_GETDTABLESIZE 1/\/\/removed have_getdtablesize/g' include/portable.h

    '';
    buildPhase = "make depend -j$(nproc) && make -j$(mproc)";
    installPhase = "make install";
}