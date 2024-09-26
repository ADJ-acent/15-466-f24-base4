#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"
#include "Font.hpp"
#include "FontRenderProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <random>

extern Load< FontRenderProgram > font_render_program;
extern Load< Font > font;


PlayMode::PlayMode() {
	// make the rooms
	Room start = Room("The Beginning...", {
		"You (hamster) find yourself waking up from a comma lying on a crunchy orange floor.", 
		"The floor exudes a delicious aroma. But, you cannot remember how you got there...", "",
		"[(up) Get up]        [(left) Take a bite of the floor]        [(down) Inspect Self]        [(right) Take a bite of the floor (from the right)]"
		}
	);
	int32_t start_index = int32_t(rooms.size());
	rooms.push_back(start);

	Room self_inspect = Room("Self Inspection", {
		"You are a chubby hamster, maybe a bit too chubby.", 
		"Did you eat too many carrots for lunch? Speaking of lunch...the floor does look appetizing...", "",
		"[(up) Get up]        [(left) Take a bite of the floor]        [(down) Take a bite...]        [(right) Take a bite...You really want to]"
		}
	);
	int32_t self_inspect_index = int32_t(rooms.size());
	rooms.push_back(self_inspect);

	Room maze_start = Room("A maze?", {
		"You see giant orange carrots circles around you, acting like walls that cages you in.", 
		"A small opening is visible on one of the walls, but there are more carrot walls beyond it", "",
		"[(up) Go to the opening]        [(left) Take a bite of the wall]        [(down) Sit back down]        [(right) Take a bite of the wall (on the right)]"
		}
	);
	int32_t maze_start_index = int32_t(rooms.size());
	rooms.push_back(maze_start);

	Room poisoned = Room("Dead Hamster", {
		"World swirls around you...Your gluttony seemed to have finally caught up to you", 
		"A small opening is visible on one of the walls, but there are more carrot walls beyond it", "",
		"No more actions... You are dead. Press R to restart."
		}
	);
	
	int32_t poinsoned_index = int32_t(rooms.size());
	rooms.push_back(poisoned);

	Room sitting = Room("Back on the floor", {
		"The crunchy carrot floor sure does provide a good sitting platform.", 
		"You wonder how the floor might taste, but you should probably get going again soon.", "",
		"[(up) Get up]        [(left) Take a bite of the floor]        [(down) Take a bite...]        [(right) Take a bite...You really want to]"
		}
	);
	int32_t sitting_index = int32_t(rooms.size());
	rooms.push_back(sitting);

	Room infinite_maze_begin = Room("Trapped?", {
		"As you stepped into this opening, the maze swirls into action and four more openings open around you.", 
		"'Have I been here before?' You wonder to yourself.",
		"On the healthy looking walls are some gibberish that you don't understand.", 
		"[(up) Go forward]        [(left) Go left]        [(down) Inspect the gibberish]        [(right) Go right]"
		}
	);
	infinite_maze_begin_index = int32_t(rooms.size());
	rooms.push_back(infinite_maze_begin);

	Room infinite_maze_inspect = Room("Gibberish", {
		"You take a step closer and stare at the writting. You correctly realize that you did not learn this at Harevard University", 
		"'Still might come in handy to some higher dimensional observer though!'",
		"you try to remember the shape of the symbols '^^vv<><>'", 
		"[(up) Go forward]        [(left) Go left]        [(down) Go down]        [(right) Go right]"
		}
	);
	infinite_maze_inspect_index = int32_t(rooms.size());
	rooms.push_back(infinite_maze_inspect);

	Room infinite_maze_middle = Room("Loop?", {
		"As you stepped into this opening, the maze swirls into action and four more openings open around you.", 
		"'Have I been here before?' You wonder to yourself.",
		"...Something seems to be missing from the room", 
		"[(up) Go forward]        [(left) Go left]        [(down) Go down]        [(right) Go right]"
		}
	);
	infinite_maze_middle_index = int32_t(rooms.size());
	rooms.push_back(infinite_maze_middle);
	Room freedom = Room("Freedom!", {
		"Sunlight! You have successfully freed yourself from the maze! You feel exhausted but relieved.", 
		"You raise your head and see 20 carrot police staring you down...",
		"You have a bad feeling about this. (To be Continued)", 
		"Thank you for playing! Press R to play again."
		}
	);
	freedom_index = int32_t(rooms.size());
	rooms.push_back(freedom);


	rooms[start_index].connecting_rooms = {maze_start_index, poinsoned_index, self_inspect_index, poinsoned_index};
	rooms[self_inspect_index].connecting_rooms = {maze_start_index, poinsoned_index, poinsoned_index, poinsoned_index};
	rooms[maze_start_index].connecting_rooms = {infinite_maze_begin_index, poinsoned_index, sitting_index, poinsoned_index};
	rooms[sitting_index].connecting_rooms = {maze_start_index, poinsoned_index, poinsoned_index, poinsoned_index};
	rooms[infinite_maze_begin_index].connecting_rooms = {infinite_maze_begin_index, infinite_maze_begin_index, infinite_maze_inspect_index, infinite_maze_begin_index};
	rooms[infinite_maze_middle_index].connecting_rooms = {infinite_maze_begin_index, infinite_maze_begin_index, infinite_maze_begin_index, infinite_maze_begin_index};
	rooms[infinite_maze_inspect_index].connecting_rooms = {infinite_maze_begin_index, infinite_maze_begin_index, infinite_maze_begin_index, infinite_maze_begin_index};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_r) {
			restart();
			return true;
		} 
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	if (warning) {
		warning_timer += elapsed;
		if (warning_timer > warning_wear_off) {
			warning = false;
			warning_timer = 0.0f;
		}
	}
	if (since_input != 0.0f) {
		since_input += elapsed;
		if (since_input > input_cool_down) since_input = 0.0f;
	}

	int direction = 0;
	bool direction_pressed = true;
	if (since_input == 0.0f){
		if (up.pressed) {
			direction = 0;
		}
		else if (left.pressed) {
			direction = 1;
		}	
		else if (down.pressed) {
			direction = 2;
		}
		else if (right.pressed) {
			direction = 3;
		}
		else {
			direction_pressed = false;
		}
		if (direction_pressed) {
			if (rooms[cur_room_index].connecting_rooms[direction] != -1) {
				if ((int32_t(cur_room_index) == infinite_maze_begin_index && direction != 2) ||
					int32_t(cur_room_index) == infinite_maze_middle_index ||
					int32_t(cur_room_index) == infinite_maze_inspect_index 
				) {
					cur_key += direction_symbols[direction];
					if (cur_key == correct_key) {
						cur_room_index = freedom_index;
					}
					else {
						bool correct_so_far = true;
						for (uint32_t i = 0; i < uint32_t(cur_key.size()); ++i) {
							if (cur_key[i] != correct_key[i]) {
								correct_so_far = false;
								break;
							}
						}
						if (correct_so_far) {
							cur_room_index = infinite_maze_middle_index;
						}
						else {
							cur_room_index = infinite_maze_begin_index;
							cur_key = "";
						}
					}

				}
				else {
					cur_room_index = rooms[cur_room_index].connecting_rooms[direction];
				}
				since_input = 0.001f;
			}
			else {
				warning_dir = direction;
				warning_timer = 0.0f;
				warning = true;
			}
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	// //update camera aspect ratio for drawable:
	// camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	// //set up light type and position for lit_color_texture_program:
	// // TODO: consider using the Light(s) in the scene to do this
	// glUseProgram(lit_color_texture_program->program);
	// glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	// glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	// glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	// glUseProgram(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	// scene.draw(*camera);

	{// draw text
		if (warning) {
			RenderText("No '" + directions[warning_dir] + "' choice for the current room!", 440.0f, 100.0f, .5f, glm::vec3(1.0f * (warning_wear_off - warning_timer), 0.0f, 0.0f), drawable_size);
		}
		RenderText(rooms[cur_room_index].title, 25.0f, 650.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), drawable_size);
		RenderText(rooms[cur_room_index].description[0], 50.0f, 500.0f, .5f, glm::vec3(0.8f, 0.8f, 0.8f), drawable_size);
		RenderText(rooms[cur_room_index].description[1], 50.0f, 430.0f, .5f, glm::vec3(0.8f, 0.8f, 0.8f), drawable_size);
		RenderText(rooms[cur_room_index].description[2], 50.0f, 360.0f, .5f, glm::vec3(0.8f, 0.8f, 0.8f), drawable_size);
		RenderText(rooms[cur_room_index].description[3], 50.0f, 200.0f, .4f, glm::vec3(0.8f, 0.8f, 0.8f), drawable_size);
		RenderText("Use arrow keys to select your choices", 25.0f, 25.0f, .3f, glm::vec3(.5f, .5f, .5f), drawable_size);
		GL_ERRORS();
	}

	// { //use DrawLines to overlay some text:
	// 	glDisable(GL_DEPTH_TEST);
	// 	float aspect = float(drawable_size.x) / float(drawable_size.y);
	// 	DrawLines lines(glm::mat4(
	// 		1.0f / aspect, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f
	// 	));

	// 	constexpr float H = 0.09f;
	// 	lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
	// 		glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
	// 		glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
	// 		glm::u8vec4(0x00, 0x00, 0x00, 0x00));
	// 	float ofs = 2.0f / drawable_size.y;
	// 	lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
	// 		glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
	// 		glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
	// 		glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	// }
	GL_ERRORS();
}

// glm::vec3 PlayMode::get_leg_tip_position() {
// 	//the vertex position here was read from the model in blender:
// 	return lower_leg->make_local_to_world() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
// }
// Text rendering code adapted from https://learnopengl.com/In-Practice/Text-Rendering
void PlayMode::RenderText(std::string text, float x, float y, float scale, glm::vec3 color, glm::uvec2 const &drawable_size)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	GL_ERRORS();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    // activate corresponding render state	
	GL_ERRORS();
	glUseProgram(font_render_program->program);
	float aspect = float(drawable_size.y) / float(drawable_size.x);
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 1280.0f * aspect);
	glUniformMatrix4fv(font_render_program->PROJECTION_mat4,  1, GL_FALSE, glm::value_ptr(projection));

    glUniform3f(font_render_program->TexColor_vec3, color.x, color.y, color.z);
	GL_ERRORS();
    glActiveTexture(GL_TEXTURE0);
	GL_ERRORS();
    glBindVertexArray(VAO);
	GL_ERRORS();
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
		char character = *c;
        Font::Character ch = font->characters.find(character)->second;

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	glUseProgram(0);

}

void PlayMode::restart()
{
	cur_room_index = 0;
	warning_timer = 0.0f;
	since_input = 0.0f;
}
