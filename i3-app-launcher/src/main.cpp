#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <pwd.h>
#include <unistd.h>
#include <chrono>
#include "config.hpp"

void error(std::string str) {
	std::cout << "\nError!\n\033[1;31m" << str << "\033[0m\n\n";
}

// Settings --------------------------------------
sf::Font font;
sf::Color box_focused(70, 10, 10);
sf::Color box_unfocused(20, 20, 20);
sf::Color text_focused(220, 220, 230);
sf::Color text_unfocused(80, 80, 80);
sf::Color background(10, 10, 10);
int WIDTH = 200, HEIGHT = 400;
int between = 2;
sf::Vector2f from_00(5, 5);
int font_size = 16;
unsigned int focused_box = 0;
bool only_outline = false;
bool kill_if_no_focus = true;
sf::Vector2f box_size(WIDTH - between - 3, 20);
bool auto_window_height = false;
bool start_at_mouse_pos = true;
sf::Vector2i start_pos_offset(0, 0);

Config config;
// ------------------------------------------------

class Box {
public:
	sf::Text _text;
	sf::RectangleShape _rect;
	std::string _command;
	void focus() {
		_text.setFillColor(text_focused);
		only_outline ? _rect.setOutlineColor(box_focused) : _rect.setFillColor(box_focused);		
	}
	void unfocus() {
		_text.setFillColor(text_unfocused);
		only_outline ? _rect.setOutlineColor(box_unfocused) : _rect.setFillColor(box_unfocused);
	}
	void move_up() {
		_text.move(0, -(box_size.y + between));
		_rect.move(0, -(box_size.y + between));
	}
	void move_down() {
		_text.move(0, box_size.y + between);
		_rect.move(0, box_size.y + between);
	}
};

Box box;
std::vector<Box> boxes;
std::vector<std::string> box_str_array;

void addbox(std::string str, sf::Vector2f pos, std::string command) {
	std::cout << "addbox(): " << str << ", " << command << " to " << pos.x << "," << pos.y << '\n';
	box._text.setString(str);
	box._text.setPosition(sf::Vector2f(pos.x + 5, pos.y + box._text.getGlobalBounds().height / 2 - 1));
	box._rect.setPosition(pos);
	box._command = command;
	boxes.emplace_back(box);
	std::cout << "size: " << boxes.size() << '\n';
}

std::string get_full(std::string file) {
	std::string full = std::string(getpwuid(getuid())->pw_dir) + "/.config/i3wm-taskbar/" + file;
	if(std::fstream(full).good()) {
		return full;
	} else return file;
}

unsigned int p_rect_indx = 0;
void toggle_box() {
	for(int i = 0; i < boxes.size(); i++) {
		float y = boxes[focused_box]._text.getPosition().y;
		if		(y < 0) 					boxes[i].move_down();
		else if (y > HEIGHT - box_size.y) 	boxes[i].move_up();
		boxes[p_rect_indx].unfocus();
		boxes[focused_box].focus();
		p_rect_indx = i;
	}

	std::cout << "box " << focused_box << " got focus | " << boxes[focused_box]._command << '\n'; 
}

sf::Color sf_color(Config::color color) {
	return sf::Color(color.r, color.g, color.b);
}

void load_config() {	
	font_size = config.value<int>("font_size");
	WIDTH = config.value<int>("width");
	HEIGHT = config.value<int>("height");
	from_00.x = config.color_value("offset_from00").r;
	from_00.y = config.color_value("offset_from00").g;	
	box_size.x = config.value<int>("box_width");
   	box_size.y = config.value<int>("box_height");	
	start_pos_offset.x = config.color_value("start_pos_offset").r;
	start_pos_offset.y = config.color_value("start_pos_offset").g;
	start_at_mouse_pos = config.value<bool>("start_at_mouse_pos");	
	kill_if_no_focus = config.value<bool>("kill_if_no_focus");
	box_str_array = config.array("programs");

	box_focused = sf_color(config.color_value("box_focused"));
	box_unfocused = sf_color(config.color_value("box_unfocused"));
	text_focused = sf_color(config.color_value("text_focused"));
	text_unfocused = sf_color(config.color_value("text_unfocused"));
	background = sf_color(config.color_value("background"));
	
	if(HEIGHT <= 0) {
		HEIGHT = box_size.y + from_00.y;
		auto_window_height = true;
	}
	if(box_size.x <= 0) {
		box_size.x = WIDTH - from_00.x * 2;
	}

}

