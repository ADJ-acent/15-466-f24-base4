#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <array>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *hip = nullptr;
	Scene::Transform *upper_leg = nullptr;
	Scene::Transform *lower_leg = nullptr;
	glm::quat hip_base_rotation;
	glm::quat upper_leg_base_rotation;
	glm::quat lower_leg_base_rotation;
	float wobble = 0.0f;

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;

	struct Room {
		std::string title;
		std::array<std::string, 4> description;
		std::array<int32_t, 4> connecting_rooms = {-1,-1,-1,-1};
		Room(std::string title_, std::array<std::string, 4> description_) : title(title_), description(description_){};
	};
	std::string cur_key = "";
	std::vector<Room> rooms;

	const std::array<std::string, 4> directions = {"up", "left", "down", "right"};
	const std::string direction_symbols = "^<v>";
	const std::string correct_key ="^^vv<><>";

	uint32_t cur_room_index = 0;
	int32_t infinite_maze_begin_index = 0;
	int32_t infinite_maze_middle_index = 0;
	int32_t infinite_maze_inspect_index = 0;
	int32_t freedom_index = 0;

	bool warning = false;
	float warning_timer = 0.0f;
	const float warning_wear_off = 1.0f;
	int warning_dir = 0;
	float since_input = 0.0f;
	const float input_cool_down = 0.3f;
	
	//camera:
	Scene::Camera *camera = nullptr;

	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color, glm::uvec2 const &drawable_size);

	void restart();

};
