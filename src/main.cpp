#include "ChessAI/GreedyBot.h"
#include "EngineCore/ChessEngine.h"
#include "EngineCore/UCIHandler.h"
#include <bits/stdc++.h>
using namespace std;

int main() {
    ChessEngine engine;
    GreedyBot bot(engine, static_cast<int>(time(nullptr)));
    const UCIHandler uci(engine, bot);
    uci.start();
}
