#include "debug.h"
#include "str.h"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

static void append(String* str, const char* txt, const int len){
    for(int i = 0; i < len; i++)
        strapp(str, txt[i]);
}

static char* getfg(char* key, char len){
    switch(len){
        case 3: 
            if(memcmp(key, "red", 3)==0) return "31";
            if(memcmp(key, "std", 3)==0) return "39";
        case 4: 
            if(memcmp(key, "cyan", 4)==0) return "36";
            if(memcmp(key, "blue", 4)==0) return "34";
        case 5: 
            if(memcmp(key, "black", 5)==0) return "30";
            if(memcmp(key, "white", 5)==0) return "37";
            if(memcmp(key, "green", 5)==0) return "32";
        case 6:
            if(memcmp(key, "yellow", 6)==0) return "33";
        case 7:
            if(memcmp(key, "magenta", 7)==0) return "35";
        default: return "39";
    }
}

static char* getbg(char* key, char len){
    switch(len){
        case 3: 
            if(memcmp(key, "red", 3)==0) return ";41";
            if(memcmp(key, "std", 3)==0) return ";49";
        case 4: 
            if(memcmp(key, "cyan", 4)==0) return ";46";
            if(memcmp(key, "blue", 4)==0) return ";44";
        case 5: 
            if(memcmp(key, "black", 5)==0) return ";40";
            if(memcmp(key, "white", 5)==0) return ";47";
            if(memcmp(key, "green", 5)==0) return ";42";
        case 6:
            if(memcmp(key, "yellow", 6)==0) return ";43";
        case 7:
            if(memcmp(key, "magenta", 7)==0) return ";45";
        default: return ";49";
    }
}

static char* getattr(char* key, char len){
    switch(len){
        case 4:
            if(memcmp(key, "slim", 4)==0) return ";2";
            if(memcmp(key, "bold", 4)==0) return ";1";
        case 5:
            if(memcmp(key, "blink", 5)==0) return ";5";
        case 6:
            if(memcmp(key, "italic", 6)==0) return ";3";
            if(memcmp(key, "strike", 6)==0) return ";9";
        case 7:
            if(memcmp(key, "reverse", 7)==0) return ";7";
        case 9:
            if(memcmp(key, "underline", 9)==0) return ";4";
        default: return ";1";
    }
}

void LOG(const char* format, ...){
    char text[512];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);

    String str = newstr("");
    
    int i = 0;
    while(text[i]){
        if(text[i] == '$' && text[i+1] == '<'){
            i += 2;
            String esc = newstr("");
            int index = 0;
            append(&str, "\e[", 2);
            while(text[i] && text[i] != ','){
                if(!isalpha(text[i]) && text[i] != ','){ i++; continue; }
                while(isalpha(text[i]))
                    strapp(&esc, text[i++]);

                switch(index) {
                    case 0: append(&str, getfg(esc.data, esc.size), 2); break;
                    case 1: append(&str, getbg(esc.data, esc.size), 3); break;
                    default: append(&str, getattr(esc.data, esc.size), 2); break;
                }
                strclean(&esc);
                index++;
            }
            i++; // skip the comma or closing '>'
            strdel(&esc);
            strapp(&str, 'm');
            while(text[i] && !(text[i] == '>' && text[i+1] == '$'))
                strapp(&str, text[i++]);
            if(text[i] == '>' && text[i+1] == '$'){
                i += 2;
                append(&str, "\e[0m", 4);
            }
            continue;
        }
        strapp(&str, text[i++]);
    }

    printf("%s\n", str.data);
    strdel(&str);
}
