#include<SDL.h>
#include<iostream>
#include<SDL_image.h>
using namespace std;

//Độ phân giải cửa sổ game
const int WINDOW_WIDTH=800; //Chiều rộng
const int WINDOW_HEIGHT=600; //Chiều dài

int main(int argc,char* argv[]){
    //Khởi tạo SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        cerr<<"Lỗi SDL: "<<SDL_GetError()<<endl;
        return 1;
    }

    //Tạo cửa sổ game (tiêu đề, x, y, w, h, cờ hiển thị)
    SDL_Window* window=SDL_CreateWindow("Game bóng bàn",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window){
        cerr<<"Lỗi cửa sổ: "<<SDL_GetError()<<endl;
        SDL_Quit();
        return 1;
    }

    //Tạo renderer (cửa sổ, index, chế độ tăng tốc)
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer){
        cerr<<"Lỗi renderer: "<<SDL_GetError()<<endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    //Khai báo texture menu
    SDL_Texture* startTex=nullptr;//Texture nút Start
    SDL_Texture* exitTex=nullptr;//Texture nút Exit
    bool inMenu=true;

    //Load ảnh
    startTex=IMG_LoadTexture(renderer,"Start-button-icon-isolated-on-transparent-background-PNG.png");
    exitTex=IMG_LoadTexture(renderer,"Quit-button-hd-png.png");
    if(!startTex||!exitTex){
        cerr<<"Lỗi! Kiểm tra xem bạn đã đặt file ảnh cùng thư mục với exe chưa!"<<endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    //Thiết lập đối tượng game (thanh trái, thanh phải, quả bóng)
    SDL_Rect paddleLeft={10,WINDOW_HEIGHT/2-50,10,100};  //x,y,w,h
    SDL_Rect paddleRight={WINDOW_WIDTH-20,WINDOW_HEIGHT/2-50,10,100};
    SDL_Rect ball={WINDOW_WIDTH/2-10,WINDOW_HEIGHT/2-10,20,20};
    int ballVelX=5, ballVelY=5;

    //Vòng lặp
    bool quit=false;
    SDL_Event e;
    while(!quit){
        while(inMenu){
        //Vẽ menu
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_Rect startBtn={300,200,200,80};
        SDL_Rect exitBtn={300,300,200,80};
        SDL_RenderCopy(renderer,startTex,NULL,&startBtn);
        SDL_RenderCopy(renderer,exitTex,NULL,&exitBtn);
        SDL_RenderPresent(renderer);

        //Xử lý sự kiện menu
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                quit=true;
                inMenu=false;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                int x,y;
                SDL_GetMouseState(&x,&y);
                if(x>=300&&x<=500&&y>=200&&y<=280)inMenu=false;//Bắt đầu
                if(x>=300&&x<=500&&y>=300&&y<=380)quit=true;//Thoát
            }
        }
    }
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) quit=true;
        }

        //Input
        const Uint8* keystates=SDL_GetKeyboardState(NULL);
        if(keystates[SDL_SCANCODE_W]&&paddleLeft.y>0) paddleLeft.y-=5;
        if(keystates[SDL_SCANCODE_S]&&paddleLeft.y+paddleLeft.h<WINDOW_HEIGHT) paddleLeft.y+=5;
        if(keystates[SDL_SCANCODE_UP]&&paddleRight.y>0) paddleRight.y-=5;
        if(keystates[SDL_SCANCODE_DOWN]&&paddleRight.y+paddleRight.h<WINDOW_HEIGHT) paddleRight.y+=5;

        //Cập nhật bóng
        ball.x+=ballVelX;
        ball.y+=ballVelY;
        if(ball.y<=0||ball.y+ball.h>=WINDOW_HEIGHT) ballVelY=-ballVelY;
        if(SDL_HasIntersection(&ball,&paddleLeft)||SDL_HasIntersection(&ball,&paddleRight)) ballVelX=-ballVelX;
        if(ball.x<0||ball.x>WINDOW_WIDTH) ball={WINDOW_WIDTH/2-10,WINDOW_HEIGHT/2-10,20,20};

        //Vẽ hình
        SDL_SetRenderDrawColor(renderer,0,0,0,255); //Màu nền: đen
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,255,255,255,255); //Màu các thanh và bóng: trắng
        SDL_RenderFillRect(renderer,&paddleLeft);
        SDL_RenderFillRect(renderer,&paddleRight);
        SDL_RenderFillRect(renderer,&ball);
        SDL_RenderPresent(renderer);

        //Tự điều chỉnh fps (xấp xỉ 60)
        SDL_Delay(16); //16ms
    }

    //Giải phóng bộ nhớ
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
