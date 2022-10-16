#include <complex>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <SDL.h>

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

std::complex<double> iterateMandelbrot(std::complex<double> constant, std::complex<double> x) {
    return x*x+constant;
}

int numIterations(std::complex<double> constant, int maxIteration) {
    int iteration = 0;
    std::complex<double> curr = 0;
    while (abs(curr) <= 2 && iteration<maxIteration) {
        curr = curr*curr+constant;
        iteration++;
    }
    return iteration;
}

std::vector<std::vector<int>> renderMandelbrot(std::complex<double> center, double zoom, int width, int height, int maxIteration) {
    std::vector<std::vector<int>> coordPlane(height, std::vector<int>(width));
    for (int i = 0; i < height; i ++) {
        for (int j = 0; j < width; j ++) {
            std::complex<double> currPoint = std::complex<double>((double)j-(double)width/2.0,(double)height/2.0-(double)i)/pow(2,zoom)+center;
            coordPlane[i][j] = numIterations(currPoint, maxIteration);
        }
    }
    return coordPlane;
}

void drawPoint(SDL_Renderer* renderer, int x, int y, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

int main() {
    //std::vector<char> charScale = {' ', ' ', '.', '\'',',','_','=','*','@','#'};
    int windowWidth = 1000;
    int windowHeight = 500;
    int maxIteration = 200;
    int width = windowWidth;
    int height = windowHeight;
    double zoom = 4;
    std::complex<double> center = {-0.742,0.15};

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    int sdl_result = SDL_Init(SDL_INIT_VIDEO);
    if (sdl_result != 0) {
        SDL_Log("SDL initialization error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "CG From Scratch",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN
    );
    if (!window) {
        SDL_Log("Window creation error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    bool running = true;
    Uint32 ticks_count = 0;
    float frame_rate = 60.0;
    unsigned int cpu_count = std::thread::hardware_concurrency();
    while (running) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_MOUSEWHEEL) {
                if(event.wheel.y > 0) // scroll up
                {
                    zoom += 0.2;
                }
                else if(event.wheel.y < 0) // scroll down
                {
                    // Put code for handling "scroll down" here!
                    zoom -= 0.2;
                }
            }
        }
        while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticks_count + 1000.0f / frame_rate));
        
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::vector<std::vector<int>> screen = renderMandelbrot(center, zoom, width, height, maxIteration);
        for (int i = 0; i < windowWidth; i ++) {
            for (int j = 0; j < windowHeight; j ++) {
                drawPoint(renderer, i, j, Color{(Uint8) screen[j][i], 0,0,255});
            }
        }
        SDL_RenderPresent(renderer);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time to render = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "[ms]" << std::endl;
        //zoom+=0.2;
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        std::cout << center << std::endl;
    }
    SDL_Quit();
    return 0;
}