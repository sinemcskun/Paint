#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
  SDL_Color color;
  SDL_Rect rect;
}Color;

Color palette[] = {
  {{255, 0, 0, 255},   {10, 10, 30, 30}},   // Red
  {{0, 255, 0, 255},   {50, 10, 30, 30}},   // Green
  {{0, 0, 255, 255},   {90, 10, 30, 30}},   // Blue
  {{0, 0, 0, 255},     {130, 10, 30, 30}},  // Black
  {{255, 255, 0, 255}, {170, 10, 30, 30}},  // Yellow
  {{255, 165, 0, 255}, {210, 10, 30, 30}},  // Orange
  {{128, 0, 128, 255}, {250, 10, 30, 30}},  // Purple
  {{128, 128, 128, 255}, {290, 10, 30, 30}}, // Gray
  {{255, 255, 255, 255}, {330, 10, 30, 30}}  // White
};

SDL_Color currentColor = {0,0,0,255}; // default color is black

SDL_Texture* canvasTexture = NULL;

void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius){
  for(int w = 0; w < radius * 2; w++){
    for(int h = 0; h < radius * 2; h++){
      int dx = radius - w;
      int dy = radius - h;
      if(dx*dx + dy*dy <= radius * radius){
        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
      }
    }
  }
}

typedef enum{
  MODE_DRAW,
  MODE_LINE,
  MODE_CIRCLE,
  MODE_RECTANGLE
}DrawMode;

DrawMode currentMode = MODE_DRAW;
SDL_Point startPoint = {0, 0};

int main(){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("Paint", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // make a canvas for texture
  canvasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

  // canvas is white
  SDL_SetRenderTarget(renderer, canvasTexture);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, NULL);

  int running = 1;
  int drawing = 0;
  int erasing = 0;
  SDL_Point previousPoint = {0, 0};
  SDL_Event event;

  while (running){
    while (SDL_PollEvent(&event)){
      if (event.type == SDL_QUIT){
         running = 0;
      }else if (event.type == SDL_MOUSEBUTTONDOWN){
        if(event.button.button == SDL_BUTTON_LEFT){
          erasing = 0;
          for(int i = 0; i < sizeof(palette)/sizeof(palette[0]); i++){
            if(event.button.x >= palette[i].rect.x && event.button.x <= palette[i].rect.x + palette[i].rect.w &&
               event.button.y >= palette[i].rect.y && event.button.y <= palette[i].rect.y + palette[i].rect.h) {
              currentColor = palette[i].color;
              break;
            }
          }
          if (currentMode == MODE_DRAW) {
            drawing = 1;
            previousPoint.x = event.button.x;
            previousPoint.y = event.button.y;
          } else {
            startPoint.x = event.button.x;
            startPoint.y = event.button.y;
          }
        } else if (event.button.button == SDL_BUTTON_RIGHT){
          erasing = 1;
          previousPoint.x = event.button.x;
          previousPoint.y = event.button.y;
        }
      }else if(event.type == SDL_MOUSEBUTTONUP){
        if (event.button.button == SDL_BUTTON_LEFT) {
          if (currentMode == MODE_DRAW) {
            drawing = 0;
          } else {
            SDL_Point endPoint = {event.button.x, event.button.y};
            SDL_SetRenderTarget(renderer, canvasTexture); // use canvas for painting
            SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
            if (currentMode == MODE_LINE) {
              SDL_RenderDrawLine(renderer, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
            } else if (currentMode == MODE_CIRCLE) {
              int radius = (int)sqrt((endPoint.x - startPoint.x) * (endPoint.x - startPoint.x) +
                                     (endPoint.y - startPoint.y) * (endPoint.y - startPoint.y));
              SDL_RenderDrawCircle(renderer, startPoint.x, startPoint.y, radius);
            } else if (currentMode == MODE_RECTANGLE) {
              SDL_Rect rect = {startPoint.x, startPoint.y, endPoint.x - startPoint.x, endPoint.y - startPoint.y};
              SDL_RenderDrawRect(renderer, &rect);
            }
            SDL_SetRenderTarget(renderer, NULL);
          }
        }else if (event.button.button == SDL_BUTTON_RIGHT){
          erasing = 0;
        }
      }else if(event.type == SDL_MOUSEMOTION){
        if(drawing){
          SDL_SetRenderTarget(renderer, canvasTexture); // use canvas for painting
          SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
          SDL_RenderDrawLine(renderer, previousPoint.x, previousPoint.y, event.motion.x, event.motion.y);
          SDL_SetRenderTarget(renderer, NULL);

          previousPoint.x = event.motion.x;
          previousPoint.y = event.motion.y;
        } else if(erasing){
          SDL_SetRenderTarget(renderer, canvasTexture); // erase process be done on canvas
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          SDL_RenderDrawLine(renderer, previousPoint.x, previousPoint.y, event.motion.x, event.motion.y);
          SDL_SetRenderTarget(renderer, NULL);

          previousPoint.x = event.motion.x;
          previousPoint.y = event.motion.y;
        }
      }else if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_s){
          SDL_Surface* saveSurface = SDL_CreateRGBSurfaceWithFormat(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
          SDL_SetRenderTarget(renderer, canvasTexture);
          SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, saveSurface->pixels, saveSurface->pitch);
          SDL_SaveBMP(saveSurface, "drawing.bmp");
          SDL_FreeSurface(saveSurface);
          SDL_SetRenderTarget(renderer, NULL);
        } else if(event.key.keysym.sym == SDLK_1){
          currentMode = MODE_DRAW;
        } else if(event.key.keysym.sym == SDLK_2){
          currentMode = MODE_LINE;
        } else if(event.key.keysym.sym == SDLK_3){
          currentMode = MODE_CIRCLE;
        } else if(event.key.keysym.sym == SDLK_4){
          currentMode = MODE_RECTANGLE;
        } else if(event.key.keysym.sym == SDLK_SPACE){
          running = 0;
        }
      }
    }

    // load canvas
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, canvasTexture, NULL, NULL);

    // color palette
    for (int i = 0; i < 9; i++) {
      SDL_SetRenderDrawColor(renderer, palette[i].color.r, palette[i].color.g, palette[i].color.b, palette[i].color.a);
      SDL_RenderFillRect(renderer, &palette[i].rect);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(canvasTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
