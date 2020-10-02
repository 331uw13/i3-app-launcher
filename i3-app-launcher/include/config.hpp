#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <algorithm>
/*
 
 ( work in progress ...)
  	
*/


class Config {

private:
	
	const char 
			DELIMITER = ':',
			ARRAY_DELIMITER = ',',
			ARRAY_OPEN = '[',
			ARRAY_CLOSE = ']',
			PREFIX = '>';
	
	std::unordered_map<std::string, std::string> table;
	std::unordered_map<std::string, std::vector<std::string>> arrays;

	std::smatch get(std::string regexstr, std::string full) {
		std::regex r(regexstr);
		std::smatch m;
		std::regex_search(full, m, r);
		return m;
	}

	void add_item(std::string name, std::string value) {
		table[name] = value;
		std::cout << "Added: '" << name << "' <-- '" << value << "'\n";
	}


public:
	 struct color { 
		unsigned int r,g,b = 0;
	 	color(unsigned int R = 0,
			  unsigned int G = 0,
			  unsigned int B = 0
		) : r(R), g(G), b(B) {} 
	 };

	 void open_file(const char* file) {
		size_t len;	
		struct stat s;
		int f = open(file, O_RDONLY);
		if(f < 0) return;
		if(fstat(f, &s) < 0) return;
		len = s.st_size;
		void* address = mmap(NULL, len, PROT_READ, MAP_PRIVATE, f, 0);
		if(address == MAP_FAILED) return;
		std::string b = (const char*)address;
		
		std::string line;
		std::istringstream stream(b);
		while(getline(stream, line)) {
			if(line[0] == PREFIX) {
				size_t found = line.find(DELIMITER);
				if(found != std::string::npos) {
					int i = found + 1, length = line.length();
					if(length > i) {
						if(	line[i]			== ' ') i++;
						if(	line[found - 1]	== ' ') found--;

						std::string name = line.substr(0, found);
						std::string value = line.substr(i, length);
						add_item(name, value);
					}
				}
				else if(line.find(ARRAY_OPEN) != std::string::npos) {
					std::vector<std::string> full;
					std::string content;
					std::string line2, name = get(">(\\S*)", line).str(1);
					size_t array_start = b.find(">"+name);
					content = get(
									">"+name+" \\[([\\s\\S]*)\\]",
								   	b.substr(
											array_start,
										   	b.find(ARRAY_CLOSE, array_start)
									)).str(1);
					content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());	
					if(content.back() != ARRAY_DELIMITER) content += ARRAY_DELIMITER;
					size_t start = 0;
				   	size_t end = content.find_first_of(ARRAY_DELIMITER, start);
					while(end <= std::string::npos) {
						full.emplace_back(content.substr(start, end - start));
						start = end + 1;
						end = content.find_first_of(ARRAY_DELIMITER, start);
						if(end >= std::string::npos) break;
					}
					arrays[name] = full;
				}
			}
		}
		munmap(address, len);
	}

	template<typename T> T value(std::string name) { T ret;
		std::string value = table[(name.find(PREFIX) != std::string::npos) ? name : PREFIX + name];
		if(value != "") {
			std::istringstream iss(value);
    		value.find("0x") != std::string::npos ? iss >> std::hex >> ret : iss >> std::dec >> ret;
		}
		return ret;
	}

	color color_value(std::string name) {
		std::smatch m = get(std::string("\"\\s*(\\d{0,3}),\\s*(\\d{0,3}),\\s*(\\d{0,3})\\s*\""), value<std::string>(name));
		int rgb[4] = { 0, 0, 0 };
		if(m.str() != "") {
			for(int i = 1; i < 4; i++) {
				unsigned int __ = std::stoul(m.str(i));
				if(__ > 255) __ = 255;
				rgb[i - 1] = __;
			}
			return color(rgb[0], rgb[1], rgb[2]);
		}
		return color();
	}
	
	std::vector<std::string> array(std::string name) {
		return arrays[name];
	}

};
#endif











