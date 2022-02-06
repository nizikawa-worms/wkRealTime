#include "Utils.h"
#include <cstdio>
#include <fstream>
/*
 * Copyright 2018 Dominik Liebler

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
void Utils::hexDump(const char *desc, const void *addr, int len) {
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	// Output description if given.
	if (desc != NULL)
		printf ("%s:\n", desc);

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				printf("  %s\n", buff);

			// Output the offset.
			printf("  %04x ", i);
		}

		// Now the hex code for the specific character.
		printf(" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
			buff[i % 16] = '.';
		} else {
			buff[i % 16] = pc[i];
		}

		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf("  %s\n", buff);
}

void Utils::tokenize(std::string & str, const char* delim, std::vector<std::string> &out) {
	char *token = strtok(const_cast<char*>(str.c_str()), delim);
	while (token != nullptr) {
		auto temp = std::string(token);
		if(!temp.empty())
			out.emplace_back(temp);
		token = strtok(nullptr, delim);
	}
}

void Utils::tokenizeSet(std::string & str, const char* delim, std::set<std::string> &out) {
	char *token = strtok(const_cast<char*>(str.c_str()), delim);
	while (token != nullptr) {
		auto temp = std::string(token);
		if(!temp.empty())
			out.insert(temp);
		token = strtok(nullptr, delim);
	}
}

std::optional<std::string> Utils::readFile(std::filesystem::path path) {
	std::ifstream in(path, std::ios::binary);
	if (in.good()) {
		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();
		return {buffer.str()};
	}
	return {};
}

void Utils::stripNonAlphaNum(std::string & s) {
	s.erase(std::remove_if(s.begin(), s.end(), []( auto const& c ) -> bool { return !std::isalnum(c); } ), s.end());
}


double Utils::fixedToDouble(int fixed) {
	return (fixed >> 16) + (double)(fixed & 0xFFFF)/65535.0;
}
