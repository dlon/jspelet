#pragma once
#ifndef INI_H
#define INI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define INI_STRING_BUFFER_SIZE 2048
/* BIG FAT NOTE: the static buffer returned by ini_read_str gets overwritten on each call */
extern int		 ini_read_int(const char *file, const char *section,
							  const char *key, int default_value);
extern double	ini_read_double(const char *file,
								const char *section,
								const char *key,
								double default_value);
extern const char	*ini_read_str(const char *file, const char *section,
								  const char *key, const char *default_value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ! INI_H */
