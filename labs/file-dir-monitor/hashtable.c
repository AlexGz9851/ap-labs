#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#define TABLE_LEN 37591

struct item{
    struct item *next;
    long key;
    char* value;
};

static struct item *hashtable[TABLE_LEN];

unsigned hash(long key)
{
    return (key + 101) % TABLE_LEN;
}

char* getValue(int key)
{
    struct item *element;
    for (element = hashtable[hash(key)]; element != NULL; element = element->next) {
        if (key == element->key) {  return element->value; }
    }
    return NULL; 
}

struct item* getElement(int key)
{
    struct item *element;
    for (element = hashtable[hash(key)]; element != NULL; element = element->next) {
        if (key == element->key) { return element;  }
    }
    return NULL; 
}

void addElement(int key, char *value)
{ 
    struct item *element;
    unsigned hashval;
    if ((element = getElement(key)) == NULL) { 
        element = malloc(sizeof(*element));
        hashval = hash(key);
        element->next = hashtable[hashval];
        element->key = key;
        element->value = malloc(strlen(value)+1);
        strcpy(element->value, value);
        hashtable[hashval] = element;
    }else {
        free(element->value);
        element->value = malloc(strlen(value)+1);
        strcpy(element->value, value);
    }
}