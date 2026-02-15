#ifndef THEIA_STR_H
#define THEIA_STR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* data;
    unsigned long size, cap;
} String;
extern void strapp(String*, const int c);
extern String newstr(const char*);
extern void strdel(String*);
extern void strundo(String*);
extern void strclean(String*);

#ifdef __cplusplus
}
#endif


#endif