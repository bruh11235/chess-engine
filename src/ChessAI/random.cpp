#include "RandomBot.h"
#include "../EngineCore/ChessEngine.h"
#include "../EngineCore/UCIHandler.h"
#include <bits/stdc++.h>

using namespace std;

int main() {
    ChessEngine engine;
    RandomBot bot(engine);
    UCIHandler uci(engine, bot);
    uci.start();
}
