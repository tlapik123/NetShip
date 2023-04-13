{ lib
, stdenv
# build stuff
, pkg-config
, gcc
, ninja
, cmake
# libs
, asio
, ftxui
}:

stdenv.mkDerivation {
    name = "netShip";
    src = ./.;
    buildInputs = [ 
        asio
        ftxui
    ];
    nativeBuildInputs = [ 
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
}