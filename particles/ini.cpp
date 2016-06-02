#include "ini.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ini_goto_section(FILE *f, const char *section) { /* non-zero = success */
	rewind(f); /* start from beginning */
	
	size_t sz = strlen(section)+3;
	char *b = (char*)malloc(sz); /* to hold [section] */
	char *z = (char*)malloc(sz); /* to hold comparison string */
	sprintf(b,"[%s]",section);
	
	while( !feof(f) ) {
		/* discard spaces/tabs at the beginning of the line */
		char ch = getc(f);
		while(ch == 32 || ch == 9)
			ch = getc(f);
		fseek(f,-1,SEEK_CUR);
		
		/* skip comments */
		if(getc(f) == ';'){
			while(getc(f) != '\n' && !feof(f));
			continue;
		}
		fseek(f,-1,SEEK_CUR);
		
		/* is this it? */
		fgets(z, (int)sz, f); /* should error check */
		if( !strcmp(b,z) ){
			free(b);
			free(z);
			while(getc(f) != '\n' && !feof(f)); /* skip newline [and unexpected tokens] */
			return 1;
		}
	}
	free(b);
	free(z);
	return 0;
}

const char *ini_read_str(const char *file, const char *section, const char *key, const char *default_value) {
	static char retbuf[INI_STRING_BUFFER_SIZE];
	
	strcpy(retbuf,default_value);
	
	/* open file */
	FILE *f = fopen(file,"r");
	if( !f )
		return retbuf;
	
	/* get value */
	if( ini_goto_section(f,section) ) {
		while( !feof(f) ) { /* read until feof, new section or our key */
			/* discard spaces/tabs at the beginning of the line */
			char ch; /* temp */
			ch = getc(f);
			while(ch == 32 || ch == 9)
				ch = getc(f);
			fseek(f,-1,SEEK_CUR);
			
			/* stop now if the section has ended */
			if(getc(f) == '[')
				break;
			fseek(f,-1,SEEK_CUR);
			
			/* skip comments */
			if(getc(f) == ';'){
				while(getc(f) != '\n' && !feof(f));
				continue;
			}
			fseek(f,-1,SEEK_CUR);
			
			/* is this the key we want? */
			ch = getc(f);
			size_t cnt=0; /* counts the number of chars read */
			while( cnt < strlen(key) ){
				if(key[cnt] != ch)
					break;
				
				ch = getc(f);
				cnt++;
			}
			fseek(f,-1,SEEK_CUR);
			
			/* it matched... if the next char is a '=' */
			if(cnt == strlen(key)){
				/* discard spaces and tabs */
				ch = getc(f);
				while(ch == 32 || ch == 9)
					ch = getc(f);
				fseek(f,-1,SEEK_CUR);
				
				/* this confirms whether it's the right key */
				if(getc(f) != '='){
					/* wrong :( */
					while(getc(f) != '\n' && !feof(f));
					continue;
				}
				
				/* discard spaces and tabs before value */
				ch = getc(f);
				while(ch == 32 || ch == 9)
					ch = getc(f);
				fseek(f,-1,SEEK_CUR);
				
				/* [move check for quotations HERE instead if not too lazy] */
				
				/* get the value! */
				fgets(retbuf,sizeof(retbuf),f); /* should error check */
				if(retbuf[strlen(retbuf)-1] == '\n') /* get rid of newline */
					retbuf[strlen(retbuf)-1] = '\0';

				/* check for quotation marks */
				int quot_len=-1; /* length minus quotes */
				if(retbuf[0] == '"'){
					/* find last quotation mark */
					int xx;
					char prev_is_esc=0; /* watch out for escaped quotes */
					for(xx=1; xx<(int)strlen(retbuf); xx++){
						if(retbuf[xx] == '"' && !prev_is_esc){
							quot_len=xx-1;
							break;
						}
						else if(retbuf[xx] == '\\' && !prev_is_esc){ /* esc char? */
							prev_is_esc=1;
						}
						else {
							prev_is_esc=0;
						}
					}
					if(quot_len == -1) /* unenclosed quote */
						quot_len = (int)strlen(retbuf)-1;
				}
				if(quot_len != -1){
					/* move string... */
					memmove(retbuf,retbuf+1,quot_len);
					retbuf[quot_len] = '\0';
				}
				
				/* check for comments at the end */
				if(quot_len == -1){
					int zz=0;
					while(retbuf[zz] != ';' && retbuf[zz] != '\0') zz++;
					retbuf[zz] = '\0';
					/* remove spaces/tabs at the end now */
					zz--;
					while(retbuf[zz] == 32 || retbuf[zz] == 9) zz--;
					retbuf[zz+1] = '\0';
				}
				break;
			}
			else {
				while(getc(f) != '\n' && !feof(f));
			}
		}
	}

	fclose(f);
	return retbuf;
}

int ini_read_int(const char *file, const char *section, const char *key, int default_value) {
	int ret = default_value;

	/* read it as a string first... */
	const char *t = ini_read_str(file, section, key, "");
	if( strcmp("",t) != 0 )
		ret = atoi(t);

	return ret;
}

double ini_read_double(const char *file, const char *section, const char *key, double default_value) {
	double ret = default_value;

	/* read it as a string first... */
	const char *t = ini_read_str(file, section, key, "");
	if( strcmp("",t) != 0 )
		ret = atof(t);

	return ret;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
