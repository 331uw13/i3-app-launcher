#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <pwd.h>
#include <unistd.h>
#include <chrono>
#include "config.hpp"


void error(const std::string& str);
void addbox(const std::string& str, const sf::Vector2f& pos, const std::string& command);
[[nodiscard]] auto get_full(const std::string& file) -> std::string;
[[nodiscard]] auto sf_color(const Config::color& color) -> sf::Color;
void toggle_box();
void load_config();
auto match_key(const sf::Keyboard::Key& key, const sf::Keyboard::Key& key2) -> bool;


class Box {
public:
    sf::Text _text;
    sf::RectangleShape _rect;
    std::string _command;
    void focus();
    void unfocus();
    void move_up();
    void move_down();
};

static struct {
// Settings --------------------------------------
sf::Font font;
sf::Color box_focused{70, 10, 10};
sf::Color box_unfocused{20, 20, 20};
sf::Color text_focused{220, 220, 230};
sf::Color text_unfocused{80, 80, 80};
sf::Color background{10, 10, 10};
unsigned int WIDTH { 200 };
unsigned int HEIGHT { 400 };
unsigned int between { 2 };
sf::Vector2f from_00{5, 5};
unsigned int font_size { 16 };
unsigned int focused_box { 0 };
bool only_outline { false };
bool kill_if_no_focus { true };
sf::Vector2f box_size{static_cast<float>(WIDTH - between - 3), 20.0};
bool auto_window_height { false };
bool start_at_mouse_pos { true };
sf::Vector2i start_pos_offset{0, 0};

Config config{};
Box box;
std::vector<Box> boxes{};
std::vector<std::string> box_str_array{};

} globals;
// ------------------------------------------------

void Box::focus() {
    _text.setFillColor(globals.text_focused);
    globals.only_outline ? _rect.setOutlineColor(globals.box_focused) : _rect.setFillColor(globals.box_focused);
}
void Box::unfocus() {
    _text.setFillColor(globals.text_unfocused);
    globals.only_outline ? _rect.setOutlineColor(globals.box_unfocused) : _rect.setFillColor(globals.box_unfocused);
}
void Box::move_up() {
    _text.move(0, -(globals.box_size.y + globals.between));
    _rect.move(0, -(globals.box_size.y + globals.between));
}
void Box::move_down() {
    _text.move(0, globals.box_size.y + globals.between);
    _rect.move(0, globals.box_size.y + globals.between);
}


void error(const std::string& str) {
    std::cout << "\nError!\n\033[1;31m" << str << "\033[0m\n\n";
}

void addbox(const std::string &str, const sf::Vector2f &pos, const std::string &command) {
    std::cout << "addbox(): " << str << ", " << command << " to " << pos.x << "," << pos.y << '\n';
    globals.box._text.setString(str);
    globals.box._text.setPosition(sf::Vector2f(pos.x + 5, pos.y + globals.box._text.getGlobalBounds().height / 2 - 1));
    globals.box._rect.setPosition(pos);
    globals.box._command = command;
    globals.boxes.emplace_back(globals.box);
    std::cout << "size: " << globals.boxes.size() << '\n';
}

auto get_full(const std::string &file) -> std::string {
    std::string full = std::string(getpwuid(getuid())->pw_dir) + "/.config/i3-app-launcher/" + file;
    if(std::fstream(full).good()) {
        return full;
    } else return file;
}

void toggle_box() {
    static size_t p_rect_indx { 0 };

    for(size_t i = 0; i < globals.boxes.size(); i++) {
        float y = globals.boxes[globals.focused_box]._text.getPosition().y;
        if		(y < 0) 					globals.boxes[i].move_down();
        else if (y > globals.HEIGHT - globals.box_size.y) 	globals.boxes[i].move_up();
        globals.boxes[p_rect_indx].unfocus();
        globals.boxes[globals.focused_box].focus();
        p_rect_indx = i;
    }

    std::cout << "box " << globals.focused_box << " got focus | " << globals.boxes[globals.focused_box]._command << '\n';
}

auto sf_color(const Config::color &color) -> sf::Color {
    return sf::Color{static_cast<sf::Uint8>(color.r), static_cast<sf::Uint8>(color.g), static_cast<sf::Uint8>(color.b)};
}

void load_config() {
    globals.font_size = globals.config.value<unsigned int>("font_size");
    globals.WIDTH = globals.config.value<unsigned int>("width");
    globals.HEIGHT = globals.config.value<unsigned int>("height");
    globals.from_00.x = globals.config.color_value("offset_from00").r;
    globals.from_00.y = globals.config.color_value("offset_from00").g;
    globals.box_size.x = globals.config.value<int>("box_width");
    globals.box_size.y = globals.config.value<int>("box_height");
    globals.start_pos_offset.x = static_cast<int>(globals.config.color_value("start_pos_offset").r);
    globals.start_pos_offset.y = static_cast<int>(globals.config.color_value("start_pos_offset").g);
    globals.start_at_mouse_pos = globals.config.value<bool>("start_at_mouse_pos");
    globals.kill_if_no_focus = globals.config.value<bool>("kill_if_no_focus");
    globals.box_str_array = globals.config.array("programs");

    globals.box_focused = sf_color(globals.config.color_value("box_focused"));
    globals.box_unfocused = sf_color(globals.config.color_value("box_unfocused"));
    globals.text_focused = sf_color(globals.config.color_value("text_focused"));
    globals.text_unfocused = sf_color(globals.config.color_value("text_unfocused"));
    globals.background = sf_color(globals.config.color_value("background"));

    if(globals.HEIGHT <= 0) {
        globals.HEIGHT = static_cast<unsigned int>(globals.box_size.y + globals.from_00.y);
        globals.auto_window_height = true;
    }
    if(globals.box_size.x <= 0) {
        globals.box_size.x = globals.WIDTH - globals.from_00.x * 2;
    }

}

auto match_key(const sf::Keyboard::Key &key, const sf::Keyboard::Key &key2) -> bool
{
    return (key == key2);
}

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
