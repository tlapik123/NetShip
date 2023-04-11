{
  description = "A flake for netShip cpp project";

  outputs = { self, nixpkgs }:
    let
      # Systems supported
      allSystems = [
        "x86_64-linux" # 64-bit Intel/AMD Linux
        "aarch64-linux"
      ];

      # Helper to provide system-specific attributes
      forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in 
    {
        packages = forAllSystems ({ pkgs }: {
            default = with pkgs; import ./default.nix { inherit 
                lib 
                stdenv  
                asio 
                breakpointHook 
                pkg-config 
                gcc 
                ninja 
                cmake; 
            };
        });
    };
}