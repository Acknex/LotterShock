#include "map.h"
#include "input.h"
#include <acknex.h>

VECTOR map_levelboundary_min;
VECTOR map_levelboundary_max;

BMAP * map_original = "map_data.tga";
BMAP * map_marker = "map_marker.tga";

BMAP * map_primary_image;
BMAP * map_secondary_image;

// [0] = current image
// [1] = buffered image
BMAP * map_images[2];

bool map_visible;

void map_init()
{
    vec_set(map_levelboundary_min, vector(-1214, -9994, -2506));
    vec_set(map_levelboundary_max, vector(17672, 9490, 5234));

    map_primary_image = bmap_createblack(
        bmap_width(map_original),
        bmap_height(map_original),
        8888);
    map_secondary_image = bmap_createblack(
        bmap_width(map_original),
        bmap_height(map_original),
        8888);

    map_images[0] = map_primary_image;
    map_images[1] = map_secondary_image;
}

void map_open()
{
    int i;
    for(i = 0; i < 2; i++)
    {
        bmap_blit(map_images[i], map_original, NULL, NULL);
        // bmap_to_mipmap(map_images[i]);
    }
    map_visible = false;
}

void map_lock()
{
	if (map_images[0])
	{
		bmap_to_format(map_images[0], 8888);
	}	
	if (map_images[1])
	{
		bmap_to_format(map_images[1], 8888);
	}	
}

MATERIAL * map_mtl_render =
{
    effect = "map_render.fx";
    flags = AUTORELOAD;
}

void map_update()
{
    if(player == NULL)
        return;

    if(input_hit(INPUT_SHOW_MAP))
        map_visible = !map_visible;

    var aspect = bmap_width(map_images[0]) / bmap_height(map_images[0]);

    var height = bmap_height(map_images[0]);
    var width = bmap_width(map_images[0]);

    var scale = height / bmap_height(map_images[0]);

    var left = (screen_size.x - width) / 2;
    var top = (screen_size.y - height) / 2;

    var px = ((float)width * (player->x - (float)map_levelboundary_min.x)) / (float)(map_levelboundary_max.x - map_levelboundary_min.x);
    var py = height - 1 - ((float)height * (player->y - (float)map_levelboundary_min.y)) / (float)(map_levelboundary_max.y - map_levelboundary_min.y);

    map_mtl_render->skill1 = floatv(px / width);
    map_mtl_render->skill2 = floatv(py / height);

    { // swap bmaps
        BMAP * tmp = map_images[0];
        map_images[0] = map_images[1];
        map_images[1] = tmp;
    }

    bmap_process(map_images[0], map_images[1], map_mtl_render);

    if(map_visible == false)
        return;

    draw_quad(
        map_images[0],
        vector(left, top, 0),
        NULL,
        NULL,
        vector(scale, scale, 1),
        NULL,
        100,
        0);

    draw_quad(
        map_marker,
        vector(left + px - 4, top + py - 4, 0),
        NULL,
        NULL,
        NULL,
        COLOR_RED,
        100,
        player->pan - 90);

}

void map_close()
{

}
