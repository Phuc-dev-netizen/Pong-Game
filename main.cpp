#include<SDL.h>
#include<iostream>
#include<SDL_image.h>
using namespace std;

// [MỚI] Bảng điểm
int leftScore=0;       // Điểm trái
int rightScore=0;      // Điểm phải
int currentRound=1;    // Vòng hiện tại
SDL_Texture* digitsTex=NULL; // Texture số
const int DIGIT_W=20;  // Chiều rộng số
const int DIGIT_H=30;  // Chiều cao số

// [MỚI] Biến điều khiển hướng bóng
int outOfBoundsCount=0;      // Đếm số lần bóng ra biên
bool shouldLaunchRight=true; // Hướng phóng tiếp theo

const int WINDOW_WIDTH=800;
const int WINDOW_HEIGHT=600;

// Bảng màu
SDL_Color rainbowColors[7]={
 {255,0,0,255},    // Đỏ
 {255,127,0,255},  // Cam
 {255,255,0,255},  // Vàng
 {0,255,0,255},    // Lục
 {0,0,255,255},    // Lam
 {148,0,211,255},  // Tím
 {255,255,255,255} // Trắng
};

// Hàm tải ảnh và trả về texture
SDL_Texture*LoadTexture(const char*path,SDL_Renderer*renderer){
 SDL_Surface*surface=IMG_Load(path);
 if(!surface){
  cerr<<"Failed to load image: "<<path<<" - "<<IMG_GetError()<<endl;
  return nullptr;
 }
 SDL_Texture*texture=SDL_CreateTextureFromSurface(renderer,surface);
 SDL_FreeSurface(surface);
 return texture;
}

