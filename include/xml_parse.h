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
#ifndef XML_PARSE_H
#define XML_PARSE_H

/*****
 * System includes
 *****/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*****
 * Structs
 *****/
typedef struct XMLAttribute {
    char *key; // value on left of = sign, eg x in x="20"
    char *value;// value on right of = sign, eg 20 in x="20"
    struct XMLAttribute *next; // Linked list, for multiple attributes in tag
} XMLAttribute;

typedef struct XMLNode {
    char *tag; //First word after the <, eg tag in "<tag x=..."
    char *text; //Text between <tag> and </tag>
    XMLAttribute *attributes;
    struct XMLNode *parent; // Points to parent node
    struct XMLNode *children; // Points to first child node
    struct XMLNode *next; //Points to next sibling node
} XMLNode;

typedef struct Token {
    char *s;
    int length;
    struct Token *next;
} Token;

/*****
 * Main functions
 *****/
void simple_parse_xml(const char *filename);
XMLNode* parse_xml(const char *filename);

/*****
 * XMLAttribute
 *****/
XMLAttribute *create_XMLAttribute(char *key, char *value);
void destroy_XMLAttribute(XMLAttribute *attr);

/*****
 * XMLNode
 *****/
XMLNode *create_XMLNode(char *tag);
void destroy_XMLNode(XMLNode *node);
void add_attribute_XMLNode(XMLNode *node, char *key, char *value);
void add_text_XMLNode(XMLNode *node, char *text);
void add_child_XMLNode(XMLNode *parent, XMLNode *node);
void print_XMLAttributes(XMLAttribute *attr, int lvl);
void print_XMLNode(XMLNode *node, int lvl);

/*****
 * Token
 *****/
Token* create_token_blank(int strsize);
void destroy_token(Token *token);
void push_blank_token_list(Token **head, int sz);
Token *create_token_list(char *str, char delim, char excluder);
void destroy_token_list(Token *head);
void print_tokens(Token *tokens);

#endif //XML_PARSE_H
