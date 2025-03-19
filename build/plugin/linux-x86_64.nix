{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11",
    version ? "unknown"
}:


let

    pkgs_native = import nixpkgs {};
    pkgs_cross = import nixpkgs { crossSystem = { config = "x86_64-unknown-linux-gnu"; }; };

    fs   = pkgs_native.lib.fileset;
    napi = import ../node-addon.nix { };
    ldap = import ../openldap/import.nix { target = "x86_64-unknown-linux-gnu" ; };
    node = pkgs_native.nodejs_21;

    cpp_h_pair = fname: pkgs_native.lib.lists.flatten ([
        (pkgs_native.lib.concatStrings [ fname ".h" ] )
        (pkgs_native.lib.concatStrings [ fname ".cpp"] )
    ]);
in

pkgs_cross.stdenv.mkDerivation {

    name = "cjs-ldap";
    version = version;

    buildInputs = [
        napi
        node
        ldap
    ];

    src = fs.toSource {
        root = ./../../ldap-async-addon;
        fileset = fs.unions (map (x: ./. + "./../../ldap-async-addon/${x}" ) (pkgs_native.lib.lists.flatten [
                "index.js"
                "package.json"
                "build.sh"
                "addon.cpp"
                "utils.h"
                "assert.h"
                "log.h"
                (cpp_h_pair "client")
                (cpp_h_pair "async-bind-ldap")
                (cpp_h_pair "async-ldap-search")
                (cpp_h_pair "async-ldap-close")
                (cpp_h_pair "ldap-result")
                (cpp_h_pair "ldap-result-obj")
                (cpp_h_pair "search_values")
                (cpp_h_pair "tls_validation_settings")
            ]));
    };

    buildPhase = ''
        NODE_INCLUDE_PATH=${node}/include/node
        NAPI_INCLUDE_PATH=${napi}
        LDAP_INCLUDE_PATH=${ldap}/include
        LDAP_LIB_PATH=${ldap}/lib
        GPP_FLAGS="-I$NODE_INCLUDE_PATH -I$NAPI_INCLUDE_PATH -I$LDAP_INCLUDE_PATH -static -fPIC -std=c++23 -latomic -c"

        # for some reason need to directly specify the ld and g++ even though they should be symlinked already via the stdenv
        GPP_FLAGS="$GPP_FLAGS" ./build.sh
    '';
    installPhase = ''
        mkdir $out
        cp addon.node $out
        cp index.js $out
        cp package.json $out
    '';

}
