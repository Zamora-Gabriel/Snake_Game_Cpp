#include "Game.h"

#include "Constants.h"
#include "EngineInterface.h"
#include "SDL.h"
#include <time.h>
#include <string>

#include <iostream>

#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8
#define WALLRIGHT 800
#define WALLLEFT 0
#define WALLTOP 600
#define WALLBOT 0

Game* Game::sInstance = nullptr;

int n = 0;
float x = 100;
float y = 100;
Game* Game::GetInstance()
{
    if (sInstance == nullptr)
    {
        sInstance = new Game();
    }
    return sInstance;
}

Game::Game() 
    : mEngine(nullptr)
    , mFontID(-1)
    , mWindowName("Game")
{}

Game::~Game()
{
    if (sInstance != nullptr) 
    {
        delete sInstance;
    }
}

void Game::Initialize(exEngineInterface* engine)
{
    mEngine = engine;

    // Set game over flag to false
    gameOver = false;

    // Snake's position
    mBodyPos.emplace_back(exVector2(100, 100));
    // Snake's head size
    mSnakeHead = exVector2(5, 5);

    // Snake's Body size
    mSnakeBody = exVector2(4, 4);

    // Start at true to create one
    isFoodEaten = true;

    mFontID = mEngine->LoadFont("resources/Urusans.ttf", 32);

    mTextPosition.x = 50.0f;
    mTextPosition.y = 50.0f;
    score = 0;
}

const char* Game::GetWindowName() const
{
    return mWindowName;
}

void Game::GetClearColor(exColor& color) const
{
    color.r = 255;
    color.g = 255;
    color.b = 255;
}

void Game::OnEvent(SDL_Event* event)
{}

void Game::OnEventsConsumed()
{
    // Grab inputs
    mInput = 0;
    int nKeys = 0;
    const Uint8* pState = SDL_GetKeyboardState(&nKeys);

    mInput |= pState[SDL_SCANCODE_UP];
    mInput |= pState[SDL_SCANCODE_DOWN] << 1;
    mInput |= pState[SDL_SCANCODE_LEFT] << 2;
    mInput |= pState[SDL_SCANCODE_RIGHT] << 3;
}

void Game::Run(float deltaTime)
{
    if (!gameOver) 
    {

        ProcessInput(deltaTime);

        CreateFood();

        Render(deltaTime);

        EatFoodBox();

        CheckGameOver();
    }
    else 
    {
        GameOverRender();
    }
}

void Game::ProcessInput(float deltaTime)
{
    // Change scene based on input
    if (mInput & UP && moveDirection != DOWN)
    {
        moveDirection = UP;
    }
    else if (mInput & DOWN && moveDirection != UP)
    {
        moveDirection = DOWN;
    }

    if (mInput & LEFT && moveDirection != RIGHT)
    {
        moveDirection = LEFT;
    }
    else if (mInput & RIGHT && moveDirection != LEFT)
    {
        moveDirection = RIGHT;
    }
    SnakeDirection(deltaTime);
    OutOfBounds(mBodyPos[0].x, mBodyPos[0].y);
}

// Move Direction
void Game::SnakeDirection(float deltaTime) 
{
    BodyUpdatePos();

    // Change scene based on input
    if (moveDirection & UP)
    {
        mBodyPos[0].y -= 100.0f * deltaTime;
    }
    else if (moveDirection & DOWN)
    {
        mBodyPos[0].y += 100.0f * deltaTime;
    }

    if (moveDirection & LEFT)
    {
        mBodyPos[0].x -= 100.0f * deltaTime;
    }
    else if (moveDirection & RIGHT)
    {
        mBodyPos[0].x += 100.0f * deltaTime;
    }
}

void Game::BodyUpdatePos() 
{
    int j = 0;
    // Check if there's a body
    if (mBodyPos.size() > 1)
    {
        // Save previous position
        mPrevPosition.x = mBodyPos[1].x;
        mPrevPosition.y = mBodyPos[1].y;

        // Place the first body part to the position of the head
        mBodyPos[1] = mBodyPos[0];

        for (int i = 2; i < mBodyPos.size(); i++)
        {
            // Save previous position of the body part
            mPrev2Pos.x = mBodyPos[i].x;
            mPrev2Pos.y = mBodyPos[i].y;

            // make the position of the current body part to the previous of the head
            mBodyPos[i].x = mPrevPosition.x;
            mBodyPos[i].y = mPrevPosition.y;

            // save new previous position for the next parts
            mPrevPosition.x = mPrev2Pos.x;
            mPrevPosition.y = mPrev2Pos.y;
        }
    }
}


// Wrapper, if snake gets out of bounds, spawn him on the other side
void Game::OutOfBounds(float xPos, float yPos) 
{
    if (xPos <= WALLLEFT) 
    {
        mBodyPos[0].x = 800.0f;
    }
    else if (xPos >= WALLRIGHT)
    {
        mBodyPos[0].x = 0.0f;
    }

    if (yPos <= WALLBOT)
    {
        mBodyPos[0].y = 600.0f;
    }
    else if (yPos >= WALLTOP)
    {
        mBodyPos[0].y = 0.0f;
    }

}


