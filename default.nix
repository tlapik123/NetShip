{ lib
, stdenv
, asio
, breakpointHook
, pkg-config
, gcc
, ninja
, cmake
}:

stdenv.mkDerivation {
    name = "netShip";
    src = self;
    buildInputs = with pkgs; [ 
        asio 
    ];
    nativeBuildInputs = with pkgs; [ 
        # for debug
        breakpointHook 
        # essential for build
        pkg-config
        gcc
        ninja
        cmake
    ];
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