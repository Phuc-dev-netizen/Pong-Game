#include "game.h"

// Khởi tạo biến toàn cục
int leftScore=0;       // Điểm trái
int rightScore=0;      // Điểm phải
int currentRound=1;    // Vòng hiện tại
SDL_Texture* digitsTex=NULL; // Texture số
bool shouldLaunchRight=true; // Hướng phóng tiếp theo
bool waitingForEnter=true;   // Đang chờ nhấn Enter
int outOfBoundsCount=0;      // Đếm số lần bóng ra biên
bool inMenu=true;

// Hàm tải ảnh và trả về texture
SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer){
    SDL_Surface* surface=IMG_Load(path);
    if(!surface){
        cerr<<"Failed to load image: "<<path<<" - "<<IMG_GetError()<<endl;
        return nullptr;
    }
    SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Hàm vẽ số (0-10)
void DrawDigit(SDL_Renderer* renderer, SDL_Texture* tex, int num, int x, int y){
    SDL_Rect src={num*DIGIT_W,0,DIGIT_W,DIGIT_H};
    SDL_Rect dest={x,y,DIGIT_W,DIGIT_H};
    // Xử lý số 10
    if(num==10){
        // Căn giữa số 10 bằng cách giảm x đi nửa chiều rộng
        SDL_Rect src1={1*DIGIT_W,0,DIGIT_W,DIGIT_H};
        SDL_Rect dest1={x-DIGIT_W/2,y,DIGIT_W,DIGIT_H}; // Số 1 lệch trái
        SDL_RenderCopy(renderer,tex,&src1,&dest1);

        SDL_Rect src2={0*DIGIT_W,0,DIGIT_W,DIGIT_H};
        SDL_Rect dest2={x+DIGIT_W/2,y,DIGIT_W,DIGIT_H}; // Số 0 lệch phải
        SDL_RenderCopy(renderer,tex,&src2,&dest2);
    }
    SDL_RenderCopy(renderer,tex,&src,&dest);
}

// Hàm vẽ bảng điểm
void DrawScore(SDL_Renderer* renderer, SDL_Texture* digits, SDL_Texture* scoreLabel, SDL_Texture* roundLabel){
    SDL_Rect bg={WINDOW_WIDTH/2-140,10,280,70};
    SDL_SetRenderDrawColor(renderer,30,30,30,255);
    SDL_RenderFillRect(renderer,&bg);

    // Vẽ chữ SCORE ở bên trái
    SDL_Rect scoreLabelRect={WINDOW_WIDTH/2-135,15,60,15};
    SDL_RenderCopy(renderer,scoreLabel,NULL,&scoreLabelRect);

    // Vẽ chữ ROUND ở giữa
    SDL_Rect roundLabelRect={WINDOW_WIDTH/2-28,15,60,15};
    SDL_RenderCopy(renderer,roundLabel,NULL,&roundLabelRect);

    // Vẽ chữ SCORE bên phải
    SDL_Rect rightScoreTextRect={WINDOW_WIDTH/2+75,15,60,15};
    SDL_RenderCopy(renderer,scoreLabel,NULL,&rightScoreTextRect);

    // Điểm trái
    DrawDigit(renderer,digits,leftScore,WINDOW_WIDTH/2-115,35);
    // Vòng hiện tại
    DrawDigit(renderer,digits,currentRound,WINDOW_WIDTH/2-8,35);
    // Điểm phải
    DrawDigit(renderer,digits,rightScore,WINDOW_WIDTH/2+95,35);
}

// Hàm chạy menu chính
void RunMainMenu(SDL_Renderer* renderer, SDL_Texture* bgMain, SDL_Texture* playBtnImg, SDL_Texture* exitBtnImg, bool& quit, bool& inMenu, bool& inColorMenu){
    bool isPlayHovered=false;
    bool isExitHovered=false;
    SDL_Event e;

    while(inMenu){
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,bgMain,NULL,NULL);

        SDL_Rect playBtn={WINDOW_WIDTH/2-100,WINDOW_HEIGHT-160,200,60};
        SDL_Rect exitBtn={WINDOW_WIDTH/2-100,WINDOW_HEIGHT-80,200,60};

        // Tạo rect phóng to khi hover
        SDL_Rect playBtnRender=playBtn;
        SDL_Rect exitBtnRender=exitBtn;
        if(isPlayHovered){
            playBtnRender.x-=5; playBtnRender.y-=5;
            playBtnRender.w+=10; playBtnRender.h+=10;
        }
        if(isExitHovered){
            exitBtnRender.x-=5; exitBtnRender.y-=5;
            exitBtnRender.w+=10; exitBtnRender.h+=10;
        }

        SDL_RenderCopy(renderer,playBtnImg,NULL,&playBtnRender);
        SDL_RenderCopy(renderer,exitBtnImg,NULL,&exitBtnRender);
        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                quit=true;
                inMenu=false;
            }
            if(e.type==SDL_MOUSEMOTION){
                int x,y;
                SDL_GetMouseState(&x,&y);
                isPlayHovered=(x>=playBtn.x&&x<=playBtn.x+playBtn.w&&y>=playBtn.y&&y<=playBtn.y+playBtn.h);
                isExitHovered=(x>=exitBtn.x&&x<=exitBtn.x+exitBtn.w&&y>=exitBtn.y&&y<=exitBtn.y+exitBtn.h);
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                int x,y;
                SDL_GetMouseState(&x,&y);
                if(x>=playBtn.x&&x<=playBtn.x+playBtn.w&&y>=playBtn.y&&y<=playBtn.y+playBtn.h){
                    inColorMenu=true;
                    inMenu=false;
                }
                if(x>=exitBtn.x&&x<=exitBtn.x+exitBtn.w&&y>=exitBtn.y&&y<=exitBtn.y+exitBtn.h){
                    quit=true;
                    inMenu=false;
                }
            }
        }
    }
}

