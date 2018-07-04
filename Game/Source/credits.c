#include "credits.h"
#include "input.h"
#include "music_player.h"

#include <acknex.h>

STRING * credits_source = "credits.dat";
var credits_scrollSpeed = 2.8; // 1 Pixel per quant
var credits_speedup = 1;
var credits_speedup_factor = 4;

// Font configuration
STRING * credits_fontName  = "Arial";
STRING * credits_fontFlags = "b";

// Font sizes for h1,h2,h3,other
var credits_hugeFontSize   = 80;
var credits_largeFontSize  = 60;
var credits_mediumFontSize = 40;
var credits_normalFontSize = 30;

// Distance of an image to the left/right screen border
var credits_imageMarginW   = 80;

// Distance of an image to text above/below.
var credits_imageMarginH   = 30;

// Distance of the text to the left/right screen border
var credits_textMarginW    = 60;

// Length of the filmic separator
var credits_filmicSepW     = 16;

// Distance between the two columns of the filmic display
var credits_filmicTextDist = 32;

typedef struct CreditsNode
{
    var height;
    var alignment; /* 0=left, 1=middle, 2=right */
    int fuckYouType; /* 0=text, 1=image, 2=empty, 3=filmic */
    STRING * text1;
    STRING * text2;
    BMAP   * image;
    int font;
    struct CreditsNode * next;
} CreditsNode;

CreditsNode * credits_firstNode = NULL;

FONT * credits_fontset[4];

TEXT * credits_text =
{
    strings = 1;
    string ( "This is stupid!");
    flags = OUTLINE;
}

BMAP* credits_bmp_ff = "credits_ff.tga";

PANEL* credits_pnl_ff =
{
    pos_x = 0;
    pos_y = 0;
    size_x = 32;
    size_y = 16;
    bmap = credits_bmp_ff;
    flags = FILTER;
//	flags = SHOW;
}

#define CREDITS_BACKGROUNDS_MAX 100

BMAP * credits_backgrounds[CREDITS_BACKGROUNDS_MAX];
VECTOR credits_background_dirs[CREDITS_BACKGROUNDS_MAX];
int credits_background_count;

var credits_timer;
var credits_fadeout;
var credits_render;

var credits_done = 0;

