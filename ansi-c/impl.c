
#include <stdlib.h>
#include "tpl.h"

#if _DEBUG
    #define compare(name, str, len) \
        { \
            size_t slen = strlen(str); \
            fprintf(stderr, "%s: len(%d) == strlen(%d) = %s\n", \
                name, len, slen, ((len == slen) ? "ok!" : "nope!") \
            ); \
        }
#else
    #define compare(name, str, len)
#endif

#define call_oncodeline(tpl, chunk, len) \
    call_cb(tpl, tpl->on_codeline, chunk, len)

#define call_oncodeblock(tpl, chunk, len) \
    call_cb(tpl, tpl->on_codeblock, chunk, len)

#define call_ondata(tpl, chunk, len) \
    call_cb(tpl, tpl->on_data, chunk, len)


static bool default_filter(TemplateParser* tpl, char* chunk, size_t len, void* p)
{
    (void)tpl;
    (void)chunk;
    (void)p;
    if(len > 0)
    {
        return true;
    }
    return false;
}

static bool default_on_codeblock(TemplateParser* tpl, char* chunk, size_t len, void* p)
{
    (void)len;
    (void)p;
    compare("on_codeblock", chunk, len);
    strcat(tpl->result, chunk);
    strcat(tpl->result, ";\n");
    return true;
}

static bool default_on_codeline(TemplateParser* tpl, char* chunk, size_t len, void* p)
{
    (void)len;
    (void)p;
    compare("on_codeline", chunk, len);
    strcat(tpl->result, "print(");
    strcat(tpl->result, chunk);
    strcat(tpl->result, ");\n");
    return true;
}

static bool default_on_data(TemplateParser* tpl, char* chunk, size_t len, void* p)
{
    (void)len;
    (void)p;
    compare("on_data", chunk, len);
    strcat(tpl->result, "print([[");
    strcat(tpl->result, chunk);
    strcat(tpl->result, "]]);\n");
    return true;
}

static void chappend(TemplateParser* tpl, char ch)
{
    tpl->chunk[tpl->chunkidx] = ch;
    tpl->chunkidx++;
}

static void chclear(TemplateParser* tpl)
{
    tpl->chunkidx = 0;
    memset(tpl->chunk, 0, tpl->chunksize);
}

static void whenlf(TemplateParser* tpl)
{
    if(tpl->src[tpl->ci] == '\n')
    {
        tpl->line++;
        tpl->column = 0;
    }
}

static void call_cb(TemplateParser* tpl, Callback which, char* chunk, size_t len)
{
    if(tpl->filterfunc(tpl, chunk, len, tpl->userdata) == true)
    {
        which(tpl, chunk, len, tpl->userdata);
    }
}



static void setup_vars(
    TemplateParser* tpl,
    const char* src, size_t srclen,
    char* buffer, size_t bufsize,
    char* chunk, size_t chsize)
{
    /* maybe str*dup() would be better here? */
    tpl->src = src;
    tpl->chunk = chunk;
    tpl->srclen = srclen;
    tpl->result = buffer;
    tpl->bufsize = bufsize;
    tpl->chunksize = chsize;
    tpl->chunkidx = 0;
    tpl->ci = 0;
    tpl->line = 1;
    tpl->column = 1;
    tplparser_set_filterfunc(tpl, default_filter);
    tplparser_set_on_codeblock(tpl, default_on_codeblock);
    tplparser_set_on_codeline(tpl, default_on_codeline);
    tplparser_set_on_data(tpl, default_on_data);
    tplparser_set_userdata(tpl, NULL);
    memset(tpl->result, 0, tpl->bufsize);
    memset(tpl->chunk, 0, tpl->chunksize);
}

void tplparser_init_l(
    TemplateParser* tpl,
    const char* src,
    size_t srclen,
    char* buffer,
    size_t bufsize,
    char* chunk,
    size_t chsize)
{
    setup_vars(tpl, src, srclen, buffer, bufsize, chunk, chsize);
    tpl->is_heapmem = false;
}

void tplparser_init(
    TemplateParser* tpl,
    const char* src,
    size_t srclen,
    size_t bufsize,
    size_t chunksize)
{
    char* buffer;
    char* chunk;
    buffer = (char*)malloc(bufsize + 1);
    chunk = (char*)malloc(chunksize + 1);
    setup_vars(tpl, src, srclen, buffer, bufsize, chunk, chunksize);
    tpl->is_heapmem = true;
}

void tplparser_fini(TemplateParser* tpl)
{
    if(tpl->is_heapmem == true)
    {
        free(tpl->result);
        free(tpl->chunk);
        tpl->result = NULL;
        tpl->chunk = NULL;
    }
}

void tplparser_set_filterfunc(TemplateParser* tpl, Callback fn)
{
    tpl->filterfunc = fn;
}

void tplparser_set_on_codeblock(TemplateParser* tpl, Callback fn)
{
    tpl->on_codeblock = fn;
}

void tplparser_set_on_codeline(TemplateParser* tpl, Callback fn)
{
    tpl->on_codeline = fn;
}

void tplparser_set_on_data(TemplateParser* tpl, Callback fn)
{
    tpl->on_data = fn;
}

void tplparser_set_userdata(TemplateParser* tpl, void* p)
{
    tpl->userdata = p;
}

void tplparser_parse(TemplateParser* tpl)
{
    bool in_code;
    bool was_eod;
    size_t len;
    const char* src;
    in_code = false;
    was_eod = false;
    len = 0;
    src = tpl->src;
    while((tpl->ci) < (tpl->srclen))
    {
        if((src[tpl->ci] == '<') && (src[tpl->ci + 1] == '%'))
        {
            in_code = true;
            tpl->ci += 2;
            while(1)
            {
                whenlf(tpl);
                if((tpl->ci >= tpl->srclen) || ((src[tpl->ci] == '%') && (src[tpl->ci + 1] == '>')))
                {
                    if((tpl->ci) < (tpl->srclen))
                    {
                        tpl->ci += 2;
                    }
                    else
                    {
                        was_eod = true;
                    }
                    in_code = false;
                    break;
                }
                chappend(tpl, src[tpl->ci]);
                len++;
                tpl->ci++;
                tpl->column++;
            }
            if(tpl->chunk[0] == '=')
            {
                call_oncodeline(tpl, tpl->chunk + 1, len - 1);
            }
            else if(tpl->chunk[0] == '#')
            {
                /* do nothing */
            }
            else
            {
                call_oncodeblock(tpl, tpl->chunk, len);
            }
            chclear(tpl);
            len = 0;
        }
        else
        {
            while((tpl->ci) < (tpl->srclen))
            {
                whenlf(tpl);
                if((src[tpl->ci] == '<') && (src[tpl->ci + 1] == '%'))
                {
                    break;
                }
                chappend(tpl, src[tpl->ci]);
                len++;
                tpl->ci++;
                tpl->column++;
            }
            call_ondata(tpl, tpl->chunk, len);
            chclear(tpl);
            len = 0;
        }
    }
}
