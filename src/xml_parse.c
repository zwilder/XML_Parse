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

/*
 * TODO: XMLAttribute should be a hash table to look up key/value pairs (might
 * be overkill, how many attributes can a tag REALLY be expected to have?
 */

void simple_parse_xml(const char *filename) {
    XMLNode *root_node = parse_xml(filename);

    if(root_node) {
        print_XMLNode(root_node, 0);
    }

    destroy_XMLNode(root_node);
}

XMLNode* parse_xml(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(!file) {
        printf("Error opening: %s.\n", filename);
        return NULL;
    }
    // This makes sure the text buffer is big enough
    fseek(file, 0L, SEEK_END);
    long res = ftell(file);
    fseek(file, 0L, SEEK_SET);
    
    char ch;
    char buffer[1024];
    char *text_buffer = malloc(res);
    char *tag_name = NULL;
    char *key = NULL;
    char *value = NULL;
    bool in_tag = false;
    bool self_closing = false;
    int text_buffer_index = 0;
    int buffer_index = 0;
    XMLNode *new_node = NULL;
    XMLNode *current_node = NULL;
    XMLNode *root_node = NULL; // Function returns this
    Token *sp_tokens = NULL;
    Token *qt_tokens = NULL;
    Token *attr_pair = NULL;
    Token *value_token = NULL;
    
    while((ch = fgetc(file)) != EOF) {
        if(ch == '<') {
            // Entered tag
            text_buffer[text_buffer_index] = '\0';
            text_buffer_index = 0;
            if(current_node) {
                add_text_XMLNode(current_node, text_buffer);
            }
            in_tag = true;
            buffer_index = 0;
            buffer[buffer_index] = ch;
        } else if (ch == '>') {
            // Exited tag
            in_tag = false;
            buffer[buffer_index] = '\0'; // Null terminate buffer str

            if(buffer[0] == '/') {
                // Handle closing tag, move up tree
                if(current_node) {
                    current_node = current_node->parent;
                }
            } else {
                // Cleanup old tokens
                destroy_token_list(sp_tokens); // Clear the token list

                // Handle opening tag or self closing tag
                self_closing = (buffer[strlen(buffer) - 1] == '/');


                // Split the buffer by spaces into sp_tokens
                sp_tokens = create_token_list(buffer, ' ', '\"');

                // Get tag name (first part of buffer)
                tag_name = sp_tokens->s; // first token in tag is the tag name
                new_node = create_XMLNode(tag_name);

                if(current_node) {
                    new_node->parent = current_node;
                    add_child_XMLNode(current_node, new_node);
                } else {
                    root_node = new_node;
                }
                if(!self_closing) {
                    current_node = new_node;
                }

                // Handle attributes
                attr_pair = sp_tokens->next; // second token in sp_tokens starts attribute pairs
                while(attr_pair) {
                    destroy_token_list(qt_tokens);
                    // Split the pair by the equls sign
                    qt_tokens = create_token_list(attr_pair->s, '=', '\0');
                    if(qt_tokens) {
                        key = qt_tokens->s; // First part is key
                        if(qt_tokens->next) {
                            value = qt_tokens->next->s; // Second is value
                            destroy_token_list(value_token);
                            value_token = create_token_list(value, '\"', '\0'); // Remove quotations
                            if(value_token->next) {
                                add_attribute_XMLNode(new_node, key, value_token->next->s); // Only add attribute if both are present
                            }
                        }
                    }
                    attr_pair = attr_pair->next;
                }
            }
        } else if (in_tag) {
            buffer[buffer_index] = ch;
            buffer_index += 1;
        } else {
            // Outside of tag, this is text between tags
            if(text_buffer_index < (res - 1)) {
                text_buffer[text_buffer_index] = ch;
                text_buffer_index += 1;
            } else {
                printf("Text buffer overflow! Buffer size %ld\n",res);
            }
        }
    }



    // Cleanup 
    destroy_token_list(qt_tokens);
    destroy_token_list(sp_tokens);
    destroy_token_list(value_token);
    if(text_buffer) {
        free(text_buffer);
    }
    fclose(file);

    return root_node;
}

