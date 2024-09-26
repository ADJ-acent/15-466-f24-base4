#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include "load_save_png.hpp"

#include <iostream>
#include <iomanip>

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
//You probably shouldn't be looking here to learn to use either library.

int main(int argc, char **argv) {
	const char *fontfile;
	const char *text;

	if (argc < 3)
	{
		fprintf (stderr, "usage: freetype-test font-file.ttf text\n");
		exit (1);
	}

	fontfile = argv[1];
	text = argv[2];
	FT_Library ft_library;
	FT_Face ft_face;
	FT_Error ft_error;

	if ((ft_error = FT_Init_FreeType(&ft_library)))
    	abort();
	if ((ft_error = FT_New_Face(ft_library, fontfile, 0, &ft_face)))
		abort();
	if ((ft_error = FT_Set_Char_Size(ft_face, FONT_SIZE*64, FONT_SIZE*64, 0, 0)))
		abort();
	
	/* Create hb-ft font. */
	hb_font_t *hb_font;
	hb_font = hb_ft_font_create(ft_face, NULL);

	/* Create hb-buffer and populate. */
	hb_buffer_t *hb_buffer;
	hb_buffer = hb_buffer_create();
	hb_buffer_add_utf8(hb_buffer, text, -1, 0, -1);
	hb_buffer_guess_segment_properties(hb_buffer);

	/* Shape it! */
	hb_shape(hb_font, hb_buffer, NULL, 0);

	/* Get glyph information and positions out of the buffer. */
	unsigned int len = hb_buffer_get_length (hb_buffer);
	hb_glyph_info_t *info = hb_buffer_get_glyph_infos (hb_buffer, NULL);
	hb_glyph_position_t *pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);


	/* Print them out as is. */
	printf ("Raw buffer contents:\n");
	for (unsigned int i = 0; i < len; i++)
	{
		hb_codepoint_t gid   = info[i].codepoint;
		unsigned int cluster = info[i].cluster;
		double x_advance = pos[i].x_advance / 64.;
		double y_advance = pos[i].y_advance / 64.;
		double x_offset  = pos[i].x_offset / 64.;
		double y_offset  = pos[i].y_offset / 64.;

		char glyphname[32];
		hb_font_get_glyph_name(hb_font, gid, glyphname, sizeof (glyphname));

		printf ("glyph='%s'	cluster=%d	advance=(%g,%g)	offset=(%g,%g)\n",
				glyphname, cluster, x_advance, y_advance, x_offset, y_offset);
	}

		printf ("Converted to absolute positions:\n");
		/* And converted to absolute positions. */
	{
		double current_x = 0;
		double current_y = 0;
		for (unsigned int i = 0; i < len; i++)
		{
			hb_codepoint_t gid   = info[i].codepoint;
			unsigned int cluster = info[i].cluster;
			double x_position = current_x + pos[i].x_offset / 64.;
			double y_position = current_y + pos[i].y_offset / 64.;


			char glyphname[32];
			hb_font_get_glyph_name (hb_font, gid, glyphname, sizeof (glyphname));

			printf ("glyph='%s'	cluster=%d	position=(%g,%g)\n",
				glyphname, cluster, x_position, y_position);

			current_x += pos[i].x_advance / 64.;
			current_y += pos[i].y_advance / 64.;
		}
	}

	for (unsigned int i = 0; i < len; ++i) {
        FT_Load_Glyph(ft_face, info[i].codepoint, FT_LOAD_DEFAULT);
        FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
    }
	FT_Bitmap& bitmap = ft_face->glyph->bitmap;
	std::cout << "FT_Bitmap Information:" << std::endl;
    std::cout << "Rows         : " << bitmap.rows << std::endl;
    std::cout << "Width        : " << bitmap.width << std::endl;
    std::cout << "Pitch        : " << bitmap.pitch << std::endl;
    std::cout << "Num Grays    : " << bitmap.num_grays << std::endl;
    std::cout << "Pixel Mode   : " << static_cast<int>(bitmap.pixel_mode) << std::endl;
    std::cout << "Palette Mode : " << static_cast<int>(bitmap.palette_mode) << std::endl;
    std::cout << "Palette      : " << bitmap.palette << std::endl;

	std::vector<glm::u8vec4> image;
	image.reserve(bitmap.rows * bitmap.width);
    // Print the bitmap buffer content
    if (bitmap.buffer) {
        std::cout << "Bitmap Buffer Content:" << std::endl;

        // Loop through the rows
        for (unsigned int row = 0; row < bitmap.rows; ++row) {
            // Print each byte in the row
            for (unsigned int col = 0; col < bitmap.width; ++col) {
                // Compute the index in the buffer
                unsigned char pixel = bitmap.buffer[row * bitmap.pitch + col];
                
                // Print each pixel as a 2-digit hex value
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(pixel) << " ";
				image.emplace_back(pixel, pixel, pixel, 255);
            }
            std::cout << std::endl; // Newline at the end of each row
        }
    } else {
        std::cout << "Buffer is empty." << std::endl;
    }

	save_png("test.png", glm::uvec2(bitmap.width, bitmap.rows), image.data(), OriginLocation::UpperLeftOrigin);

	
	hb_buffer_destroy(hb_buffer);
	hb_font_destroy (hb_font);

	FT_Done_Face (ft_face);
	FT_Done_FreeType (ft_library);

	std::cout << "It worked?" << std::endl;
}
