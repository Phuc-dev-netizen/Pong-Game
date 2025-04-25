#include <SDL.h>
#include <iostream>
using namespace std;
const int WINDOW_WIDTH=800;
const int WINDOW_HEIGHT=600;

int main(int argc,char* argv[]){
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        cerr<<"SDL không thể khởi tạo! SDL_Error: "<<SDL_GetError()<<endl;
        return 1;
    }
    SDL_Window* window=SDL_CreateWindow("Game bóng bàn",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    if(window==nullptr){
        cerr<<"Không thể tạo cửa sổ! SDL_Error: "<<SDL_GetError()<<endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(renderer==nullptr){
        cerr<<"Không thể tạo renderer! SDL_Error: "<<SDL_GetError()<<endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Rect paddleLeft={10,WINDOW_HEIGHT/2-50,10,100};
    SDL_Rect paddleRight={WINDOW_WIDTH-20,WINDOW_HEIGHT/2-50,10,100};
    SDL_Rect ball={WINDOW_WIDTH/2-10,WINDOW_HEIGHT/2-10,20,20};
    int ballVelX=5,ballVelY=5;
    bool quit=false;
    SDL_Event e;
    while(!quit){
        while(SDL_PollEvent(&e)!=0){
            if(e.type==SDL_QUIT){
                quit=true;
            }
        }
        const Uint8* keystates=SDL_GetKeyboardState(NULL);
        if(keystates[SDL_SCANCODE_W]&&paddleLeft.y>0){
            paddleLeft.y-=5;
        }
        if(keystates[SDL_SCANCODE_S]&&paddleLeft.y+paddleLeft.h<WINDOW_HEIGHT){
            paddleLeft.y+=5;
        }
        if(keystates[SDL_SCANCODE_UP]&&paddleRight.y>0){
            paddleRight.y-=5;
        }
        if(keystates[SDL_SCANCODE_DOWN]&&paddleRight.y+paddleRight.h<WINDOW_HEIGHT){
            paddleRight.y+=5;
        }
        ball.x+=ballVelX;
        ball.y+=ballVelY;
        if(ball.y<=0||ball.y+ball.h>=WINDOW_HEIGHT){
            ballVelY=-ballVelY;
        }
        if(SDL_HasIntersection(&ball,&paddleLeft)||SDL_HasIntersection(&ball,&paddleRight)){
            ballVelX=-ballVelX;
        }
        if(ball.x<0||ball.x>WINDOW_WIDTH){
            ball.x=WINDOW_WIDTH/2-10;
            ball.y=WINDOW_HEIGHT/2-10;
        }
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderFillRect(renderer,&paddleLeft);
        SDL_RenderFillRect(renderer,&paddleRight);
        SDL_RenderFillRect(renderer,&ball);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
