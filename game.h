#ifndef GAME_H
#define GAME_H
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_audio.h>
#include<iostream>
using namespace std;
// Kích thước cửa sổ
const int WINDOW_WIDTH=800;
const int WINDOW_HEIGHT=600;
// Kích thước chữ số
const int DIGIT_W=20;
const int DIGIT_H=30;
// Biến toàn cục
extern int leftScore;       // Điểm trái
extern int rightScore;      // Điểm phải
extern int currentRound;    // Vòng hiện tại
extern SDL_Texture* digitsTex; // Texture số
extern bool shouldLaunchRight; // Hướng phóng tiếp theo
extern bool waitingForEnter;   // Đang chờ nhấn Enter
extern int outOfBoundsCount;  // Đếm số lần bóng ra biên
// Cấu trúc màu
struct Color{Uint8 r,g,b,a;Color(Uint8 r=255,Uint8 g=255,Uint8 b=255,Uint8 a=255):r(r),g(g),b(b),a(a){}};
// Hàm tải texture
SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer);
// Hàm vẽ
void DrawDigit(SDL_Renderer* renderer, SDL_Texture* tex, int num, int x, int y);
void DrawScore(SDL_Renderer* renderer, SDL_Texture* digits, SDL_Texture* scoreLabel, SDL_Texture* roundLabel);
// Hàm xử lý menu
void RunMainMenu(SDL_Renderer* renderer, SDL_Texture* bgMain, SDL_Texture* playBtnImg, SDL_Texture* exitBtnImg, bool& quit, bool& inMenu, bool& inColorMenu);
void RunColorMenu(SDL_Renderer* renderer, SDL_Texture* bgMenus[], Color rainbowColors[], int& currentColorMenu, Color& paddleLeftColor, Color& paddleRightColor, Color& ballColor, bool& inColorMenu, bool& quit);
// Hàm game logic
void HandleGameInput(const Uint8* keystates, SDL_Rect& paddleLeft, SDL_Rect& paddleRight);
void UpdateGame(SDL_Rect& ball, int& ballVelX, int& ballVelY, float& speedMultiplier, const SDL_Rect& paddleLeft, const SDL_Rect& paddleRight, bool& waitingForEnter);
#endif
