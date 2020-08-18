#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "../libfixmath/fixmath.h"

int main(int argc, char** argv) {
	FILE* fp = fopen("fix32_trig_sin_lut.h", "wb");
	if(fp == NULL) {
		fprintf(stderr, "Error: Unable to open file for writing.\n");
		return EXIT_FAILURE;
	}

	// TODO - Store as uint16_t with a count to determine the end and return 1.

	fprintf(fp, "#ifndef __fix32_trig_sin_lut_h__\n");
	fprintf(fp, "#define __fix32_trig_sin_lut_h__\n");
	fprintf(fp, "\n");

	fix32_t fix32_sin_lut_count = (fix32_pi >> (1+16));
	fix32_t* fix32_sin_lut = malloc(sizeof(fix32_t) * fix32_sin_lut_count);

	fix32_t i;
	for(i = 0; i < fix32_sin_lut_count; i++)
		fix32_sin_lut[i] = fix32_from_dbl(sin(fix32_to_dbl(i<<16)));
	for(i--; fix32_sin_lut[i] == fix32_one; i--, fix32_sin_lut_count--);

	fprintf(fp, "static const uint32_t _fix32_sin_lut_count = %"PRIi32";\n", fix32_sin_lut_count);
	fprintf(fp, "static uint32_t _fix32_sin_lut[%"PRIi32"] = {", fix32_sin_lut_count);

	for(i = 0; i < fix32_sin_lut_count; i++) {
		if((i & 7) == 0)
			fprintf(fp, "\n\t");
		fprintf(fp, "%"PRIi32", ", fix32_sin_lut[i]);
	}
	fprintf(fp, "\n\t};\n");

	fprintf(fp, "\n");
	fprintf(fp, "#endif\n");

	free(fix32_sin_lut);
	fclose(fp);

    return EXIT_SUCCESS;
}
