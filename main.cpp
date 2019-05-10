#include <SDL.h>
#include <bits/stdc++.h>
#include <fstream>
#include <SDL_image.h>
#include <ctime>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <SDL_mixer.h>

using namespace std;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 720;
int score = 0;
int speed = 12;
int highScore;

enum
{
    KEY_PRESS_SURFACE_TOTAL, KEY_PRESS_SURFACE_DEFAULT
};

SDL_Window* Window = NULL;
SDL_Surface* ScreenSurface = NULL;
SDL_Texture* CAR = NULL;
SDL_Texture* road = NULL;
SDL_Texture* gameover = NULL;
SDL_Surface* KeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ];
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;
TTF_Font* font = NULL;
Mix_Chunk* chunk = NULL;
Mix_Chunk* click = NULL;
Mix_Music* backGroundMusic = NULL;
Mix_Music* gameMusic = NULL;
SDL_Rect pointRect;
SDL_Rect carRect;
SDL_Rect enemyRect;
SDL_Rect enemy2Rect;

void playGame();
void gameOver();
void Menu();
void Rule();

bool Init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cout << "Error: " << SDL_GetError();
        success = false;
    }
    else
    {
        //Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
        //Create window
        Window = SDL_CreateWindow("CAR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if ( Window == NULL )
            cout << "Error Window: " << SDL_GetError();
        else
        {
            gRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
            if ( gRenderer == NULL )
            {
                cout << "Error gRenderer: " << SDL_GetError();
                success = false;
            }
            else
            {
            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            int imgFlags = IMG_INIT_JPG;
            if ( !IMG_Init( imgFlags ) & imgFlags )
            {
                cout << "Error: " << IMG_GetError();
                success = false;
            }

            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
            {
                cout << "Error: " << Mix_GetError();
                success = false;
            }
            }
        }
    }
    if (TTF_Init() == -1 )
        success = false;
    return success;
}

SDL_Texture* loadTexture( std::string path )
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

    if ( loadedSurface == NULL )
        cout << "Error Surface: " << IMG_GetError();
    else
    {
        SDL_SetColorKey (loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 123, 123, 123));
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL )
		{
			cout << "Error newTexture: " << SDL_GetError();
		}

		SDL_FreeSurface( loadedSurface );
    }
    return newTexture;
}


void close()
{
    SDL_DestroyTexture( CAR );
    CAR = NULL;
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(Window);
    Window = NULL;
    gRenderer = NULL;

    Mix_CloseAudio();
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_Quit();
}

struct car
{
    int xPos;
    int yPos;

    car()
    {
        xPos = 160;
        yPos = 520;
    }

    void appear()
    {
        SDL_Texture* car = NULL;
        car = loadTexture("redcar.jpg");
        carRect.x = xPos;
        carRect.y = yPos;
        carRect.h = 151;
        carRect.w = 81;
        SDL_RenderSetViewport( gRenderer, &carRect );
        SDL_RenderCopy( gRenderer, car, NULL, NULL);
        SDL_DestroyTexture(car);
    }

    void moveLeft()
    {
       xPos = ( (xPos-130) < 30 )? 30: (xPos-130);
       yPos = 520;
    }

    void moveRight()
    {
        xPos = ((xPos+130) > 290)? 290: (xPos+130);
        yPos = 520;
    }
};

struct point
{
    int xPos;
    int yPos;

    point()
    {
        xPos = 320;
        yPos = -100;

    }

    void appear()
    {
        if(yPos >= 720)
        {
                srand(time(0));
            int position = rand()%9 + 1;
            if(position == 1 || position == 2 || position == 3)
            {
                xPos = 190;
            }
            else if( position == 4 || position == 5 || position == 6 )
            {
                xPos = 320;
            }
            else if ( position == 7 || position == 8 || position == 9 )
            {
                xPos = 60;
            }
            yPos = -100;
        }
        SDL_Texture* point = NULL;
        point = loadTexture("redcircle.jpg");
        pointRect.x = xPos;
        pointRect.y = yPos;
        pointRect.h = 40;
        pointRect.w = 40;
        SDL_RenderSetViewport( gRenderer, &pointRect);
        SDL_RenderCopy(gRenderer, point, NULL, NULL);
        SDL_DestroyTexture(point);
    }

