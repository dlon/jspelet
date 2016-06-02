#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TREAD(b, s, ss, fp) { \
	if (fread(b,s,ss,fp) != ss) { return; } \
	}

const char *IdentifyTag(int i)
{
	switch (i) {
		case 0: return "SolidV1";
		case 1: return "TileV1";
		case 2: return "EntityV1";
		case 3: return "TileV2";
		case 4: return "EntityV2";

		case 500: return "MusicV1";
		case 600: return "EditorMetaTagsV1";
		case 700: return "MapBoundarySizeV1";
		case 800: return "GeometryV1";

		case 900: return "EntityV3";
		case 1000: return "SkyColorV1";
	}
	return "Unknown";
}

void ShowShortTagInfo(FILE *fp)
{
	char buffer[512];
	int size, id;

	TREAD(&size, sizeof(size), 1, fp);
	TREAD(&id, sizeof(id), 1, fp);

	printf("\tSize: %i\n", size);
	printf("\tID: %i (%s)\n\n", id, IdentifyTag(id));

	fseek(fp, size-4, SEEK_CUR);
}

void ShowTagInfo(FILE *fp)
{
	char buffer[512];
	int size, id;

	TREAD(&size, sizeof(size), 1, fp);
	TREAD(&id, sizeof(id), 1, fp);

/*
	printf("\n\n----------\n");

	printf("\tSize: %i\n", size);
	printf("\tID: %i (%s)\n", id, IdentifyTag(id));

	printf("\n\n\n----------\n");
*/

	fseek(fp, size-4, SEEK_CUR);
}

void Routine(const char *file)
{
	FILE *fp = fopen(file, "rb");
	if (fp) {
		printf("-----------\n");
		for (;;) {
			ShowShortTagInfo(fp);

			if (feof(fp))
				break;
		}

		printf("\n---------\n\n");
		fseek(fp, 0, SEEK_SET);

		puts("Press any key to continue.");
		getc(stdin);

		for (;;) {
			ShowTagInfo(fp);

			if (feof(fp))
				break;
		}

		fclose(fp);
	}
	else {
		printf("%s: ", file);
		perror("");
	}
}

int main(int argc, char **argv)
{
	for (int i=1; i<argc; i++)
		Routine(argv[i]);

	puts("Press any key to continue.");
	getc(stdin);
	return 0;
}