void credits_init()
{
    credits_imageMarginW = screen_size.x * 0.2;
    credits_textMarginW  = screen_size.x * 0.2;

    STRING * temp = "#100";
    credits_fontset[0] = font_create(str_printf(temp,
        "%s#%d%s",
        _chr(credits_fontName),
        (int)credits_normalFontSize,
        _chr(credits_fontFlags)));
    credits_fontset[1] = font_create(str_printf(temp,
        "%s#%d%s",
        _chr(credits_fontName),
        (int)credits_mediumFontSize,
        _chr(credits_fontFlags)));
    credits_fontset[2] = font_create(str_printf(temp,
        "%s#%d%s",
        _chr(credits_fontName),
        (int)credits_largeFontSize,
        _chr(credits_fontFlags)));
    credits_fontset[3] = font_create(str_printf(temp,
        "%s#%d%s",
        _chr(credits_fontName),
        (int)credits_hugeFontSize,
        _chr(credits_fontFlags)));

    if(credits_fontset[0] == NULL) error("Failed to init font 0");
    if(credits_fontset[1] == NULL) error("Failed to init font 1");
    if(credits_fontset[2] == NULL) error("Failed to init font 2");
    if(credits_fontset[3] == NULL) error("Failed to init font 3");

    var f = file_open_read(credits_source);

    STRING * tempString = "#512";
    STRING * fuckYouTypeString = "#512";
    STRING * inputString = "#512";

    str_cpy(delimit_str, ";\n");

    diag("\nLoading credits data...");

    CreditsNode * it = NULL;
    while(TRUE)
    {
        if(file_str_readto(f, fuckYouTypeString, NULL, 512) < 0) {
            break;
        }
        CreditsNode * curr = malloc(sizeof(CreditsNode));
        memset(curr, 0, sizeof(CreditsNode));

        if(str_cmp("h1", fuckYouTypeString)) {
            file_str_read(f, inputString);
            curr.fuckYouType = 0;
            curr.font = 3;
            curr.text1 = str_create(inputString);
            curr.alignment = 1;
        }
        else if(str_cmp("h2", fuckYouTypeString)) {
            file_str_read(f, inputString);
            curr.fuckYouType = 0;
            curr.font = 2;
            curr.text1 = str_create(inputString);
            curr.alignment = 1;
        }
        else if(str_cmp("h3", fuckYouTypeString)) {
            file_str_read(f, inputString);
            curr.fuckYouType = 0;
            curr.font = 1;
            curr.text1 = str_create(inputString);
            curr.alignment = 1;
        }
        else if(str_cmp("c", fuckYouTypeString)) {
            file_str_read(f, inputString);
            curr.fuckYouType = 0;
            curr.text1 = str_create(inputString);
            curr.alignment = 1;
        }
        else if(str_cmp("l", fuckYouTypeString)) {
            file_str_read(f, inputString);
            curr.fuckYouType = 0;
            curr.text1 = str_create(inputString);
            curr.alignment = 0;
        }
        else if(str_cmp("r", fuckYouTypeString)) {
            file_str_read(f, inputString);
            curr.fuckYouType = 0;
            curr.text1 = str_create(inputString);
            curr.alignment = 2;
        }
        else if(str_cmp("x", fuckYouTypeString)) {
            curr.fuckYouType = 0;

            file_str_read(f, inputString);
            curr.text1 = str_create(inputString);

            file_str_read(f, inputString);
            curr.text2 = str_create(inputString);

            curr.alignment = 0;
        }
        else if(str_cmp("f", fuckYouTypeString)) {
            curr.fuckYouType = 3;

            file_str_read(f, inputString);
            curr.text1 = str_create(inputString);

            file_str_read(f, inputString);
            curr.text2 = str_create(inputString);

            curr.alignment = 0;
        }
        else if(str_cmp("i", fuckYouTypeString)) {
            curr.fuckYouType = 1;

            file_str_read(f, inputString);
            curr.image = bmap_create(inputString);
            if(curr.image == NULL) {
                str_printf(
                    tempString,
                    "Image %s not found!",
                    _chr(inputString));
                error(tempString);
            }

            curr.alignment = 1;
        }
        else if(str_cmp("s", fuckYouTypeString)) {
            curr.fuckYouType = 2;
            file_str_read(f, inputString);
            curr.height = credits_normalFontSize * str_to_num(inputString);
        }
        else {
            str_printf(
                tempString,
                "Unknown directive recognized: %s",
                _chr(fuckYouTypeString));
            error(tempString);
        }

        if(curr != NULL) {
            diag_var("\nnode(%f)", (var)curr.fuckYouType);
            if(it != NULL ) {
                it->next = curr;
            } else {
                credits_firstNode = curr;
            }
            it = curr;
        }
    }

    file_close(f);

    // Load all credits backgrounds
    for(credits_background_count = 0; credits_background_count < CREDITS_BACKGROUNDS_MAX; credits_background_count ++)
    {
        STRING * fname = str_printf(NULL, "screen_%d.jpg", (credits_background_count + 1));

        credits_background_dirs[credits_background_count].x = random(2) - 1;
        credits_background_dirs[credits_background_count].y = random(2) - 1;
        credits_background_dirs[credits_background_count].z = random(2) - 1;

        if(credits_background_count > 0)
        {
            // Flip image direction if direction would be roughly the same
            if(vec_dot(credits_background_dirs[credits_background_count], credits_background_dirs[credits_background_count- 1]) > 0)
                vec_scale(credits_background_dirs[credits_background_count], -1);
        }

        credits_backgrounds[credits_background_count] = bmap_create(fname);
        if(credits_backgrounds[credits_background_count] == NULL)
            break;
    }
}

void credits_open()
{
    if(credits_firstNode == NULL)
    {
        error("Error in credits: failed to initialize credits!");
        return;
    }

    credits_pnl_ff.scale_x = (screen_size.x/16)/32;
    credits_pnl_ff.scale_y = (screen_size.y/16)/16;

    //error(str_for_num(NULL, credits_pnl_ff.scale_x));

    //credits_pnl_ff.scale_x = 10;
    //credits_pnl_ff.scale_y = 10;

    // Kill all evil in this world!
	fog_color = 2;
	camera.fog_end = 20000.0;
    level_load(NULL);

    music_start("media\\credits.mp3", 0.5, 0);

    credits_done = 0;
    credits_timer = 0;
    credits_fadeout = 0;
    credits_render = 1;
}

void credits_close()
{
    music_speedup(100);
    music_start("media\\intro.mp3", 0.5, 0);
	fog_color = 2;
	camera.fog_end = 20000.0;
    level_load(NULL);
    credits_pnl_ff.flags &= ~SHOW;
}

void credits_draw_bg(int id, var fOffset, var alpha)
{
    BMAP * bmp = credits_backgrounds[id];

    var scrollLen = 0.15 * maxv(screen_size.x, screen_size.y);

    var scale_x = (screen_size.x + 2.0 * scrollLen) / bmap_width(bmp);
    var scale_y = (screen_size.y + 2.0 * scrollLen) / bmap_height(bmp);
    var scale = maxv(scale_x, scale_y);

    VECTOR offset;
    vec_set(offset, credits_background_dirs[id]);
    vec_normalize(offset, scrollLen);

    draw_quad(
        bmp,
        vector(
            fOffset * offset.x - scrollLen,
            fOffset * offset.y - scrollLen,
            0),
        NULL,
        vector(bmap_width(bmp), bmap_height(bmp), 0),
        vector(scale, scale, 0),
        NULL,
        alpha,
        0);

}

