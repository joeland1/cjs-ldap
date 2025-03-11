{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11",
    version ? "unknown"
}:


let
    pkgs = import nixpkgs {};
    fs = pkgs.lib.fileset;
    aarch64 = import ./plugin/import.nix { target = "aarch64-unknown-linux-gnu"; };
    x86_64 = import ./plugin/import.nix { target = "x86_64-unknown-linux-gnu"; };
in

pkgs.stdenv.mkDerivation {

    name = "cjs-ldap-multiarch";
    version = version;

    buildInputs = [
        aarch64
        x86_64
    ];

    src = fs.toSource {
        root = ./../ldap-async-addon;
        fileset = fs.unions [
            ./../ldap-async-addon/package.json
            ./../ldap-async-addon/index.js
        ];
    };

    installPhase = ''
        mkdir -p $out/prebuilt

        cp ${aarch64}/addon.node $out/prebuilt/linux_aarch64.node
        cp ${x86_64}/addon.node $out/prebuilt/linux_amd64.node
        
        cp index.js $out
        cp package.json $out
    '';

}