// Hàm chạy menu màu
void RunColorMenu(SDL_Renderer* renderer, SDL_Texture* bgMenus[], Color rainbowColors[], int& currentColorMenu, Color& paddleLeftColor, Color& paddleRightColor, Color& ballColor, bool& inColorMenu, bool& quit){
    SDL_Event e;
    while(inColorMenu){
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,bgMenus[currentColorMenu],NULL,NULL);

        int startX=(WINDOW_WIDTH-(7*80+6*10))/2;
        for(int i=0;i<7;++i){
            SDL_Rect rect={startX+i*90,250,80,80};
            SDL_SetRenderDrawColor(renderer,rainbowColors[i].r,rainbowColors[i].g,rainbowColors[i].b,255);
            SDL_RenderFillRect(renderer,&rect);
        }
        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                quit=true;
                inColorMenu=false;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                int x,y;
                SDL_GetMouseState(&x,&y);
                for(int i=0;i<7;++i){
                    SDL_Rect rect={startX+i*90,250,80,80};
                    if(x>=rect.x&&x<=rect.x+rect.w&&y>=rect.y&&y<=rect.y+rect.h){
                        if(currentColorMenu==0) paddleLeftColor=rainbowColors[i];
                        else if(currentColorMenu==1) paddleRightColor=rainbowColors[i];
                        else if(currentColorMenu==2) ballColor=rainbowColors[i];
                        currentColorMenu++;
                        if(currentColorMenu>2) inColorMenu=false;
                        break;
                    }
                }
            }
        }
    }
}

// Hàm xử lý input game
void HandleGameInput(const Uint8* keystates, SDL_Rect& paddleLeft, SDL_Rect& paddleRight){
    if(keystates[SDL_SCANCODE_W]&&paddleLeft.y>0) paddleLeft.y-=5;
    if(keystates[SDL_SCANCODE_S]&&paddleLeft.y+paddleLeft.h<WINDOW_HEIGHT) paddleLeft.y+=5;
    if(keystates[SDL_SCANCODE_UP]&&paddleRight.y>0) paddleRight.y-=5;
    if(keystates[SDL_SCANCODE_DOWN]&&paddleRight.y+paddleRight.h<WINDOW_HEIGHT) paddleRight.y+=5;
}

// Hàm cập nhật game
void UpdateGame(SDL_Rect& ball, int& ballVelX, int& ballVelY, float& speedMultiplier, const SDL_Rect& paddleLeft, const SDL_Rect& paddleRight, bool waitingForEnter){
    if(!waitingForEnter){
        ball.x+=(int)(ballVelX*speedMultiplier);
        ball.y+=(int)(ballVelY*speedMultiplier);
        if(ball.y<=0||ball.y+ball.h>=WINDOW_HEIGHT) ballVelY=-ballVelY;
        if(SDL_HasIntersection(&ball,&paddleLeft)||SDL_HasIntersection(&ball,&paddleRight)){
            ballVelX=-ballVelX;
            speedMultiplier+=0.05f; // Tăng tốc sau mỗi lần đập
        }
    }
    if(ball.x<0||ball.x>WINDOW_WIDTH){
        if(ball.x<0) rightScore++;
        else leftScore++;

        bool isFirstLaunch=(outOfBoundsCount==0&&ball.x==WINDOW_WIDTH/2-10);
        outOfBoundsCount++;
        if(outOfBoundsCount>=2&&!isFirstLaunch){
            shouldLaunchRight=!shouldLaunchRight;
            outOfBoundsCount=0;
        }

        ball.x=WINDOW_WIDTH/2-10;
        ball.y=WINDOW_HEIGHT/2-10;
        speedMultiplier=1.0f;
        ballVelX=shouldLaunchRight?abs(ballVelX):-abs(ballVelX);
        do{
            ballVelY=(rand()%3+1)*(rand()%2==0?1:-1);
        }while(ballVelY==0);

        if(leftScore>=11||rightScore>=11){
            currentRound++;
            if(currentRound>3){
                currentRound=1;
                leftScore=rightScore=0;
                inMenu=true;
            }
            else {  // Xử lý trường hợp round <= 3
                leftScore=rightScore=0;
                waitingForEnter=true;
                shouldLaunchRight=(currentRound%2==0);
                outOfBoundsCount=0;
                }
        }
    }
}
