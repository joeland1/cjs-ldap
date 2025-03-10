{ 
    version ? "3.4.0",
    target ? builtins.currentSystem
}:

if          builtins.elem target [ "aarch64-linux"  "aarch64-unknown-linux-gnu"   ]   then import ./aarch64-linux.nix { version = version; }
else if     builtins.elem target [ "x86_86-linux"   "x86_64-unknown-linux-gnu"    ]   then import ./x86_64-linux.nix { version = version; }
else if     builtins.elem target [ "x86_64-windows" "x86_64-w64-mingw32"          ]   then import ./winshit.nix { version = version; }

else abort "invalid target"