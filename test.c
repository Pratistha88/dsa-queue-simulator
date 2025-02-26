#include <SDL3/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL3 Test Window", 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          800, 600, 
                                          SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Main event loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear screen with a color
        SDL_SetRenderDrawColor(SDL_GetRenderer(window), 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(SDL_GetRenderer(window));
        
        // Update the screen
        SDL_RenderPresent(SDL_GetRenderer(window));
    }

    // Cleanup and quit SDL
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
