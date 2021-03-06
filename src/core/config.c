/*
 * ConfigParser https://github.com/vChrysanthemum/ConfigParser
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/config.h"
#include "core/zmalloc.h"

static void skipWhitespaces(char **ptr)  {
    while((' ' == **ptr || '\t' == **ptr || '\n' == **ptr) && 0x00 != **ptr) *ptr+=1;
}

static int parseSection(char *ptr) {
    int n = 0;
    while (']' != *ptr && 0x00 != *ptr) {
        n++;
        ptr++;
    }
    return n;
}

static int parseOptionKeyNSkip(char **ptr) {
    int n=0, isEqualFound=0;
    while (' ' != **ptr && '\t' != **ptr && 0x00 != **ptr) {
        if ('=' == **ptr) {
            isEqualFound = 1;
            break;
        }

        n++;
        (*ptr)++;
    }

    skipWhitespaces(ptr);

    if(1 == isEqualFound) {
        (*ptr)++;
    }
    else {
        while ('=' != **ptr && 0x00 != **ptr) {
            (*ptr)++;
        }
        skipWhitespaces(ptr);
    }

    return n;
}

static int parseOptionValue(char *ptr) {
    int n=0, whitespaceCount=0;
    while (1) {
        whitespaceCount = 0;

        while(' ' == *ptr || '\t' == *ptr) {
            whitespaceCount += 1;
            ptr++;
        }

        if ('\n' == *ptr) {
            break;
        }

        n += whitespaceCount;

        ptr++;
        n++;
    }
    return n;
}

static int configGetReturnId(struct config *conf, char *section, char *option) {
    int j;
    struct configOption* opt;
    for (j = 0; j < conf->optionsCount; j++) {
        opt = conf->options[j];

        if (strlen(section) != opt->sectionLen) {
            continue;
        }

        if (0 == strncmp(opt->section, section, opt->sectionLen)) {
            if (0 == strncmp(opt->key, option, opt->keyLen)) {
                return j;
            }
        }
    }

    return -1;
}


struct config *initConfig() {
    struct config *conf;
    conf = zmalloc(sizeof(struct config));
    memset(conf, 0x00, sizeof(struct config));
    return conf;
}

void configRead(struct config *conf, char *path) {
    FILE* fp; 
    long len;
    char *content, *ptr;
    int currentContentId, sectionLen=0, id;
    char* section = NULL;
    char tmpSection[512] = "", tmpOption[512] = "";
    struct configOption *opt = NULL;

    fp = fopen(path, "r");
    if (fp == NULL) {
        return;
    }   

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (len <= 0) {
        return;
    }

    content = zmalloc(len + 1);
    memset(content, 0x00, len + 1);
    fread(content, len, 1, fp);

    conf->contentsCount += 1;
    conf->contents = (char **)realloc(conf->contents, sizeof(char **) * conf->contentsCount);
    currentContentId = conf->contentsCount - 1;
    conf->contents[currentContentId] = content;

    ptr = content;

    while (0x00 != *ptr) {
        opt = (struct configOption*)zmalloc(sizeof(struct configOption));

        skipWhitespaces(&ptr);

        if (0x00 == *ptr) {
            break;
        }

        if ('[' == *ptr) {
            ptr++;//skip [
            section = ptr;
            sectionLen = parseSection(ptr);
            ptr += sectionLen;
            ptr++;//skip ]
            skipWhitespaces(&ptr);
        }

        opt->section = section;
        opt->sectionLen = sectionLen;

        opt->key = ptr;
        opt->keyLen = parseOptionKeyNSkip(&ptr);

        opt->value = ptr;
        opt->valueLen = parseOptionValue(ptr);

        ptr += opt->valueLen;

        strncpy(tmpSection, section, sectionLen);
        tmpSection[sectionLen] = '\0';
        strncpy(tmpOption, opt->key, opt->keyLen);
        tmpOption[opt->keyLen] = '\0';

        id = configGetReturnId(conf, tmpSection, tmpOption);
        if (id < 0) {
            conf->optionsCount += 1;
            conf->options = (struct configOption**)realloc(conf->options, sizeof(struct configOption**) * conf->optionsCount);
            conf->options[conf->optionsCount-1] = opt;
        }
        else {
            free(conf->options[id]);
            conf->options[id] = opt;
        }

    }
}

struct configOption* configGet(struct config *conf, char *section, char *option) {
    int j = configGetReturnId(conf, section, option);
    if (-1 == j) {
        return NULL;
    }
    return conf->options[j];
}

void releaseConfig(struct config **conf) {
    int j;
    for (j = 0; j < (*conf)->contentsCount; j++) {
        free((*conf)->options[j]);
        free((*conf)->contents[j]);
    }
    free(*conf);
    *conf = NULL;
}
