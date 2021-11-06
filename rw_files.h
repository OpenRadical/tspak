#ifndef __RW_FILES_H__
#define __RW_FILES_H__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

 /*! \addtogroup rw_files File helpers
  *
  * A variety of helper functions for working with files and file paths.
  *
  * @{
  */

  /*! Join parts into a path for the current system.
   *
   * \param[in] n_args The number of parts that are specified as var args.
   * \param[in] ...    Strings of path parts to join.
   *
   * \return Joined path.
   */
char* rw_join_path(size_t n_args, ...);

  /*! Fix path separators for the current system.
   *
   * \param[in] in     The path to fix.
   * \param[in] in_len The length of the path.
   */
void rw_fix_path(const char* in, size_t in_len);

/*! Read a file.
 *
 * \param[in]  filename Path to and name of a file to read.
 * \param[out] len      Length of the data read.
 *
 * \return Data read from file. If the file exists and is empty
 *         an empty string ("") is returned. On error NULL is returned.
 */
unsigned char* rw_read_file(const char* filename, size_t* len);

/*! Write data to a file.
 *
 * Will create the file if it does not exist.
 *
 * \param[in] filename Path to and name of a file to read.
 * \paran[in] data     Data to write to the file.
 * \param[in] len      Length of data to write.
 * \param[in] append   Should the data be appened to the file if it already exists.
 * \param[in] mkdirs   Should an attempt to make parent directories be made.
 *
 * \return Number of bytes written to the file.
 */
size_t rw_write_file(const char* filename, const unsigned char* data, size_t len, bool append, bool mkdirs);

/*! Create a directory on disk.
 *
 * A relative path will create relative to cwd.
 *
 * \param[in] name Directory path and name to create.
 *
 * \return true on success, otherwise false.
 */
bool rw_create_dir(const char* name);

/*! Check if a file exists.
 *
 * \param[in] filename File path and name. Can be relative.
 *
 * \return true on success, otherwise false.
 */
bool rw_file_exists(const char* filename);

/*! Get the size of a file.
 *
 * \param[in] filename File path and name. Can be relative.
 *
 * \return Size of file. -1 if file does not exist.
 */
int64_t rw_file_size(const char* filename);

/*! Delete a file.
 *
 * \param[in] filename File path and name. Can be relative.
 *
 * \return true on success, otherwise false.
 */
bool rw_file_unlink(const char* filename);

/*! Rename a file.
 *
 * \param[in] cur_filename File path and name to rename.
 * \param[in] new_filename New file path and name.
 * \param[in] overwrite    If new_filename exists should it be overwritten.
 *                         If false and new_filename this will fail.
 *
 * \return true on success, otherwise false.
 */
bool rw_rename(const char* cur_filename, const char* new_filename, bool overwrite);

#endif /* __RW_FILES_H__ */
