#include <stdio.h>
#include "textures.h"


#define NUM_TEXTURES 11 

static const char* TexturesPaths[NUM_TEXTURES] = {
    /*0*/"./data/textures/no_texture.png",
    /*1*/"./data/sprites/jelly_normal.png",
    /*2*/"./data/sprites/jelly_frozen.png",
    /*3*/"./data/sprites/jelly_explosion.png",
    /*4*/"./data/sprites/shotgun_gg.png",
    /*5*/"./data/sprites/planet.png",
	/*6*/"./data/sprites/gun_static.png",
	/*7*/"./data/textures/corridor_wall.png",
	/*8*/"./data/textures/BIGSQUARES.png",
	/*9*/"./data/textures/door_open.png",
	/*10*/"./data/textures/door_closed.png",
};

// Hold all textures neccessary in game.
texture_t Textures[NUM_TEXTURES];


// TODO: Later on I need to get rid of upng somehow and use only my texture_t struct.
// Why? To have less dependencies. When I will be doing some porting code: I want to
// have as little dependencies as I can have. No UPNG, no SDL etc. But for now let's
// let it as it is.
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
                // TODO: add assert here and some warning messages
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
        upng_t* Texture = UPNGLoadedTextures[i];
        if (Texture) {
            upng_free(Texture);
            UPNGLoadedTextures[i] = 0;
        }
    }
}