    void update()
    {
        yPos = yPos + speed;
    }
};

struct enemy
{
    int xPos;
    int yPos;

    enemy()
    {
        xPos = 320;
        yPos = 0;

    }

    void appear()
    {
        if(yPos >= 720){
            int position = rand()%3 + 1;
            if(position == 1)
            {
                xPos = 190;
            }
            else if( position == 2 )
            {
                xPos = 320;
            }
            else
            {
                xPos = 60;
            }
            yPos = 0;
        }
        SDL_Texture* enemy = NULL;
        enemy = loadTexture("redSquare.jpg");
        enemyRect.x = xPos;
        enemyRect.y = yPos;
        enemyRect.h = 40;
        enemyRect.w = 40;
        SDL_RenderSetViewport( gRenderer, &enemyRect);
        SDL_RenderCopy(gRenderer, enemy, NULL, NULL);
        SDL_DestroyTexture(enemy);
    }

    void update()
    {
        yPos = yPos + speed;
    }
};

struct enemy2
{
    int xPos;
    int yPos;

    enemy2()
    {
        xPos = 320;
        yPos = 0;

    }

    void appear()
    {
        if(yPos >= 720)
        {
            int position = rand()%3 + 1;
            if( position == 1 )
            {
                xPos = 190;
            }
            else if( position == 2 )

            {
                xPos = 320;
            }
            else
            {
                xPos = 60;
            }
            yPos = 0;
        }

        SDL_Texture* enemy2 = NULL;
        enemy2 = loadTexture("redSquare.jpg");
        enemy2Rect.x = xPos;
        enemy2Rect.y = yPos;
        enemy2Rect.h = 40;
        enemy2Rect.w = 40;
        SDL_RenderSetViewport( gRenderer, &enemy2Rect);
        SDL_RenderCopy(gRenderer, enemy2, NULL, NULL);
        SDL_DestroyTexture(enemy2);
    }

    void update()
    {
        yPos = yPos + speed;
    }
};

void checkAccident( car &car, enemy &ene, enemy2 &ene2, bool check )
{

    if (( ene.yPos >= 500 && ene.yPos <= 630 && abs( car.xPos - ene.xPos ) <= 30 ) || ( ene2.yPos >= 500 && ene2.yPos <= 630 && abs( car.xPos - ene2.xPos ) <= 30 ))
    {
        SDL_Delay(500);
        check = true;
        gameOver();
    }
    else
        check = false;

}

void getScore( car &car, point &p )
{
    chunk = Mix_LoadWAV("button-6.wav");

    if ( p.yPos >= 490 && p.yPos <= 630 && abs(car.xPos - p.xPos ) <= 60 )
        {
            Mix_PlayChannel(-1, chunk, 0);
            int position = rand()%3 + 1;
            if( position == 1 )
            {
                p.xPos = 190;
            }
            else if( position == 2 )

            {
                p.xPos = 320;
            }
            else
            {
                p.xPos = 60;
            }
            p.yPos = -200;
            score ++;
            ofstream highScoreOut;
            highScoreOut.open("highscore.txt");
            if ( score > highScore )
            highScore = score;
            highScoreOut << highScore;
            highScoreOut.close();
            ifstream highScoreIn;
            highScoreIn.open("highscore.txt");
            highScoreIn >> highScore;
            highScoreIn.close();
        }

}

void Text()
{
    font = TTF_OpenFont("Robika.ttf", 50);
    if (font == NULL)
    {
      cout << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
      TTF_Quit();
      SDL_Quit();
      exit(1);
    }

    SDL_Color color = { 255, 255, 255 };
    std::string SCORE;
    ostringstream convert;
    convert << score;
    SCORE = convert.str();

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, SCORE.c_str() ,  color);
    if (scoreSurface == NULL)
    {
      cout << "TTF_RenderText_Solid() Failed: " << TTF_GetError() << endl;
      TTF_Quit();
      SDL_Quit();
      exit(1);
    }
    SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);
    SDL_Rect Score;
    Score.x = 710;
    Score.y = 480;
    Score.h = 50;
    Score.w = 35;
    SDL_RenderCopy(gRenderer, Message, NULL, &Score);
    SDL_FreeSurface(scoreSurface);
}

