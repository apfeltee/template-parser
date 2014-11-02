
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct TemplateParser;

/* callback type */
typedef void(*Callback)(struct TemplateParser* tpl, const char* chunk, size_t len, void*);

struct TemplateParser
{
    /* called on codeblocks, i.e., <% ... %> */
    Callback on_codeblock;

    /* called on codelines, i.e., <%= ... %> (note the '='!) */
    Callback on_codeline;

    /* called on everything else */
    Callback on_data;

    /* userdata pointer passed to Callback functions */
    void* userdata;

    /* the raw template, i.e., input data */
    const char* src;

    /* the buffer to which the transformed template is usually strcat'd to */
    char*  result;

    /* chunk for intermediate data */
    char* chunk;

    /* size of $src */
    size_t srclen;

    /* the size of memory available in our buffer $result */
    size_t bufsize;

    /* max size for intermediate chunks */
    size_t chunksize;

    /* */
    size_t chunkidx;

    /* the current line (for error handling) */
    size_t line;

    /* the current column (for error handling) */
    size_t column;

    /* the current index used from $src */
    size_t ci;
};

typedef struct TemplateParser TemplateParser;

/* initiate a TemplateParser instance. */
extern void tplparser_init_l(
    /* reference to a TemplateParser struct. see example.c! */
    TemplateParser* tpl,
    /* input string */
    const char* src,
    /* length of input string*/
    size_t srclen,
    /* buffer to write the template to */
    char* buffer,
    /* size of buffer */
    size_t bufsize,
    /* chunk for intermediate data */
    char* chunk,
    /* size of chunk */
    size_t chunksize
);

/* calls tplparser_init_l, using strlen() to provide $srclen */
extern void tplparser_init(
    TemplateParser* tpl,
    const char* src,
    char* buffer,
    size_t bufsize,
    char* chunk,
    size_t chunksize
);

/* de-initializes a TemplateParser instance */
extern void tplparser_fini(TemplateParser* tpl);

/* sets a userdata pointer for Callback functions */
extern void tplparser_set_userdata(TemplateParser* tpl, void* p);

/* set callback for codeblocks (i.e., <% if foo then bar end %>)*/
extern void tplparser_set_on_codeblock(TemplateParser* tpl, Callback fn);

/* set callback for codelines (i.e., <%= someVariable %>)*/
extern void tplparser_set_on_codeline(TemplateParser* tpl, Callback fn);

/* set callback for everything else */
extern void tplparser_set_on_data(TemplateParser* tpl, Callback fn);

/*
* perform the actual parsing
*
* NOTE: callbacks HAVE to be already defined at this point!
* even though TemplateParser provides "default" callbacks (mostly as an example),
* they're obviously being used here, and thus need to be defined prior to calling
* this function.
*/
extern void tplparser_parse(TemplateParser* tpl);

