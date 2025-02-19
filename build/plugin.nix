{ 
    pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11"){}
}:

let
    fs = pkgs.lib.fileset;
    napi = import ./node-addon.nix { };
    ldap = import ./openldap.nix {};
    node = pkgs.nodejs_21;

    cpp_h_pair = fname: pkgs.lib.lists.flatten ([
        (pkgs.lib.concatStrings [ fname ".h" ] )
        (pkgs.lib.concatStrings [ fname ".cpp"] )
    ]);
in

pkgs.stdenv.mkDerivation {

    name = "cjs-ldap";
    version = "0.0.1";

    buildInputs = [
        napi
        node
        ldap
    ];

    src = fs.toSource {
        root = ./../ldap-async-addon;
        fileset = fs.unions (map (x: ./. + "./../ldap-async-addon/${x}" ) (pkgs.lib.lists.flatten [
                "index.js"
                "package.json"
                "build.sh"
                "addon.cpp"
                "utils.h"
                "assert.h"
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

        GPP_FLAGS="-I$NODE_INCLUDE_PATH -I$NAPI_INCLUDE_PATH -I$LDAP_INCLUDE_PATH -static -fPIC -std=c++23 -outline-atomics -c"
        GPP_FLAGS="$GPP_FLAGS" ./build.sh
        ls
    '';
    installPhase = ''
        mkdir $out
        cp addon.node $out
        cp index.js $out
        cp package.json $out
    '';

}
