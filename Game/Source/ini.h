#ifndef _INI_H_
#define _INI_H_

/**
 * \file ini.h
 * \brief INI Access.
 *
 * ini.h provides functions to read or write ini files.
 */

/**
 * Writes a string into an ini file.
 * \param	filename	The absolute path to the filename of the ini file.
 * \param	section		The section to write into.
 * \param	entry		The key of the section entry to write.
 * \param	value		The value which will be written into the key entry.
 */
bool ini_write(STRING *filename, STRING *section, STRING *entry, STRING *value);

/**
 * Writes an integer into an ini file.
 * \param	filename	The absolute path to the filename of the ini file.
 * \param	section		The section to write into.
 * \param	entry		The key of the section entry to write.
 * \param	value		The value which will be written into the key entry.
 */
bool ini_write_int(STRING *filename, STRING *section, STRING *entry, int value);

/**
 * Writes a var into an ini file.
 * \param	filename	The absolute path to the filename of the ini file.
 * \param	section		The section to write into.
 * \param	entry		The key of the section entry to write.
 * \param	value		The value which will be written into the key entry.
 */
bool ini_write_var(STRING *filename, STRING *section, STRING *entry, var value);

/**
 * Writes a float into an ini file.
 * \param	filename	The absolute path to the filename of the ini file.
 * \param	section		The section to write into.
 * \param	entry		The key of the section entry to write.
 * \param	value		The value which will be written into the key entry.
 */
bool ini_write_float(STRING *filename, STRING *section, STRING *entry, float value);

/**
 * Reads all sections from an ini file into a TEXT object.
 * \param	txt			The TEXT object where the sections will be stored in.
 * \param	filename	The absolute path to the filename of the ini file.
 * \return				Number of sections read from the ini file.
 */
int ini_read_sections(TEXT *txt, STRING *filename);

/**
 * Reads a string from an ini file.
 * \param	targetValue		The string where the result will be stored in.
 * \param	filename		The absolute path to the filename of the ini file.
 * \param	section			The section to read from.
 * \param	entry			The key of the section entry to read.
 * \param	defaultValue	The default value which will be written into the return value if the key doesn't exist.
 * \return					Number of characters read into the string.
 */
int ini_read(STRING *targetValue, STRING *filename, STRING *section, STRING *entry, STRING *defaultValue);

/**
 * Reads an integer from an ini file.nichts
 * \param	filename		The absolute path to the filename of the ini file.
 * \param	section			The section to read from.
 * \param	entry			The key of the section entry to read.
 * \param	defaultValue	The default value which will be returned if the key doesn't exist.
 * \return					The entry value converted to an integer.
 */
int ini_read_int(STRING *filename, STRING *section, STRING *entry, int defaultValue);

/**
 * Reads a float from an ini file.
 * \param	filename		The absolute path to the filename of the ini file.
 * \param	section			The section to read from.
 * \param	entry			The key of the section entry to read.
 * \param	defaultValue	The default value which will be returned if the key doesn't exist.
 * \return					The entry value converted to an float.
 */
float ini_read_float(STRING *filename, STRING *section, STRING *entry, float defaultValue);

/**
 * Reads a var from an ini file.
 * \param	filename		The absolute path to the filename of the ini file.
 * \param	section			The section to read from.
 * \param	entry			The key of the section entry to read.
 * \param	defaultValue	The default value which will be returned if the key doesn't exist.
 * \return					The entry value converted to a var.
 */
var ini_read_var(STRING *filename, STRING *section, STRING *entry, var defaultValue);

#endif
