{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-unstable",
}:

let
    pkgs_cross = import nixpkgs { crossSystem = { config = "x86_64-w64-mingw32"; };};
    pkgs_native = import nixpkgs { };
in

# sed -Ei 's/#define ([0-9a-zA-Z]+) rpl_\1//g' config.h
# CC='x86_64-w64-mingw32-gcc-posix' CFLAGS='-fPIC -pie -DPIC -static' ./configure --prefix='/root/regex_build' --host='x86_64-w64-mingw32' --enable-static
# cp /tmp/testdir/gllib/libgnu.a /code/regexshit/lib/libgnu.a && cp /tmp/testdir/gllib/libgnu.a /code/regexshit/lib/libgnuregex.a

pkgs_cross.stdenv.mkDerivation {

    name = "gnulib";
    version = "1.0.0";

    src = /code/regexshit;

    installPhase = ''
        mkdir -p $out/lib

        cp -rv $src/* $out
  '';

}