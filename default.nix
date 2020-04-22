with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "chemilang";
  src = ./.;

  buildInputs = [ cmake clang gtest glew glfw freetype glm ];

  buildPhase = "cmake . && make";

}
