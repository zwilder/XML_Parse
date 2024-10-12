#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// In order to build need to use flag -D_SVID_SOURCE for gcc (strdup requires
// this)

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

/*****
 * Token
 *****/
Token *create_token_list(char *str, char delim);
void destroy_token_list(Token *head);

/*****
 * Functions
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
    /*
    if(node->text) {
        free(text);
        node->text = NULL;
    }
    */
    node->text = strdup(text);
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
        printf("  ");
    }

    //Print tag
    printf("%d - <%s>\n", lvl, node->tag);

    if(node->attributes) {
        print_XMLAttributes(node->attributes, lvl);
    }

    if(node->text) {
        for(i = 0; i < lvl + 1; i++) {
            printf("  ");
        }
        printf("Text: %s\n", node->text);
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

Token* create_token_blank(int strsize) {
    /* Create a node and allocate the memory for the string, but don't assign
     * anything to the string yet */
    Token *node = malloc(sizeof(Token));
    if(!node) return NULL;
    node->s = malloc(sizeof(char) * (strsize + 1));
    if(!node->s) {
        free(node);
        return NULL;
    }
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

Token *create_token_list(char *str, char delim) {
    /* Given a char* array string, split it by char delim and return a linked list of tokens */
    if(!str) {
        return NULL;
    }
    Token *result = NULL;
    Token *tmp = NULL;
    int i = 0;
    int letters = 0;

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
        if((str[i] == delim) && (str[i+1] != delim)) {
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
/*
void destroy_token_list(Token **head) {
    Token *tmp = *head;
    while(*head) {
        tmp = *head;
        *head = (*head)->next;
        free(tmp->s);
        free(tmp);
    }
    *head = NULL;
}
*/

void simple_parse_xml(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(!file) {
        printf("Error opening: %s.\n", filename);
        return;
    }
    
    char ch;
    bool in_tag = false;
    bool self_closing = false;
    char buffer[1024];
    int buffer_index = 0;
    //XMLNode *nodes = NULL;
    //XMLNode *tmp = NULL;
    XMLNode *new_node = NULL;
    XMLNode *current_node = NULL;
    XMLNode *root_node = NULL; // Function should return this (eventually)
    char *tag_name = NULL;
    char *key = NULL;
    char *value = NULL;
    Token *sp_tokens = NULL;
    Token *qt_tokens = NULL;
    Token *attr_pair = NULL;
    Token *value_token = NULL;
    
    while((ch = fgetc(file)) != EOF) {
        if(ch == '<') {
            // Entered tag
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
                sp_tokens = create_token_list(buffer, ' ');

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
                    qt_tokens = create_token_list(attr_pair->s, '=');
                    key = qt_tokens->s; // First part is key
                    if(qt_tokens->next) {
                        value = qt_tokens->next->s; // Second is value
                        destroy_token_list(value_token);
                        value_token = create_token_list(value, '\"'); // Remove quotations
                        if(value_token->next) {
                            add_attribute_XMLNode(new_node, key, value_token->next->s); // Only add attribute if both are present
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
        }
    }


    print_XMLNode(root_node, 0);

    // Cleanup 
    destroy_XMLNode(root_node); // Again, this function should (eventually) return root_node
    destroy_token_list(qt_tokens);
    destroy_token_list(sp_tokens);
    destroy_token_list(value_token);
    fclose(file);
}

/*****
 * Main
 *****/
int main(int argc, char **argv) {
    simple_parse_xml("MapA.tmx");
    return 0;
}