/*****
 * XMLAttribute Functions
 *****/
XMLAttribute *create_XMLAttribute(char *key, char *value) {
    XMLAttribute *attr = malloc(sizeof(XMLAttribute));
    if(!attr) {
        return NULL;
    }
    attr->key = strdup(key);
    attr->value = strdup(value);
    attr->next = NULL;
    return attr;
}

void destroy_XMLAttribute(XMLAttribute *attr) {
    if(!attr) return;
    if(attr->key) {
        free(attr->key);
    }
    if(attr->value) {
        free(attr->value);
    }
    free(attr);
    attr = NULL;
}

/*****
 * XMLNode Functions
 *****/
XMLNode *create_XMLNode(char *tag) {
    XMLNode *node = malloc(sizeof(XMLNode));
    if(!node) {
        return NULL;
    }
    node->tag = strdup(tag);
    node->text = NULL;
    node->attributes = NULL;
    node->parent = NULL;
    node->children = NULL;
    node->next = NULL;

    return node;
}

void destroy_XMLNode(XMLNode *node) {
    if(!node) return;
    XMLAttribute *attr = node->attributes;
    XMLAttribute *tmp_attr = NULL;
    XMLNode *child = node->children;
    XMLNode *tmp_node = NULL;

    if(node->tag) {
        free(node->tag);
        node->tag = NULL;
    }
    if(node->text) {
        free(node->text);
        node->text = NULL;
    }
    
    while(attr) {
        tmp_attr = attr;
        attr = attr->next;
        destroy_XMLAttribute(tmp_attr);
    }

    // Destroy node children
    while(child) {
        tmp_node = child;
        child = child->next;
        destroy_XMLNode(tmp_node);
    }

    free(node);
    node = NULL;
}

void add_attribute_XMLNode(XMLNode *node, char *key, char *value) {
    if(!node || !key || !value) return;
    XMLAttribute *attr = NULL;
    XMLAttribute *attr_new = create_XMLAttribute(key,value);
    if(!attr_new) return;

    // Add the new attribute to the end of the node's attribute list
    attr = node->attributes;
    if(!attr){
        node->attributes = attr_new;
    } else {
        // Iterate through to last node
        while(attr->next) {
            attr = attr->next; 
        }
        attr->next = attr_new;
    }
}

void add_text_XMLNode(XMLNode *node, char *text) {
    if(!node || !text) return;
    bool isempty = true;
    int i = 0;
    for(i = 0; text[i] != '\0'; i++) {
        if((text[i] != ' ') && (text[i] != '\t') && (text[i] != '\n')) {
            isempty = false;
        }
    }
    if(!isempty) {
        node->text = strdup(text);
    }
}

void add_child_XMLNode(XMLNode *parent, XMLNode *node) {
    if(!parent || !node) return;
    XMLNode *children = parent->children;
    if(children) {
        while(children->next) {
            children = children->next;
        }
        children->next = node;
    } else {
        parent->children = node;
    }
}

void print_XMLAttributes(XMLAttribute *attr, int lvl) {
    /*
    if(!attr) return;
    int i = 0;
    for(i = 0; i < lvl; i++) {
        printf("  ");
    }
    printf(" :%s = %s\n", attr->key, attr->value);
    print_XMLAttributes(attr->next, lvl);
    */
    int i = 0;
    while(attr) {
        for(i = 0; i < lvl; i++) {
           printf("  ");
        }
        printf("    :%s = %s\n", attr->key, attr->value);
        attr = attr->next;
    }
}

