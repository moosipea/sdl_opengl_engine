echo Building project

# copy assets folder to build folder
cp ./src/assets ./output -r

g++ ./src/main.cpp -o ./output/out -lSDL2 -lGL -lGLEW -lGLU

# also run built executable
cd ./output
./out
