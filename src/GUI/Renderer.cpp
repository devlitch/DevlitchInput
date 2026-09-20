#include "Renderer.h"

#include "Renderer/Texture.h"

#include <SDL3_image/SDL_image.h>

bool Renderer::Init(SDL_GPUDevice* gpu) {
	device = gpu;
	return device != nullptr;
}
Texture* Renderer::LoadTexture(const std::string& file) {
	SDL_Surface* loaded = IMG_Load(file.c_str());
	if (!loaded) return nullptr;
	SDL_Surface* surface = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_ABGR8888);
	SDL_DestroySurface(loaded);
	if (!surface) return nullptr;
	Texture* texture = new Texture();
	if (!texture->Create(device, surface->w, surface->h)) {
		delete texture;
		SDL_DestroySurface(surface);
		return nullptr;
	}
	if (!texture->Upload(device, surface)) {
		delete texture;
		SDL_DestroySurface(surface);
		return nullptr;
	}
	SDL_DestroySurface(surface);
	return texture;
}
Texture* Renderer::LoadTextureFromMemory(const void* data, size_t size) {
	SDL_IOStream* io = SDL_IOFromConstMem(data, size);
	if (!io) return nullptr;
	SDL_Surface* loaded = IMG_Load_IO(io, true);
	if (!loaded) return nullptr;
	SDL_Surface* surface = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_ABGR8888);
	SDL_DestroySurface(loaded);
	if (!surface) return nullptr;
	Texture* texture = new Texture();
	if (!texture->Create(device, surface->w, surface->h)) {
		delete texture;
		SDL_DestroySurface(surface);
		return nullptr;
	}
	if (!texture->Upload(device, surface)) {
		delete texture;
		SDL_DestroySurface(surface);
		return nullptr;
	}
	SDL_DestroySurface(surface);
	return texture;
}
void Renderer::Destroy() {
	device = nullptr;
}