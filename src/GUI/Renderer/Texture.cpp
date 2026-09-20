#include "Texture.h"

#include <cstring>

Texture::~Texture() {
	if (texture && device) {
		SDL_ReleaseGPUTexture(device, texture);
		texture = nullptr;
	}
}
bool Texture::Create(SDL_GPUDevice* device, int w, int h) {
	this->device = device;
	width = w;
	height = h;
	SDL_GPUTextureCreateInfo info{};
	info.type = SDL_GPU_TEXTURETYPE_2D;
	info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	info.width = w;
	info.height = h;
	info.layer_count_or_depth = 1;
	info.num_levels = 1;
	info.sample_count = SDL_GPU_SAMPLECOUNT_1;
	texture = SDL_CreateGPUTexture(device, &info);
	return texture != nullptr;
}
bool Texture::Upload(SDL_GPUDevice* device, SDL_Surface* surface) {
	Uint32 size = surface->pitch * surface->h;
	SDL_GPUTransferBufferCreateInfo info{};
	info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	info.size = size;
	SDL_GPUTransferBuffer* buffer = SDL_CreateGPUTransferBuffer(device, &info);
	if (!buffer) return false;
	void* mapped = SDL_MapGPUTransferBuffer(device, buffer, false);
	if (!mapped) {
		SDL_ReleaseGPUTransferBuffer(device, buffer);
		return false;
	}
	memcpy(mapped, surface->pixels, size);
	SDL_UnmapGPUTransferBuffer(device, buffer);
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
	if (!cmd) return false;
	SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTextureTransferInfo src{};
	src.transfer_buffer = buffer;
	src.offset = 0;
	SDL_GPUTextureRegion dst{};
	dst.texture = texture;
	dst.w = surface->w;
	dst.h = surface->h;
	dst.d = 1;
	SDL_UploadToGPUTexture(copy, &src, &dst, false);
	SDL_EndGPUCopyPass(copy);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, buffer);
	return true;
}