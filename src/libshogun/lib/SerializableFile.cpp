/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2010 Soeren Sonnenburg
 * Copyright (C) 1999-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 * Copyright (C) 2010 Berlin Institute of Technology
 */

#include "lib/SerializableFile.h"

using namespace shogun;

CSerializableFile::CSerializableFile(void) :CSGObject()
{
	init(NULL, 0, "(file)");
}

CSerializableFile::CSerializableFile(FILE* f, char rw) :CSGObject()
{
	init(f, rw, "(file)");
}

CSerializableFile::CSerializableFile(char* fname, char rw) :CSGObject()
{
	char mode[3] = {rw, 'b', '\0'};

	init(NULL, rw, fname);

	if (rw != 'r' && rw != 'w') {
		SG_WARNING("Unknown mode '%c'!\n", mode[0]);
		close(); return;
	}

	if (filename == NULL || *filename == '\0') {
		SG_WARNING("Filename not given for opening file!\n");
		close(); return;
	}

	file = fopen(filename, mode);
	if (file == NULL) {
		SG_WARNING("Error opening file '%s'\n", filename);
		close(); return;
	}
}

CSerializableFile::~CSerializableFile(void)
{
	close();
	if (filename != NULL) { free(filename); filename = NULL; }
}

void
CSerializableFile::init(FILE* file_, char task_, const char* filename_)
{
	file = file_; task = task_; filename = strdup(filename_);
}

void
CSerializableFile::close()
{
	if (file != NULL) { fclose(file); file = NULL; }
	task = 0;
}

bool
CSerializableFile::is_opened(void)
{
	return file != NULL;
}

bool
CSerializableFile::is_task_warn(char rw)
{
	if (rw == 'w' && task != 'w') {
		SG_WARNING("`%s' not opened for writing!\n", filename);
		return false;
	}
	if (rw == 'r' && task != 'r') {
		SG_WARNING("`%s' not opened for reading!\n", filename);
		return false;
	}

	return true;
}

bool
CSerializableFile::false_warn(const char* prefix, const char* name)
{
	if (task == 'w')
		SG_WARNING("Could not write `%s%s' from `%s'!", prefix,
				   name, filename);
	if (task == 'r')
		SG_WARNING("Could not read `%s%s' from `%s'!", prefix,
				   name, filename);
	if (task != 'w' && task != 'r')
		SG_WARNING("Could not read/write `%s%s' from `%s'!",
				   prefix, name, filename);

	return false;
}

bool
CSerializableFile::write_type(const TSGDataType* type, const void* param,
						const char* name, const char* prefix)
{
	if (!is_task_warn('w')) return false;

	if (!write_type_wrapped(type, param, name, prefix))
		return false_warn(prefix, name);

	return true;
}

bool
CSerializableFile::read_type(const TSGDataType* type, void* param,
					   const char* name, const char* prefix)
{
	if (!is_task_warn('r')) return false;

	if (!read_type_wrapped(type, param, name, prefix))
		return false_warn(prefix, name);

	return true;
}