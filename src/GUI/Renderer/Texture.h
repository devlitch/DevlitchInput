#pragma once

#include <SDL3/SDL.h>

class Texture {
public: Texture() =
	default;
	  ~Texture();
	  bool Create(SDL_GPUDevice* device, int width, int height);
	  bool Upload(SDL_GPUDevice* device, SDL_Surface* surface);
	  SDL_GPUTexture* Get() const {
		  return texture;
	  }
	  int Width() const {
		  return width;
	  }
	  int Height() const {
		  return height;
	  }
private:
		SDL_GPUDevice* device = nullptr;
	    SDL_GPUTexture* texture = nullptr;
	    int width = 0;
	    int height = 0;
};