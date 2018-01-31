#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "draw.h"
#include "stb_image.h"
#include "stb_truetype.h"
#include <glad/glad.h>

#define MAX_FONTS 64

typedef struct
{
	color_t color;
	blend_t blend;
	texture_t texture;
} state_t;

typedef struct fontdata_s
{
	texture_t texture;
	stbtt_bakedchar characters[96];
	int width;
	int height;
	struct fontdata_s* next;
} fontdata_t;

static state_t g_state;
static fontdata_t *g_fonts = NULL;

static fontdata_t* alloc_font()
{
	fontdata_t* alloc = (fontdata_t*) malloc( sizeof(fontdata_t) );
	memset(alloc, 0, sizeof(fontdata_t));
	if ( g_fonts != NULL )
	{
		alloc->next = g_fonts;
	}
	g_fonts = alloc;
	return alloc;
}

static void free_font(fontdata_t* font)
{
	if ( font == NULL ) return;

	fontdata_t* ptr = g_fonts;
	if ( ptr == font )
	{
		g_fonts = font->next;
		free(font);
		return;
	}

	while( ptr->next != font ) ptr = ptr->next;
	if ( ptr->next != font ) return;

	ptr->next = font->next;
	free(font);
}

texture_t _load_texture(const char* filename)
{
	int width;
	int height;
	int channels;

	void* data = stbi_load(filename, &width, &height, &channels, 4);

	if ( data != NULL )
	{
		printf("LOAD %s : %ix%i : %i\n", filename, width, height, channels);
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return texture;
}

font_t _load_font(const char* filename)
{
	FILE* fp;
	unsigned char *ttf_buffer;
	unsigned char bitmap[512*512];
	fontdata_t* data;

	fp = fopen(filename, "rb");
	if ( !fp ) 
	{
		printf("CAN'T FIND %s\n", filename);
		return NULL;
	}

	data = alloc_font();
	data->width = 512;
	data->height = 512;

	printf("LOAD %s\n", filename);

	ttf_buffer = (unsigned char*) malloc(0x100000);

	fread(ttf_buffer, 1, 1<<20, fp);
	stbtt_BakeFontBitmap(ttf_buffer, 0, 24.0, bitmap, data->width, data->height, 32, 96, data->characters);

	fclose(fp);
	free(ttf_buffer);

	glGenTextures(1, &data->texture);
	glBindTexture(GL_TEXTURE_2D, data->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, data->width, data->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return data;
}

void _free_texture(texture_t texture)
{
	glDeleteTextures(1, &texture);
}

void _free_font(font_t font) 
{
	free_font( (fontdata_t*) font );
}

void _set_blend(blend_t blend)
{
	if ( blend = g_state.blend ) return;
	g_state.blend = blend;

	switch( blend )
	{
		case BLEND_ALPHA:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
		case BLEND_ADD:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}
}

void _set_texture(texture_t texture) 
{
	if ( texture == g_state.texture ) return;
	g_state.texture = texture;

	glBindTexture(GL_TEXTURE_2D, texture);
}

void _set_color(color_t color) 
{
	if ( color == g_state.color ) return;
	g_state.color = color;

	glColor4ubv((GLubyte*)&g_state.color);
}

void _draw_rect(float x, float y, float width, float height) 
{
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(x,y);
	glTexCoord2f(1,0);
	glVertex2f(x+width,y);
	glTexCoord2f(1,1);
	glVertex2f(x+width,y+height);
	glTexCoord2f(0,1);
	glVertex2f(x,y+height);
	glEnd();
}

void _draw_sprite(float x, float y, float width, float height, float rotation) 
{
	float c = cosf(rotation);
	float s = sinf(rotation);
	float cw = c * width * .5f;
	float sw = s * width * .5f;
	float ch = c * height * .5f;
	float sh = s * height * .5f;

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(x - cw - sh, y + sw - ch);
	glTexCoord2f(1,0);
	glVertex2f(x + cw - sh, y - sw - ch);
	glTexCoord2f(1,1);
	glVertex2f(x + cw + sh, y - sw + ch);
	glTexCoord2f(0,1);
	glVertex2f(x - cw + sh, y + sw + ch);
	glEnd();
}

void _draw_polygon(vertex_t* vertices, int num_vertices) 
{
	int i;
	glBegin(GL_QUADS);
	for (i=0; i<num_vertices; ++i)
	{
		vertex_t* v = vertices + i;
		glTexCoord2f(v->u, v->v);
		glVertex2f(v->x, v->y);
	}
	glEnd();
}

void _draw_quad(vertex_t vertices[4]) 
{
	_draw_polygon( vertices, 4 );
}

void _draw_text(font_t font, float x, float y, const char* text) 
{
	fontdata_t* data = (fontdata_t*) font;
	if ( data == NULL ) return;

	glBindTexture(GL_TEXTURE_2D, data->texture);
	glBegin(GL_QUADS);
	while ( *text )
	{
		if ( *text >= 32 && *text < 128 )
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad( data->characters, data->width, data->height, *text-32, &x, &y, &q, 1 );
			glTexCoord2f(q.s0, q.t0);
			glVertex2f(q.x0, q.y0);
			glTexCoord2f(q.s1, q.t0);
			glVertex2f(q.x1, q.y0);
			glTexCoord2f(q.s1, q.t1);
			glVertex2f(q.x1, q.y1);
			glTexCoord2f(q.s0, q.t1);
			glVertex2f(q.x0, q.y1);
		}
		++text;
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, g_state.texture);
}

void init_gfx_lib(libgfx_t* gfx)
{
	gfx->load_texture = _load_texture;
	gfx->load_font = _load_font;
	gfx->free_texture = _free_texture;
	gfx->free_font = _free_font;

	gfx->set_blend = _set_blend;
	gfx->set_texture = _set_texture;
	gfx->set_color = _set_color;

	gfx->draw_rect = _draw_rect;
	gfx->draw_sprite = _draw_sprite;
	gfx->draw_quad = _draw_quad;
	gfx->draw_polygon = _draw_polygon;
	gfx->draw_text = _draw_text;

	g_state.color = 0xFFFFFFFF;
	g_state.blend = BLEND_ALPHA;
	g_state.texture = 0;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ubv((GLubyte*)&g_state.color);
}

void shutdown_gfx_lib()
{

}