void gameOver()
{
    backGroundMusic = Mix_LoadMUS("backgroundmusic.wav");
    Mix_PlayMusic(backGroundMusic,-1);
    SDL_Event Over;
    SDL_Texture* gameoverTexture = loadTexture("gameover.png");
    SDL_Rect overRect;
    overRect.x = 0;
    overRect.y = 0;
    overRect.h = SCREEN_HEIGHT;
    overRect.w = SCREEN_WIDTH;
    SDL_RenderSetViewport( gRenderer, &overRect );
    SDL_RenderCopy( gRenderer, gameoverTexture, NULL, NULL);

    font = TTF_OpenFont("Robika.ttf", 150);
    SDL_Color color = { 2, 2, 2 };
    std::string SCORE;
    ostringstream convert;
    convert << score;
    SCORE = convert.str();
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, SCORE.c_str() ,  color);
    SDL_Rect scoreRect;
    scoreRect.x = 818;
    scoreRect.y = 410;
    scoreRect.h = 70;
    scoreRect.w = 40;
    SDL_Texture* yourscoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_RenderCopy(gRenderer, yourscoreTexture, NULL, &scoreRect);

    SDL_Rect highscoreRect;
    highscoreRect.x = 400;
    highscoreRect.y = 410;
    highscoreRect.h = 70;
    highscoreRect.w = 40;
    std::string HIGHSCORE;
    ostringstream convert2;
    convert2 << highScore;
    HIGHSCORE = convert2.str();
    SDL_Surface* highscoreSurface = TTF_RenderText_Solid(font, HIGHSCORE.c_str(), color);
    SDL_Texture* highscoreTexture= SDL_CreateTextureFromSurface(gRenderer, highscoreSurface);
    SDL_FreeSurface(highscoreSurface);
    SDL_RenderCopy(gRenderer, highscoreTexture, NULL, &highscoreRect);
    SDL_RenderPresent(gRenderer);

    bool quitOver = false;
    while(!quitOver){
        SDL_Delay(10);
        if(SDL_WaitEvent(&Over) == 0){
            continue;
        }
        if(Over.type == SDL_QUIT){
            quitOver = true;
        }
        if(Over.type == SDL_MOUSEBUTTONDOWN){
            if (Over.button.button == SDL_BUTTON_LEFT){
                if(Over.button.x >= 340 && Over.button.x <= 700 && Over.button.y >= 490 && Over.button.y <= 560 )
                {
                    Mix_PlayChannel(-1, click, 0);
                    score = 0;
                    playGame();
                }
                else if(Over.button.x >= 340 && Over.button.x <= 700 && Over.button.y >= 565 && Over.button.y <= 630 )
                {
                    Mix_PlayChannel(-1, click, 0);
                    Menu();
                    quitOver = true;
                }
                else if(Over.button.x >= 340 && Over.button.x <= 700 && Over.button.y >= 645 && Over.button.y <= 715 )
                {
                    Mix_PlayChannel(-1, click, 0);
                    close();
                }
            }
        }
    }
}

void playGame()
{
    car mycar = car();
    point p = point();
    enemy ene = enemy();
    enemy2 ene2 = enemy2();

    road = loadTexture("road.png");
    if (road == NULL)
        cout << "Error road:" << SDL_GetError();
    bool quit = false;
    SDL_Event e;
    gameMusic = Mix_LoadMUS("roadtofar.wav");
    Mix_PlayMusic(gameMusic, -1);

    while(!quit)
    {
        int start = SDL_GetTicks();
        while( SDL_PollEvent( &e ) != 0 )
            {
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                else if( e.type == SDL_KEYDOWN )
                {
                    switch( e.key.keysym.sym )
                    {
                        case SDLK_LEFT:
                            mycar.moveLeft();
                        break;
                        case SDLK_RIGHT:
                            mycar.moveRight();
                        break;
                    }
                }
            }

        SDL_Rect stretchRect;
        stretchRect.x = 0;
        stretchRect.y = 0;
        stretchRect.w = SCREEN_WIDTH;
        stretchRect.h = SCREEN_HEIGHT;
        SDL_RenderSetViewport( gRenderer, &stretchRect);
        SDL_RenderCopy(gRenderer, road, NULL, NULL);

        Text();

        SDL_Color textcolor = { 255, 255, 255 };
        SDL_Rect highscoreRect;
        highscoreRect.x = 850;
        highscoreRect.y = 570;
        highscoreRect.h = 50;
        highscoreRect.w = 35;
        std::string HIGHSCORE;
        ostringstream convert;
        convert << highScore;
        HIGHSCORE = convert.str();
        SDL_Surface* highscoreSurface = TTF_RenderText_Solid(font, HIGHSCORE.c_str(), textcolor);
        SDL_Texture* highscoreTexture= SDL_CreateTextureFromSurface(gRenderer, highscoreSurface);
        SDL_FreeSurface(highscoreSurface);
        SDL_RenderCopy(gRenderer, highscoreTexture, NULL, &highscoreRect);

        mycar.appear();
        ene.appear();
        p.appear();
        ene2.appear();
        ene.update();
        p.update();
        ene2.update();
        getScore(mycar, p);
        checkAccident(mycar, ene, ene2, quit);

        SDL_RenderPresent( gRenderer );
        int theend = SDL_GetTicks();
        int delay = 1000 / 60 - (theend - start);
        SDL_Delay(delay > 0 ? delay : 0);
    }
}

