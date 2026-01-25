if (Test-Path bin/Blossom.exe) { Remove-Item bin/Blossom.exe }

# Compile
g++ src/*.cpp -o bin/Blossom.exe -std=c++20 -O0 -g -Wall -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

# Run only if compile succeeded
if ($?) {
    Set-Location bin
    ./Blossom.exe
    Set-Location ..
}