// Instantiate a "Foodbox"
void Game::CreateFood() 
{
    // If the foodbox was eaten, instantiate a new one in random position
    if (isFoodEaten) 
    {
        mFoodbox = exVector2(3, 3);
        int x = 1 + (rand() % 800);
        int y = 1 + (rand() % 600);
        std::cout << x << ", " << y << std::endl;
        mFoodPosition = exVector2(x, y);
        isFoodEaten = false;
    }
}

void Game::EatFoodBox() 
{
    // No food to eat: Preventive if
    if (isFoodEaten) 
    {
        return;
    }
    
    // Head's center is inside the boundaries of foodbox, then eats it
    if (IsCenterInsideFoodBound())
    {
        isFoodEaten = true;
        score++;
        AppendTail();
        n++;
    }
    
}

void Game::AppendTail() 
{
    for (int i = 0; i <= 6; i++) {
        mBodyPos.emplace_back(exVector2(0, 0));
    }
    exVector2 tail = exVector2(0,0);

    exVector2 p1 = exVector2(tail.x - mSnakeBody.x, tail.y - mSnakeBody.y);
    exVector2 p2 = exVector2(tail.x + mSnakeBody.x, tail.y + mSnakeBody.y);

    // Add boundaries for body
    bodyBound.emplace_back(p1);
    bodyBound.emplace_back(p2);
}

void Game::DrawBody() 
{
    //Check if there's a body to draw
    if (mBodyPos.size() > 1) 
    {
        int j = 0;
        // For every 7 positions, draw body
        for (int i = 7; i < mBodyPos.size(); i+= 7)
        {
            exVector2 tail = mBodyPos[i];

            exVector2 p1 = exVector2(tail.x - mSnakeBody.x, tail.y - mSnakeBody.y);
            exVector2 p2 = exVector2(tail.x + mSnakeBody.x, tail.y + mSnakeBody.y);

            // Save boundaries
            bodyBound[j] = p1;
            bodyBound[j + 1] = p2;
            j += 2;

            mEngine->DrawBox(p1, p2, exColor(0, 255, 0), 1);
        }
    }

}

bool Game::IsCenterInsideFoodBound() 
{
    // Check snake head's center if is inside foodbox boundaries in X
    if (mBodyPos[0].x >= foodbound[0].x && mBodyPos[0].x <= foodbound[1].x)
    {
        // Check snake head's center if is inside foodbox boundaries in Y
        if (mBodyPos[0].y >= foodbound[0].y && mBodyPos[0].y <= foodbound[1].y)
        {
            return true;
        }
    }

    return false;
}

void Game::CheckGameOver()
{
    // If snake head collides with body
    if (DoesCenterCollidedWithBody()) 
    {
        gameOver = true;
    }
}

bool Game::DoesCenterCollidedWithBody()
{
    if (bodyBound.size() > 2) 
    {
        // Check Collisions
        for (int i = 0; i < bodyBound.size() - 1; i+= 2)
        {
            // Check snake head's center if is inside foodbox boundaries in 
            if (mBodyPos[0].x >= bodyBound[i].x && mBodyPos[0].x <= bodyBound[i + 1].x)
            {
                // Check snake head's center if is inside foodbox boundaries in Y
                if (mBodyPos[0].y >= bodyBound[i].y && mBodyPos[0].y <= bodyBound[i+1].y)
                {
                    return true;
                }
            }
        }
    }
    
    
    return false;
}

void Game::GameOverRender() 
{
    // Print Game over message
    mEngine->DrawText(mFontID, exVector2(300, 200), "GAME OVER", exColor(0, 0, 0), 0);

    // Sting to save Final Score
    std::string sText = "Final Score: ";

    // cast integer of score to string and save it to text
    sText += std::to_string(score);

    // Change string to a constant character array
    const char* scoreText = sText.c_str();

    mEngine->DrawText(mFontID, exVector2(300, 300), scoreText, exColor(0, 0, 0), 0);
}


void Game::Render(float deltaTime)
{
    // box boundaries 
    exVector2 p1 = exVector2(mBodyPos[0].x - mSnakeHead.x, mBodyPos[0].y - mSnakeHead.y);
    exVector2 p2 = exVector2(mBodyPos[0].x + mSnakeHead.x, mBodyPos[0].y + mSnakeHead.y);

    // Food boundaries
    if (!isFoodEaten) {

        exVector2 f1 = exVector2(mFoodPosition.x - mFoodbox.x, mFoodPosition.y - mFoodbox.y);
        exVector2 f2 = exVector2(mFoodPosition.x + mFoodbox.x, mFoodPosition.y + mFoodbox.y);

        foodbound[0] = f1;
        foodbound[1] = f2;

        mEngine->DrawBox(f1, f2, exColor(0, 0, 255), 1);
    }

    // draw a full box
    mEngine->DrawBox(p1, p2, exColor(255, 0, 0), 1);

    // Call the function to draw the body
    DrawBody();

    std::string sText = "Score: ";
    sText += std::to_string(score);
    const char* scoreText = sText.c_str();

    mEngine->DrawText(mFontID, mTextPosition, scoreText, exColor(0, 0, 0), 0);

    //////////////////////////
    // other primities
    //////////////////////////
    // mEngine->DrawLineBox(top_left, bottom_right, color, layer);

    // mEngine->DrawCircle( origin, radius, color, layer );

    // mEngine->DrawLineCircle( origin, radius, color, layer );
}