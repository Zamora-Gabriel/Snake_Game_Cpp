#include "Game.h"
#include "EngineInterface.h"
#include <time.h>

int main()
{

    srand((unsigned)time(0));
    exEngineInterface* engine = AccessEngine();

    if (engine == nullptr)
    {
        return 0;
    }

    engine->Run(Game::GetInstance());

    return 0;
}