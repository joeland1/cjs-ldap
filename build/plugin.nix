{ 
    nixpkgs ? fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11",
    target ? builtins.currentSystem,
    version ? "unknown"
}:


let
    tgt = 
        if          builtins.elem target [ "aarch64-linux"  "aarch64-unknown-linux-gnu"   ] then "aarch64-unknown-linux-gnu"
        else if     builtins.elem target [ "x86_86-linux"   "x86_64-unknown-linux-gnu"    ] then "x86_64-unknown-linux-gnu"
        else if     builtins.elem target [ "x86_64-windows" "x86_64-w64-mingw32"          ] then "x86_64-w64-mingw32"
        else abort "invalid target";

    pkgs_native = import nixpkgs {};
    pkgs_cross = import nixpkgs { crossSystem = { config = tgt; }; };

    fs   = pkgs_native.lib.fileset;
    napi = import ./node-addon.nix { };
    ldap = import ./openldap/import.nix { target = tgt; };
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
        root = ./../ldap-async-addon;
        fileset = fs.unions (map (x: ./. + "./../ldap-async-addon/${x}" ) (pkgs_native.lib.lists.flatten [
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

        # for some reason need to directly specify the ld and g++ even though they should be symlinked already via the stdenv
        LD_BIN="$LD" \
        GPP_BIN="$CXX" \
        GPP_FLAGS="$GPP_FLAGS" \
        LDAP_LIB="${ldap}/lib" \
        GPP_FLAGS="$GPP_FLAGS" \
            ./build.sh
    '';
    installPhase = ''
        mkdir $out
        cp addon.node $out
        cp index.js $out
        cp package.json $out
    '';

}
