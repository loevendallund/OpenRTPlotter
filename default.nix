with import <nixpkgs> {};

let
  unstable = import
    (fetchTarball
      https://github.com/NixOS/nixpkgs-channels/archive/nixos-unstable.tar.gz) {};
in
stdenv.mkDerivation {
  name = "chemilang";
  src = ./.;

  buildInputs = [ cmake clang gtest glew unstable.glfw freetype glm ];

  buildPhase = "cmake . && make";

}
