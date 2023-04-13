{
  description = "A flake for netShip cpp project";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; overlays = [
        (final: prev: {netShip = final.callPackage ./default.nix {};})
      ];};
    in 
    {
      packages.x86_64-linux.default = pkgs.netShip;
    };
}