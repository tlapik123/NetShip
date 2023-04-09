{
  description = "A flake for netShip cpp project";

  outputs = { self, nixpkgs }:
    let
      # Systems supported
      allSystems = [
        "x86_64-linux" # 64-bit Intel/AMD Linux
      ];

      # Helper to provide system-specific attributes
      forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in 
    {
        packages = forAllSystems ({ pkgs }: {
            default =
                let
                    binName = "netShip";
                    buildDependencies = with pkgs; [ 
                        # for debug
                        breakpointHook 
                        # essential for build
                        pkg-config
                        gcc
                        ninja
                        cmake
                    ];
                    runDependencies = with pkgs; [ 
                        asio 
                    ];
                in
                pkgs.stdenv.mkDerivation {
                    name = binName;
                    src = self;
                    buildInputs = runDependencies;
                    nativeBuildInputs = buildDependencies;
                    doCheck = true;
                    # configurePhase = ''
                    #     cmake .
                    # '';

                    # buildPhase = ''
                    #     make
                    # '';

                    # installPhase = ''
                    #     mkdir -p $out/bin
                    #     mv netShip $out/bin
                    # '';
            };
        });
    };
}