void credits_update()
{
    if(input_hit(INPUT_ATTACK) || input_hit(INPUT_BLOCK) || input_hit(INPUT_NAVBACK) || input_hit(INPUT_USE))
	{
        credits_done = 1;
        return;
    }

    if(credits_background_count > 0)
    {
        var bg_id = (total_ticks / 256);

        // clamp to [0 ... credits_background_count+0.99999]
        bg_id -= credits_background_count * integer(bg_id / credits_background_count);

        var fOffset = fraction(bg_id);

        credits_draw_bg(integer(bg_id), fOffset, 100);

        var altemp = 100.0 * (10.0 * (fraction(bg_id) - 0.9)); // , 0.0, 1.0);
        var alnext = 100 - clamp(-altemp, 0, 100);
        if(alnext >= 0)
        {
            credits_draw_bg(integer(bg_id + 1) % credits_background_count, fOffset - 1.0, alnext);
        }
    }



    CreditsNode * it;
    var h = 0;
    var y = screen_size.y - credits_timer;

    if(credits_render)
    {
        for(it = credits_firstNode; it != NULL; it = it->next, y += h)
        {
            if(y >= screen_size.y) {
                break;
            }
            h = it.height;
            COLOR * color = COLOR_RED;
            switch(it->fuckYouType)
            {
                case 0: // text
                    credits_text.font = credits_fontset[it.font];
                    credits_text.pos_y = y;

                    credits_text.flags &= ~(CENTER_X | ARIGHT);
                    switch(it.alignment) {
                        case 0:
                            credits_text.pos_x = credits_textMarginW;
                        case 1:
                            credits_text.pos_x = 0.5 * screen_size.x;
                            credits_text.flags |= CENTER_X;
                            break;
                        case 2:
                            credits_text.pos_x = screen_size.x - credits_textMarginW;
                            credits_text.flags |= ARIGHT;
                            break;
                    }

                    if(it.text1) {
                        (credits_text.pstring)[0] = it.text1;
                        draw_obj(credits_text);
                    }

                    if(it.text2) {
                        credits_text.pos_x = screen_size.x - credits_textMarginW;
                        credits_text.flags |= ARIGHT;
                        (credits_text.pstring)[0] = it.text2;
                        draw_obj(credits_text);
                    }

                    y += credits_text.font.dy;
                    break;
                case 1: // image
                    h = bmap_height(it.image);
                    var w = bmap_width(it.image);
                    var sc = (screen_size.x - 2*credits_imageMarginW) / w;
                    draw_quad(
                        it.image,
                        vector(
                            0.5 * (screen_size.x - sc * w),
                            y + credits_imageMarginH,
                            0),
                        NULL,
                        vector(w, h, 0), vector(sc, sc, 0),
                        NULL, 100, 0);
                    h *= sc;
                    h += 2 * credits_imageMarginH;
                    break;
                case 2: // space
                    break;

                case 3: // filmic
                    credits_text.font = credits_fontset[it.font];
                    credits_text.pos_y = y;

                    credits_text.flags &= ~(CENTER_X | ARIGHT);
                    credits_text.flags |= ARIGHT;
                    credits_text.pos_x = 0.5 * screen_size.x - 0.5 * credits_filmicTextDist;
                    (credits_text.pstring)[0] = it.text1;
                    draw_obj(credits_text);

                    draw_quad(
                        NULL,
                        vector(
                            0.5 * screen_size.x - 0.5 * credits_filmicSepW,
                            y + 0.5 * credits_text.font.dy,
                            0),
                        NULL,
                        vector(credits_filmicSepW, 1, 0),
                        NULL,
                        COLOR_WHITE,
                        100,
                        0);

                    credits_text.flags &= ~(CENTER_X | ARIGHT);
                    credits_text.pos_x = 0.5 * screen_size.x + 0.5 * credits_filmicTextDist;
                    (credits_text.pstring)[0] = it.text2;
                    draw_obj(credits_text);

                    y += credits_text.font.dy;
                    break;
                default:
                    error(str_printf(NULL, "invalid fuckYouType: %d", it.fuckYouType));
                    break;
            }
        }
    }
    credits_render &= (y >= 0);

    if(!credits_render && !music_is_playing())
    {
        // fade out screen to black, then be done as well
        draw_quad(
            NULL,
            vector(0,0,0),
            NULL,
            screen_size,
            NULL,
            COLOR_BLACK,
            credits_fadeout,
            0);
        if(credits_fadeout >= 100)
            credits_done = true;
        credits_fadeout += credits_speedup * credits_scrollSpeed * time_step;
    }

    if(input_down(INPUT_JUMP))
    {
        credits_speedup = credits_speedup_factor;
        if ((total_ticks % 16) > 8)
        {
            draw_obj(credits_pnl_ff);
        }
    }
    else
    {
        credits_speedup = 1;
    }
    credits_timer += credits_speedup * credits_scrollSpeed * time_step;

    music_speedup(100 * credits_speedup);
}

var credits_isdone()
{
    return credits_done;
}
