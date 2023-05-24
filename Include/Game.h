#pragma once

#include "GameInterface.h"
#include "EngineTypes.h"
#include <vector>
#include <iostream>

class Game : public exGameInterface
{
public:
    static Game*        GetInstance();
        
    virtual             ~Game();

    virtual void        Initialize(exEngineInterface* engine);
    virtual const char* GetWindowName() const;
    virtual void        GetClearColor(exColor& color) const;

    // Added Methods
    virtual void        SnakeDirection(float deltaTime);
    
    // Mantain the snake inside area (Wrapper)
    virtual void        OutOfBounds(float xPos, float yPos);
    
    // Create a foodbox
    virtual void        CreateFood();
    
    // Check if foodbox was eaten
    virtual void        EatFoodBox();

    // Update the snake body's position
    virtual void        BodyUpdatePos();

    // Add a new square to the snake's body
    virtual void        AppendTail();

    // Draw the snake's body
    virtual void        DrawBody();

    // returns a boolean stating whether the snake head's center collides 
    // with the foodbox in x and y coordinates
    virtual bool        IsCenterInsideFoodBound();

    // Check Game over status
    virtual void        CheckGameOver();

    // Check if head of the snake collided with body to game over
    virtual bool        DoesCenterCollidedWithBody();

    virtual void        GameOverRender();

    // End of Added Methods

    virtual void        OnEvent(SDL_Event* event);
    virtual void        OnEventsConsumed();

    virtual void        Run(float deltaTime);
private:
    Game();
    
    void                ProcessInput(float deltaTime);
    void                Render(float deltaTime);
    
    static Game*        sInstance;
    
    exEngineInterface*  mEngine;

    int                 mFontID;
    const char*         mWindowName;

    exVector2           mTextPosition;
    exVector2           mSquarePosition;
    exVector2           mSnakeHead;
    
    // Added variables

    // Previous position vectors for head and body respectively
    exVector2           mPrevPosition;
    exVector2           mPrev2Pos;

    // Snake body dimensions
    exVector2           mSnakeBody;

    // Box for snake to eat
    exVector2           mFoodbox;

    // Position of food box
    exVector2           mFoodPosition;
    
    // Boundaries of the foodbox
    exVector2           foodbound[2];
    
    // Boundaries of the snake's body
    std::vector<exVector2>   bodyBound;
    
    // Snakes body position
    std::vector<exVector2>   mBodyPos;
    
    // The direction of the snake's movement
    int                 moveDirection;
    
    // Flag to check if instanced foodbox has been eaten or not
    bool                isFoodEaten;
    
    // The number of foodboxes eaten
    int                 score;
    
    // Flag for game over status
    bool                gameOver;
    
    // End of added variables

    int                 mInput;
};

