
#include <stdlib.h>
#include "tpl.h"

static void tplparser_default_on_codeblock(TemplateParser* tpl, const char* chunk, size_t len, void* p)
{
    (void)len;
    (void)p;
    strcat(tpl->result, chunk);
    strcat(tpl->result, ";\n");
}

static void tplparser_default_on_codeline(TemplateParser* tpl, const char* chunk, size_t len, void* p)
{
    (void)len;
    (void)p;
    strcat(tpl->result, "print(");
    strcat(tpl->result, chunk);
    strcat(tpl->result, ");\n");
}

static void tplparser_default_on_data(TemplateParser* tpl, const char* chunk, size_t len, void* p)
{
    (void)len;
    (void)p;
    strcat(tpl->result, "print([[");
    strcat(tpl->result, chunk);
    strcat(tpl->result, "]]);\n");
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

void tplparser_init_l(
    TemplateParser* tpl,
    const char* src,
    size_t srclen,
    char* buffer,
    size_t bufsize,
    char* chunk,
    size_t chsize
)
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
    tplparser_set_on_codeblock(tpl, tplparser_default_on_codeblock);
    tplparser_set_on_codeline(tpl, tplparser_default_on_codeline);
    tplparser_set_on_data(tpl, tplparser_default_on_data);
    tplparser_set_userdata(tpl, NULL);
    memset(tpl->result, 0, tpl->bufsize);
}

void tplparser_init(
    TemplateParser* tpl,
    const char* src,
    char* buffer,
    size_t bufsize,
    char* chunk,
    size_t chunksize
)
{
    tplparser_init_l(tpl, src, strlen(src), buffer, bufsize, chunk, chunksize);
}

void tplparser_fini(TemplateParser* tpl)
{
    (void)tpl;
    /* nothing to do for now */
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
    const char* src;
    src = tpl->src;
    while((tpl->ci) < (tpl->srclen))
    {
        if((src[tpl->ci] == '<') && (src[tpl->ci + 1] == '%'))
        {
            tpl->ci += 2;
            while(1)
            {
                whenlf(tpl);
                if((src[tpl->ci] == '%') && (src[tpl->ci + 1] == '>'))
                {
                    tpl->ci += 2;
                    break;
                }
                chappend(tpl, src[tpl->ci]);
                tpl->ci++;
                tpl->column++;
            }
            if(tpl->chunk[0] == '=')
            {
                tpl->on_codeline(tpl, tpl->chunk + 1, 0);
            }
            else
            {
                tpl->on_codeblock(tpl, tpl->chunk, 0);
            }
            chclear(tpl);
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
                tpl->ci++;
                tpl->column++;
            }
            tpl->on_data(tpl, tpl->chunk, 0);
            chclear(tpl);
        }
    }
}
