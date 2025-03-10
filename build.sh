docker run --rm -it \
    -v $(pwd)/build:/code/build \
    -v $(pwd)/ldap-async-addon:/code/ldap-async-addon \
    -v $(pwd)/cjs-ldap:/final_package \
    nixos/nix bash -c 'PACKAGE_LOCATION=$(nix-build /code/build/plugin.nix) && cp $PACKAGE_LOCATION/* /final_package'