void print_XMLNode(XMLNode *node, int lvl) {
    if(!node) return;
    XMLNode *child = node->children;
    int i = 0;
    for(i = 0; i < lvl; i++) {
        printf("--");
    }

    //Print tag
    printf("%d - %s\n", lvl, node->tag);

    if(node->attributes) {
        print_XMLAttributes(node->attributes, lvl);
    }

    if(node->text) {
        for(i = 0; i < lvl + 1; i++) {
            printf("  ");
        }
        printf("\"\"\" %s \"\"\"\n", node->text);
    }
    
    //Print children
    while(child) {
        print_XMLNode(child, lvl + 1);
        child = child->next;
    }

    //Print closing tag - disabled for clarity
    /*
    for(i = 0; i < lvl; i++) {
        printf("  ");
    }
    printf("</%s>\n", node->tag);
    */
}

/*****
 * Token functions
 *****/
Token* create_token_blank(int strsize) {
    /* Create a node and allocate the memory for the string, but don't assign
     * anything to the string yet */
    Token *node = malloc(sizeof(Token));
    if(!node) return NULL;
    //TODO: This is silly. Allocating twice the amount for the string fixes
    //read/write memory errors in Valgrind.
    node->s = malloc(2 * sizeof(char) * (strsize + 1));
    if(node->s == NULL) {
        free(node);
        return NULL;
    }
    node->s[0] = '\0';
    node->length = strsize;
    node->next = NULL;
    return node;
}

void destroy_token(Token *token) {
    if(!token) return;
    if(token->s) {
        free(token->s);
    }
    free(token);
}

void push_blank_token_list(Token **head, int sz) {
    Token *newNode = create_token_blank(sz);
    Token *tmp;
    if(!(*head)) {
        *head = newNode;
        return;
    }
    if(!(*head)->next) {
        (*head)->next = newNode;
        return;
    }
    tmp = *head;
    while(tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = newNode;
}

Token *create_token_list(char *str, char delim, char excluder) {
    /* Given a char* array string, split it by char delim and return a linked list of tokens.
     * Beautifully ignores any delimiters contained between two "excluder"
     * chars. If you passed in, for example, the string "Really cool fish fry",
     * with a delimiter of ' ' and excluder of 'f' this would return a list
     * containing:
     * - Really
     * - cool
     * - fish fry (space is ignored between the two 'f's)
     *
     * Pass in '\0' for the excluder if not needed. 
     */
    if(!str) {
        return NULL;
    }
    Token *result = NULL;
    Token *tmp = NULL;
    int i = 0;
    int letters = 0;
    bool ignore = false;

    for(i = 0; str[i] != '\0'; i++) {
        /* move through string, looking at the letters */
        if((str[i] == delim) && (str[i+1] != delim)) {
            push_blank_token_list(&result, letters);
            letters = 0;
        } else if (str[i+1] == '\0') {
            letters++;
            push_blank_token_list(&result, letters);
        } else {
            letters++;
        }
    }

    tmp = result;
    letters = 0;
    for(i = 0; str[i] != '\0'; i++) {
        if(!tmp) {
            break;
        }
        if((str[i] == excluder) && !ignore) {
            // Start ignoring delimiters
            ignore = true;
        }else if((str[i] == excluder) && ignore) {
            // Stop ignoring delimiters
            ignore = false;
        }
        if((str[i] == delim) && (str[i+1] != delim) && !ignore) {
            tmp->s[letters] = '\0';
            tmp = tmp->next;
            letters = 0;
        }  else {
            tmp->s[letters] = str[i];
            letters++;
        }
    }

    if(tmp) {
        tmp->s[letters] = '\0';
    }
    return result;
}

void destroy_token_list(Token *head) {
    if(!head) return;
    Token *tmp = head;
    while(head) {
        tmp = head;
        head = head->next;
        destroy_token(tmp);
    }
}

void print_tokens(Token *tokens) {
    if(!tokens) return;
    while(tokens) {
        printf("%s\n", tokens->s);
        tokens = tokens->next;
    }
}
