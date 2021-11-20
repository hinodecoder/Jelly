#include <stdio.h>
#include "textures.h"

static const char* TexturesPaths[NUM_TEXTURES] = {
    "./data/textures/bricks.png",
    "./data/textures/stone.png"
};

// Hold all textures neccessary in game.
texture_t Textures[NUM_TEXTURES];


upng_t* UPNGLoadedTextures[NUM_TEXTURES] = {NULL};

void LoadTextures(void) {
    for (int32_t i=0; i < NUM_TEXTURES; ++i) {
        upng_t* Upng = NULL;
        Upng = upng_new_from_file(TexturesPaths[i]);
        
        if (Upng != NULL) {
            upng_decode(Upng);
            
            if (upng_get_error(Upng) == UPNG_EOK) {
                // File loaded.
                Textures[i].Width = upng_get_width(Upng);
                Textures[i].Height = upng_get_height(Upng);
                Textures[i].Buffer = (uint32_t*) upng_get_buffer(Upng);
                UPNGLoadedTextures[i] = Upng;
            } else {
                // TODO: add assert here
                printf("Cannot load texture %s\n", TexturesPaths[i]);
            }
        }
    }
}

texture_t* GetTexture(int32_t Index) {
    // TODO: Add runtime error when trying to access index out of array boundaries.
    return &Textures[Index];
}

void FreeTextures(void) {
    for (int32_t i=0; i < NUM_TEXTURES; ++i) {
        upng_free(UPNGLoadedTextures[i]);
    }
}