// [MỚI] Hàm vẽ số (0-10)
void DrawDigit(SDL_Renderer* renderer,SDL_Texture* tex,int num,int x,int y){
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

// [MỚI] Hàm vẽ bảng điểm
void DrawScore(SDL_Renderer* renderer, SDL_Texture* digits, SDL_Texture* scoreLabel, SDL_Texture* roundLabel){
 SDL_Rect bg={WINDOW_WIDTH/2-140,10,280,70};
 SDL_SetRenderDrawColor(renderer,30,30,30,255);
 SDL_RenderFillRect(renderer,&bg);

 // Vẽ chữ SCORE ở bên trái
 SDL_Rect scoreLabelRect={WINDOW_WIDTH/2-135,15,60,15}; // vị trí và kích thước
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


int main(int argc,char*argv[]){
 if(SDL_Init(SDL_INIT_VIDEO)<0){
  cerr<<"SDL could not initialize: "<<SDL_GetError()<<endl;
  return 1;
 }
 if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)){
  cerr<<"SDL_image could not initialize: "<<IMG_GetError()<<endl;
  return 1;
 }
 SDL_Window*window=SDL_CreateWindow("Game bóng bàn",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
 if(!window){
  cerr<<"Window could not be created: "<<SDL_GetError()<<endl;
  return 1;
 }
 SDL_Renderer*renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
 if(!renderer){
  cerr<<"Renderer could not be created: "<<SDL_GetError()<<endl;
  return 1;
 }

 // Tải ảnh nền và nút (đã xóa game_bg.png)
 SDL_Texture*bgMain=LoadTexture("menu_main.png",renderer);
 SDL_Texture*bgMenus[3]={
  LoadTexture("menu_leftPaddle.png",renderer),
  LoadTexture("menu_rightPaddle.png",renderer),
  LoadTexture("menu_ball.png",renderer)
 };
 SDL_Texture*playBtnImg=LoadTexture("button_play.png",renderer);
 SDL_Texture*exitBtnImg=LoadTexture("button_exit.png",renderer);

 // [MỚI] Tải texture số
 digitsTex=LoadTexture("digits.png",renderer);
 if(!digitsTex){
  cerr<<"Failed to load digits texture!"<<endl;
  return 1;
 }

 SDL_Texture*scoreLabelTex=LoadTexture("score.png",renderer); //Texture chữ score
 SDL_Texture*roundLabelTex=LoadTexture("round.png",renderer); //Texture chữ round
 if(!scoreLabelTex||!roundLabelTex){
	cerr<<"Failed to load score/round label texture!"<<endl;
	return 1;
}
 SDL_Rect paddleLeft={10,WINDOW_HEIGHT/2-50,10,100};
 SDL_Rect paddleRight={WINDOW_WIDTH-20,WINDOW_HEIGHT/2-50,10,100};
 SDL_Rect ball={WINDOW_WIDTH/2-10,WINDOW_HEIGHT/2-10,20,20};
 int ballVelX=5,ballVelY=5;
 float speedMultiplier=1.0f; // [MỚI] Hệ số tốc độ ban đầu
 const float SPEED_INCREASE=0.05f; // [MỚI] Tốc độ tăng sau mỗi lần đập

 SDL_Color paddleLeftColor={255,255,255,255};
 SDL_Color paddleRightColor={255,255,255,255};
 SDL_Color ballColor={255,255,255,255};

 bool quit=false;
 SDL_Event e;
 bool inMenu=true;
 bool inColorMenu=false;

 // Biến kiểm tra hover chuột
 bool isPlayHovered = false;
 bool isExitHovered = false;
 int currentColorMenu=0; // 0 = thanh trái, 1 = thanh phải, 2 = bóng

 while(!quit){
  // Giao diện menu chính
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

   // Render với kích thước đã điều chỉnh
   SDL_RenderCopy(renderer,playBtnImg,NULL,&playBtnRender);
   SDL_RenderCopy(renderer,exitBtnImg,NULL,&exitBtnRender);
   SDL_RenderPresent(renderer);

   while(SDL_PollEvent(&e)){
    if(e.type==SDL_QUIT){
     quit=true;
     inMenu=false;
    }
    if(e.type==SDL_MOUSEMOTION){  // Sụ kiện di chuột
     int x,y;
     SDL_GetMouseState(&x,&y);

     // Kiểm tra hover nút Play
     isPlayHovered=(x>=playBtn.x&&x<=playBtn.x+playBtn.w&&y>=playBtn.y&&y<=playBtn.y+playBtn.h);

     // Kiểm tra hover nút Exit
     isExitHovered=(x>=exitBtn.x&&x<=exitBtn.x+exitBtn.w&&y>=exitBtn.y&&y<=exitBtn.y+exitBtn.h);
    }
    if(e.type==SDL_MOUSEBUTTONDOWN){
     int x,y;
     SDL_GetMouseState(&x,&y);
     if(x>=playBtn.x&&x<=playBtn.x+playBtn.w&&y>=playBtn.y&&y<=playBtn.y+playBtn.h){
      inColorMenu=true;
      inMenu=false;
      currentColorMenu=0;
     }
     if(x>=exitBtn.x&&x<=exitBtn.x+exitBtn.w&&y>=exitBtn.y&&y<=exitBtn.y+exitBtn.h){
      quit=true;
      inMenu=false;
     }
    }
   }
  }

  // Giao diện chọn màu
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

  // Xử lý sự kiện khi chơi
  while(SDL_PollEvent(&e)){
   if(e.type==SDL_QUIT) quit=true;
  }

  const Uint8*keystates=SDL_GetKeyboardState(NULL);
  if(keystates[SDL_SCANCODE_W]&&paddleLeft.y>0) paddleLeft.y-=5;
  if(keystates[SDL_SCANCODE_S]&&paddleLeft.y+paddleLeft.h<WINDOW_HEIGHT) paddleLeft.y+=5;
  if(keystates[SDL_SCANCODE_UP]&&paddleRight.y>0) paddleRight.y-=5;
  if(keystates[SDL_SCANCODE_DOWN]&&paddleRight.y+paddleRight.h<WINDOW_HEIGHT) paddleRight.y+=5;

  // Cập nhật vị trí bóng (đã thêm tính năng tăng tốc)
  ball.x+=(int)(ballVelX*speedMultiplier); // [MỚI] Áp dụng hệ số tốc độ
  ball.y+=(int)(ballVelY*speedMultiplier); // [MỚI] Áp dụng hệ số tốc độ
  if(ball.y<=0||ball.y+ball.h>=WINDOW_HEIGHT) ballVelY=-ballVelY;
  if(SDL_HasIntersection(&ball,&paddleLeft)||SDL_HasIntersection(&ball,&paddleRight)){
   ballVelX=-ballVelX;
   speedMultiplier+=SPEED_INCREASE; // [MỚI] Tăng tốc sau mỗi lần đập
  }
  if(ball.x<0||ball.x>WINDOW_WIDTH){
   // [MỚI] Tính điểm
   if(ball.x<0) rightScore++;
   else leftScore++;

   // [MỚI] Tăng biến đếm và kiểm tra đổi hướng
   outOfBoundsCount++;
   if(outOfBoundsCount%2==0){ // Mỗi 2 lần thì đổi hướng
       shouldLaunchRight=!shouldLaunchRight;
   }
   ballVelX=shouldLaunchRight?abs(ballVelX):-abs(ballVelX);

   // Reset vị trí bóng
   ball.x=WINDOW_WIDTH/2-10;
   ball.y=WINDOW_HEIGHT/2-10;
   speedMultiplier=1.0f; // Reset tốc độ khi ra khỏi biên

   // [MỚI] Đặt hướng bóng
   ballVelX = shouldLaunchRight ? abs(ballVelX) : -abs(ballVelX);
   do {
       ballVelY = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
   } while (ballVelY == 0); // Lặp đến khi có góc chéo

   if(leftScore>=11||rightScore>=11){
    currentRound++;
    leftScore=rightScore=0;
   }
   ball.x=WINDOW_WIDTH/2-10;
   ball.y=WINDOW_HEIGHT/2-10;
   speedMultiplier=1.0f;
   shouldLaunchRight=!shouldLaunchRight; // [MỚI] Đổi hướng khi kết thúc round
  }

  // Vẽ khung hình
  SDL_SetRenderDrawColor(renderer,0,0,0,255);
  SDL_RenderClear(renderer);

  // [MỚI] Vẽ bảng điểm
  DrawScore(renderer,digitsTex,scoreLabelTex,roundLabelTex);

  SDL_SetRenderDrawColor(renderer,paddleLeftColor.r,paddleLeftColor.g,paddleLeftColor.b,255);
  SDL_RenderFillRect(renderer,&paddleLeft);

  SDL_SetRenderDrawColor(renderer,paddleRightColor.r,paddleRightColor.g,paddleRightColor.b,255);
  SDL_RenderFillRect(renderer,&paddleRight);

  SDL_SetRenderDrawColor(renderer,ballColor.r,ballColor.g,ballColor.b,255);
  SDL_RenderFillRect(renderer,&ball);

  SDL_RenderPresent(renderer);
  SDL_Delay(16); // FPS Cap:60
 }

 // Giải phóng tài nguyên
 SDL_DestroyTexture(bgMain);
 for(int i=0;i<3;++i) SDL_DestroyTexture(bgMenus[i]);
 SDL_DestroyTexture(playBtnImg);
 SDL_DestroyTexture(exitBtnImg);

 // [MỚI] Giải phóng texture số
 SDL_DestroyTexture(digitsTex);

 // [MỚI] Giải phóng texture chữ
 SDL_DestroyTexture(scoreLabelTex);
 SDL_DestroyTexture(roundLabelTex);

 SDL_DestroyRenderer(renderer);
 SDL_DestroyWindow(window);
 IMG_Quit();
 SDL_Quit();
 return 0;
}
