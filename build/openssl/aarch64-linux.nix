{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-unstable",
    version ? "3.4.0"
}:

let
    # x86_64-unknown-linux-gnu - amd/intel 64 bit cpu linux
    # aarch64-linux-gnu - arm 64 bit cpu linux
    # x86_64-w64-mingw32 - amd/inte 64 bit cpu windows

    hashes = import ./tarball_hashes.nix;
    pkgs_cross = import nixpkgs { crossSystem = { config = "aarch64-unknown-linux-gnu"; }; };
    pkgs_native = import nixpkgs { };
in

pkgs_cross.stdenv.mkDerivation {

    name = "openssl";
    version = version;

    src = pkgs_native.fetchurl {
        url = "https://github.com/openssl/openssl/releases/download/openssl-${version}/openssl-${version}.tar.gz";
        sha256 = hashes.${version};
        curlOpts = "-L";               #follow redirects cuz github 302
    };

    nativeBuildInputs = [
        pkgs_native.perl
    ];


    unpackPhase = "tar -xzf $src";
    sourceRoot = "./openssl-${version}";
    configurePhase = "CFLAGS='-fPIC -pie -DPIC ' ./Configure --prefix=$out linux-aarch64 && perl configdata.pm --dump";
    buildPhase = "make -j$(nproc)";
    installPhase = "make install";
}