bool match_key(sf::Keyboard::Key key, sf::Keyboard::Key key2) { return (key == key2); };

bool exit_when_done = false;
int main(int argc, char **argv)
{ 
	for(int i = 0; i < argc; i++) {
		std::cout << argv[i] << " ";
		if(strcmp(argv[i], "exit") == 0) {
			exit_when_done = true;
		}
	}
	std::cout << '\n';

	config.open_file(get_full("config").c_str());
	std::cout << "creating window: " << WIDTH << "x" << HEIGHT << '\n'; 
	load_config();

	box._text.setCharacterSize(font_size);
	box._text.setFont(font);
	box._text.setFillColor(text_unfocused);
	box._rect.setSize(box_size);
	if(only_outline) {
		box._rect.setOutlineThickness(2);
		box._rect.setOutlineColor(box_unfocused);
		box._rect.setFillColor(sf::Color::Transparent);
	} else {
		box._rect.setFillColor(box_unfocused);
	}
	
	sf::Vector2f auto_pos = from_00;
	for(int i = 0; i < box_str_array.size(); i++) {
		// name, pos, command
		size_t d = box_str_array[i].find(":");
		addbox(box_str_array[i].substr(0, d), auto_pos, box_str_array[i].substr(d + 1, box_str_array[i].length()));
		auto_pos.y += box_size.y + between;
	}
	//sf::Vector2f pos(from_00.x, from_00.y);
	// Moved --> load_config()
	/*for(int i = 0; i < 5; i++) {
		addbox("test" + std::to_string(i), pos, "");
		pos.y += box_size.y + between;
	}*/
	if(auto_window_height) HEIGHT = boxes[boxes.size() - 1]._rect.getPosition().y + box_size.y + 3;
	boxes[0].focus();

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "i3wm-taskbar", sf::Style::None);
	window.setFramerateLimit(30);
	window.setVerticalSyncEnabled(true);
	window.clear(sf::Color(110, 20, 20));
	window.setMouseCursorVisible(false);		
	
	if(start_at_mouse_pos) {	
		sf::Mouse mouse;
		sf::Vector2i targetpos = mouse.getPosition();
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		targetpos = mouse.getPosition();
		if(targetpos.y + HEIGHT > desktop.height) {
			targetpos.y -= (targetpos.y + HEIGHT + start_pos_offset.y) - desktop.height;
		}
		targetpos += start_pos_offset;
		window.setPosition(targetpos);
	} else {
		window.setPosition(start_pos_offset);	
	}
	
	if(!font.loadFromFile(get_full(config.value<std::string>("font")))) {
		error("please confirm that you have font in config file: " + get_full("config"));
		return EXIT_FAILURE;
	}

	while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
			if (kill_if_no_focus && event.type == sf::Event::LostFocus) {
					std::cout << "lost focus\n";
					std::cout << "quit!\n";
					window.close();
			}
            if (event.type == sf::Event::KeyPressed) {
				sf::Keyboard::Key key = event.key.code;
				if(match_key(key, sf::Keyboard::Q) || match_key(key, sf::Keyboard::Escape)) {
					std::cout << "quit!\n";
					window.close();
				}
				else if(match_key(key, sf::Keyboard::Up) || match_key(key, sf::Keyboard::W)) {
					if(focused_box > 0 && focused_box != 0) {
						focused_box--;
						toggle_box();
					}
				}
				else if(match_key(key, sf::Keyboard::Down) || match_key(key, sf::Keyboard::S)) {
					if(focused_box < boxes.size() - 1 && focused_box != boxes.size()) {
						focused_box++;
						toggle_box();
					}
				}
				else if(match_key(key, sf::Keyboard::Enter) || match_key(key, sf::Keyboard::D)) {
					std::cout << "index: " << focused_box << " | command: " << boxes[focused_box]._command << '\n';
					system(boxes[focused_box]._command.c_str());
					std::cout << "quit!\n"; 
					window.close();
				}
	    	}
		}	
        window.clear(background);
		if(boxes.size() > 0) {
			for(int i = 0; i < boxes.size(); i++) {
				window.draw(boxes[i]._rect);
				window.draw(boxes[i]._text);
			}
		}
		window.display();
    }

    return 0;
}
