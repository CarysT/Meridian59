// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/* 
 * maketrig.c: Automatically generate .c and .h file containing discrete trig tables.
 */

#include <cstdio>
#include <cmath>
#include <process.h>
#include "drawdefs.h"

static char progname[] = "MAKETRIG";
static char header_file[] = "trig.h";
static char table_file[] = "trig.c";

void usage()
{
	printf("usage: maketrig <output directory>\n");
	exit(1);
}

void file_error()
{
	printf("Error opening output file. Exiting.\n");
	exit(1);
}

int discrete_log(int num)
{
	int i = 0;
	for (int x = 1; x < num; x <<= 1, i++);
	return i;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		usage();

	char* dir = argv[1];

	/* First spew the header file */
	char tempbuf[512];
	sprintf(tempbuf, "%s\\%s", dir, header_file);
	FILE* headerfile = fopen(tempbuf, "wt");

	if (headerfile == NULL)
		file_error();

	int num_degrees = NUMDEGREES;
	fprintf(headerfile, "/*\n");
	fprintf(headerfile, " * %s: Header file for trig tables.\n", header_file);
	fprintf(headerfile, " * This file was automatically generated by %s.\n", progname);
	fprintf(headerfile, " */\n\n");
	fprintf(headerfile, "#ifndef _TRIG_H\n");
	fprintf(headerfile, "#define _TRIG_H\n\n");
	fprintf(headerfile, "#define NUMDEGREES %d\n", num_degrees);
	fprintf(headerfile, "#define LOG_NUMDEGREES %d\n", discrete_log(num_degrees));
	fprintf(headerfile, "#define COS(x) (cos_table[x & (NUMDEGREES - 1)])\n");
	fprintf(headerfile, "#define SIN(x) (sin_table[x & (NUMDEGREES - 1)])\n");
	fprintf(headerfile, "\n");
	fprintf(headerfile, "\n#endif /* ifndef _TRIG_H */\n");

	fclose(headerfile);

	/* Now write out the tables themselves */
	sprintf(tempbuf, "%s\\%s", dir, table_file);
	FILE* tablefile = fopen(tempbuf, "wt");

	if (tablefile == NULL)
		file_error();

	fprintf(tablefile, "/*\n");
	fprintf(tablefile, " * %s: Trig tables.\n", table_file);
	fprintf(tablefile, " * This file was automatically generated by %s.\n", progname);
	fprintf(tablefile, " */\n");
	fprintf(tablefile, "#include \"client.h\"\n\n");

	/* cosine table */
	fprintf(tablefile, "double cos_table[%d] = {\n", num_degrees);

	for (int angle = 0; angle < num_degrees; angle++)
	{
		double radians = (double)angle * PITWICE / (double)num_degrees;

		double result = cos(radians);
      
		fprintf(tablefile, "0x%8.8x, ", (int)result);

		if (angle % 4 == 3)
			fprintf(tablefile, "\n");
	}

	fprintf(tablefile, "\n};\n\n");

	/* sine table */
	fprintf(tablefile, "double sin_table[%d] = {\n", num_degrees);

	for (int angle = 0; angle < num_degrees; angle++)
	{
		double radians = (double) angle * PITWICE / (double)num_degrees;

		double result = sin(radians);
      
		fprintf(tablefile, "0x%8.8x, ", (int)result);

		if (angle % 4 == 3)
			fprintf(tablefile, "\n");
	}

	fprintf(tablefile, "\n};\n");

	return 0;
}
