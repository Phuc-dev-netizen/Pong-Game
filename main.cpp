#include "game.h"
#undef main
int main(int argc, char* argv[]){
    // Khởi tạo SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        cerr<<"SDL could not initialize: "<<SDL_GetError()<<endl;
        return 1;}
    if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)){
        cerr<<"SDL_image could not initialize: "<<IMG_GetError()<<endl;
        return 1;}
    // [MỚI] Khởi tạo audio
    if(SDL_Init(SDL_INIT_AUDIO)<0){
        cerr<<"SDL audio could not initialize: "<<SDL_GetError()<<endl;}
    InitializeAudio();
    // Tạo cửa sổ
    SDL_Window* window=SDL_CreateWindow("Pong Game",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window){
        cerr<<"Window could not be created: "<<SDL_GetError()<<endl;
        return 1;}
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer){
        cerr<<"Renderer could not be created: "<<SDL_GetError()<<endl;
        return 1;}
    // Tải texture
    SDL_Texture* bgMain=LoadTexture("menu_main.png",renderer);
    SDL_Texture* bgMenus[3]={
        LoadTexture("menu_leftPaddle.png",renderer),
        LoadTexture("menu_rightPaddle.png",renderer),
        LoadTexture("menu_ball.png",renderer)};
    SDL_Texture* playBtnImg=LoadTexture("button_play.png",renderer);
    SDL_Texture* exitBtnImg=LoadTexture("button_exit.png",renderer);
    digitsTex=LoadTexture("digits.png",renderer);
    SDL_Texture* scoreLabelTex=LoadTexture("score.png",renderer);
    SDL_Texture* roundLabelTex=LoadTexture("round.png",renderer);
    if(!digitsTex||!scoreLabelTex||!roundLabelTex){
        cerr<<"Failed to load digits/score/round textures!"<<endl;
        return 1;}
    // Khởi tạo game objects
    SDL_Rect paddleLeft={10,WINDOW_HEIGHT/2-50,10,100};
    SDL_Rect paddleRight={WINDOW_WIDTH-20,WINDOW_HEIGHT/2-50,10,100};
    SDL_Rect ball={WINDOW_WIDTH/2-10,WINDOW_HEIGHT/2-10,20,20};
    int ballVelX=5,ballVelY=5;
    float speedMultiplier=1.0f;
    // Màu sắc
    Color paddleLeftColor(255,255,255);
    Color paddleRightColor(255,255,255);
    Color ballColor(255,255,255);
    Color rainbowColors[7]={
        {255,0,0,255},    // Đỏ
        {255,127,0,255},  // Cam
        {255,255,0,255},  // Vàng
        {0,255,0,255},    // Lục
        {0,0,255,255},    // Lam
        {148,0,211,255},  // Tím
        {255,255,255,255} // Trắng
    };
    // Biến trạng thái
    bool quit=false;
    bool inMenu=true;
    bool inColorMenu=false;
    int currentColorMenu=0;
    // Vòng lặp chính
    while(!quit){
        if(inMenu){
            RunMainMenu(renderer,bgMain,playBtnImg,exitBtnImg,quit,inMenu,inColorMenu);}
        else if(inColorMenu){
            RunColorMenu(renderer,bgMenus,rainbowColors,currentColorMenu,paddleLeftColor,paddleRightColor,ballColor,inColorMenu,quit);}
        else{
            // Xử lý sự kiện
            SDL_Event e;
            while(SDL_PollEvent(&e)){
                if(e.type==SDL_QUIT) quit=true;
                if(waitingForEnter&&e.type==SDL_KEYDOWN&&e.key.keysym.sym==SDLK_RETURN){
                    waitingForEnter=false; PlayRoundStartSound();}
                // [MỚI] Thêm xử lý phím ESC
                if(e.key.keysym.sym==SDLK_ESCAPE){
                    inMenu=true;  // Về menu chính
                    waitingForEnter=true;  // Reset trạng thái chờ
                    // [MỚI] Reset điểm nếu muốn
                    leftScore=0;
                    rightScore=0;
                    currentRound=1;
                }
            }
            // Xử lý input
            const Uint8* keystates=SDL_GetKeyboardState(NULL);
            HandleGameInput(keystates,paddleLeft,paddleRight);
            // Cập nhật game
            UpdateGame(ball,ballVelX,ballVelY,speedMultiplier,paddleLeft,paddleRight,waitingForEnter);
            // Render
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
            DrawScore(renderer,digitsTex,scoreLabelTex,roundLabelTex);
            SDL_SetRenderDrawColor(renderer,paddleLeftColor.r,paddleLeftColor.g,paddleLeftColor.b,255);
            SDL_RenderFillRect(renderer,&paddleLeft);
            SDL_SetRenderDrawColor(renderer,paddleRightColor.r,paddleRightColor.g,paddleRightColor.b,255);
            SDL_RenderFillRect(renderer,&paddleRight);
            SDL_SetRenderDrawColor(renderer,ballColor.r,ballColor.g,ballColor.b,255);
            SDL_RenderFillRect(renderer,&ball);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);}}
    // [MỚI] Dọn dẹp audio
    CleanupAudio();
    // Giải phóng bộ nhớ
    SDL_DestroyTexture(bgMain);
    for(int i=0;i<3;++i) SDL_DestroyTexture(bgMenus[i]);
    SDL_DestroyTexture(playBtnImg);
    SDL_DestroyTexture(exitBtnImg);
    SDL_DestroyTexture(digitsTex);
    SDL_DestroyTexture(scoreLabelTex);
    SDL_DestroyTexture(roundLabelTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;}
