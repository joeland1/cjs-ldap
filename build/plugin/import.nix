{ 
    version ? "unknown",
    target ? builtins.currentSystem
}:

if          builtins.elem target [ "aarch64-linux"  "aarch64-unknown-linux-gnu"   ]   then import ./linux-aarch64.nix { version = version; }
else if     builtins.elem target [ "x86_86-linux"   "x86_64-unknown-linux-gnu"    ]   then import ./linux-x86_64.nix { version = version; }

else abort "invalid target"