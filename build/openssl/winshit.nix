{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-unstable",
    version ? "3.4.0"
}:

let
    # x86_64-unknown-linux-gnu - amd/intel 64 bit cpu linux
    # aarch64-linux-gnu - arm 64 bit cpu linux
    # x86_64-w64-mingw32 - amd/intel 64 bit cpu windows

    pkgs_cross = import nixpkgs { crossSystem = { config = "x86_64-w64-mingw32"; };};
    pkgs_native = import nixpkgs { };
in

pkgs_cross.stdenv.mkDerivation {

    name = "openssl";
    version = version;

    src = pkgs_native.fetchurl {
        url = "https://github.com/openssl/openssl/releases/download/openssl-${version}/openssl-${version}.tar.gz";
        sha256 = "4V3agv4v6BOdwqwho21MoB1TE8dfmfRsTooncJtylL8=";
        curlOpts = "-L";               #follow redirects cuz github 302
    };

    nativeBuildInputs = [
        pkgs_native.perl
    ];

    unpackPhase = "tar -xzf $src";
    sourceRoot = "./openssl-3.4.0";
    configurePhase = "CFLAGS='-fPIC -pie -DPIC ' ./Configure --prefix=$out mingw64 && perl configdata.pm --dump";
    buildPhase = "make -j$(nproc)";
    installPhase = "make install";
}