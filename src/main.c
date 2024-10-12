/*
* XML Parse
* Copyright (C) Zach Wilder 2024
* 
* This file is a part of XML Parse
*
* XML Parse is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* XML Parse is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with XML Parse.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <xml_parse.h>

int main(int argc, char **argv) {
    int i = 0;
    for(i = 1; i < argc; i++) {
        printf("Loading %s...\n", argv[i]);
        simple_parse_xml(argv[i]);
    }
    return 0;
}
