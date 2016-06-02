#ifdef __cplusplus
extern "C" {
#endif

#include "_wavinfo.h" 
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// DETERMINE AUTHENTICY

int is_wave_file(FILE *file) {
	assert(file);
	
	// seek to beginning
	fseek(file,0,SEEK_SET);
	
	// get RIFF info
	char riffid[4];
	int  sz;
	
	fread(riffid,1,sizeof(riffid),file);
	fread(&sz,sizeof(sz),1,file);
	
	// confirm it's a wave file
	char rifftype[4];
	fread(rifftype, 1, sizeof(rifftype), file);
	
	if( !memcmp(riffid,"RIFF",sizeof(riffid)) ){
		if( !memcmp(rifftype,"WAVE",sizeof(rifftype)) ){
			return sz;
		}
	}

	// reset cursor if it isn't a wave file
	fseek(file, 0, SEEK_SET);
	return 0;
}

// READ WAVE FILE [CHUNKS]

/** INTERNAL WAVE CHUNK FUNCTIONS **/

static void FreeChunk(WaveChunk *r) {
	free(r->data);
	free(r);
}

static WaveChunk *GetChunk(FILE *fp) {
	assert(fp);
	
	if( feof(fp) )
		return NULL;
	
	WaveChunk *ret = (WaveChunk*)malloc(sizeof(WaveChunk));
	
	// get RIFF info
	fread(ret->id, 1, sizeof(ret->id)-1, fp);
	fread(&ret->sz, sizeof(ret->sz), 1, fp);
	
	if( ferror(fp) || feof(fp) ){
		free(ret);
		return NULL;
	}
	
	ret->id[sizeof(ret->id)-1] = '\0'; // null-terminate
	
	// read data
	ret->data = (char*)malloc(ret->sz);
	fread(ret->data, 1, ret->sz, fp);
	
	// if error; ignore and move on
	if( ferror(fp) ){
		FreeChunk(ret);
		printf("GetChunk Warning: Failed to read data from '%s'.\n",ret->id);
		return NULL;
	}
	
	return ret;
}

/** END OF INTERNAL WAVE CHUNK FUNCTIONS **/

_wavinfo *wavinfo_load(const char *file) {
	FILE *fp = fopen(file,"rb");
	if(!fp){
		fprintf(stderr,"Couldn't open %s!\n",file);
		return NULL;
	}
	_wavinfo *ret = (_wavinfo*)malloc(sizeof(_wavinfo));
	
	if(ret){
		/* to hold our required info... */
		WaveChunk *__format = NULL;
		WaveChunk *__data = NULL;
		
		/* confirm it's a wave file to begin with */
		if(!is_wave_file(fp)){
			fprintf(stderr,"%s: Not a wave file!\n",file);
			fclose(fp);
			free(ret);
			return NULL;
		}
		
		/* read all wave info + data */
		WaveChunk *temp;
		while( (temp = GetChunk(fp)) )
		{
			if( !strcmp( temp->id, "fmt " ) ){
				__format = temp;
				temp = NULL; // don't free
			}
			else if( !strcmp( temp->id, "data" ) ){
				printf("Data: %d bytes.\n", temp->sz );
				
				__data = temp;
				temp=NULL; // don't free
			}
			else if( !strcmp( temp->id, "LIST" ) ){ // meta info I believe
			}
			else {
				printf("%s warning: Unsupported WAV chunk: '%s'.\n",file,temp->id);
			}
			if (temp != NULL)
				FreeChunk(temp);
		}
		
		/* confirm we got the info we needed *
		 * and then fill it into the struct  */
		if(!__format || !__data){ // fail!!!
			if(__format)
				FreeChunk(__format);
			else
				fprintf(stderr,"%s: File contains no format info.\n",file);
			if(__data)
				FreeChunk(__data);
			else
				fprintf(stderr,"%s: File contains no data info.\n",file);
			
			fclose(fp);
			free(ret);
			
			return NULL;
		}
		
		/* first our data chunk */
		ret->data = __data->data;
		ret->data_len = __data->sz;
		
		/* then our format chunk */
		ret->rate = *(int*)(__format->data+4);
		ret->bitspersec = *(short*)(__format->data+14);
		ret->channels = *(short*)(__format->data+2);
		
		/* save the chunks so we can free them later */
		ret->__data = __data;
		ret->__format = __format;
	}
	
	fclose(fp);
	return ret;
}

void wavinfo_free(_wavinfo *w) {
	/* free info + data */
	FreeChunk(w->__format);
	FreeChunk(w->__data);
	
	free(w);
}

#ifdef __cplusplus
}
#endif
