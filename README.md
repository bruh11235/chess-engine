# Chess AI

A simple Chess AI built with C++

Uses universal chess interface (UCI)


## How to Run
1. Clone this repository:
2. Build the project:
```
g++ -std=c++23 -O2 -I..\EngineCore\util -I..\EngineCore -I..\ChessAI ..\main.cpp ..\ChessAI\MinimaxBot.cpp ..\EngineCore\util\Bitboard.cpp ..\EngineCore\util\Zobrist.cpp ..\EngineCore\ChessEngine.cpp ..\EngineCore\UCIHandler.cpp -o main.exe
```
3. Run in UCI mode
```
./main.exe
```

(Optional) Integrate into Lichess via https://github.com/lichess-bot-devs/lichess-bot