void Menu()
{
    SDL_Event menu;
    SDL_Texture* menuTexture = loadTexture("menu.png");
    SDL_Rect menuRect;
    menuRect.x = 0;
    menuRect.y = 0;
    menuRect.w = SCREEN_WIDTH;
    menuRect.h = SCREEN_HEIGHT;
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, menuTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
    click = Mix_LoadWAV("click.wav");
    bool quitMenu = false;
    while( !quitMenu )
    {
        if ( SDL_WaitEvent(&menu) == 0 )
            continue;
        if ( menu.type == SDL_QUIT )
        {
            quitMenu = true;
        }
        if ( menu.type == SDL_MOUSEBUTTONDOWN )
        {
            if ( menu.button.button == SDL_BUTTON_LEFT )
                if ( menu.button.x >= 360 && menu.button.x <= 680 && menu.button.y >= 355 && menu.button.y <= 440  )
                {
                    Mix_PlayChannel(-1, click, 0);
                    playGame();
                    quitMenu = true;
                }
                else if( menu.button.x >= 360 && menu.button.x <= 680 && menu.button.y >= 482 && menu.button.y <= 565 )
                {
                    Mix_PlayChannel(-1, click, 0);
                    Rule();
                    quitMenu = true;
                }
                else if (menu.button.x >= 360 && menu.button.x <= 680 && menu.button.y >= 600 && menu.button.y <= 685)
                {
                    Mix_PlayChannel(-1, click, 0);
                    close();
                }
        }
        else
            continue;

    }
}

void Rule()
{
    SDL_Event rule;
    SDL_Texture* ruleTexture = loadTexture("rules.png");
    SDL_Rect ruleRect;
    ruleRect.x = 0;
    ruleRect.y = 0;
    ruleRect.h = SCREEN_HEIGHT;
    ruleRect.w = SCREEN_WIDTH;
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, ruleTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
    bool quitRule= false;
    while(!quitRule)
    {
        SDL_Delay(10);
        if(SDL_WaitEvent(&rule) == 0){
            continue;
        }
        if(rule.type == SDL_QUIT){
            quitRule = true;
        }
        if(rule.type == SDL_MOUSEBUTTONDOWN)
        {
            if (rule.button.button == SDL_BUTTON_LEFT)
            {
                if(rule.button.x >= 330 && rule.button.y >= 450 && rule.button.x <= 650 && rule.button.y <= 550 )
                {
                    Mix_PlayChannel(-1, click, 0);
                    playGame();
                    quitRule = true;
                }
                if(rule.button.x >= 330 && rule.button.y >= 590 && rule.button.x <= 650 && rule.button.y <= 685 )
                {
                    Mix_PlayChannel(-1, click, 0);
                    Menu();
                    quitRule = true;
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    srand(time(0));
    ifstream highScoreIn;
    highScoreIn.open("highscore.txt");
    highScoreIn >> highScore;
    if(!Init())
    {
        printf("Fail to init\n");
    }
    else
    {
        backGroundMusic = Mix_LoadMUS("backgroundmusic.wav");
        Mix_PlayMusic(backGroundMusic,-1);
        Menu();
    }

    return 0;
}
