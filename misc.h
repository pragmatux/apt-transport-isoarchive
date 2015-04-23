#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include <memory>
#include <vector>
#include <string>

std::unique_ptr<std::vector<std::string>>
removable_part_paths();
	/* Return a vector of device node paths to all partitions on removable
	 * block devices. */

std::string
filesystem_type(const std::string& filename);
	/* Return a string naming the filesystem type or an empty string. */

std::string
get_first_iso_path(const std::string &dir);
	/* Return the path to the alphabetically-first iso-format file in
	   dir. */

class tempdir;

class mount
{
	public:
		class failure {};
		mount(const std::string &filename);
		~mount();
		std::string path() {return _path;}

	private:
		std::string _path;
		std::unique_ptr<tempdir> _tempdir;
};

#endif // MISC_H_INCLUDED
