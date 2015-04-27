#include "misc.h"
#include <libudev.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>
#include <blkid/blkid.h>

using namespace std;

unique_ptr<vector<string>>
removable_part_paths()
{
	unique_ptr<vector<string>> paths (new vector<string>());

	struct udev *udev = udev_new();
	if (!udev)
		throw runtime_error("Error accessing udev");

	struct udev_enumerate *enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "block");
	udev_enumerate_add_match_sysattr(enumerate, "partition", 0);
	udev_enumerate_scan_devices(enumerate);
	struct udev_list_entry *devices =
		udev_enumerate_get_list_entry(enumerate);

	struct udev_list_entry *dev_list_entry;
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device *dev = udev_device_new_from_syspath(udev, path);
		struct udev_device *parent = udev_device_get_parent(dev);

		const char *removable = 
			udev_device_get_sysattr_value(parent, "removable");
		if (removable && (strcmp(removable, "1") == 0))
			paths->push_back(udev_device_get_devnode(dev));
	}

	udev_enumerate_unref(enumerate);
	udev_unref(udev);

	return paths;
}

system_error errno_error(const char *what)
{
	return system_error(error_code(errno, generic_category()), what);
}

string filesystem_type(const string& filename)
{
	string result;

	blkid_probe p = blkid_new_probe_from_filename(filename.c_str());
	int rc = blkid_do_probe(p);
	if (rc == 0) {
		const char *type;
		rc = blkid_probe_lookup_value(p, "TYPE", &type, 0);
		if (rc == 0) {
			result = type;
		}
	}

	blkid_free_probe(p);
	return result;
}

class dir
{
    public:
	dir(const string &path) {
		c_object = opendir(path.c_str());
		if (c_object == 0)
			throw errno_error("opening directory");
	}

	~dir() {
		closedir(c_object);
	}

	DIR *c_object;
};

string get_first_iso_path(const string &dirname)
{
	dir d(dirname);

	while (true) {
		struct dirent *e = readdir(d.c_object);
		if (e) {
			if (e->d_name[0] == '.') /* skip hidden */
				continue;
			string filename = dirname + "/" + e->d_name;
			struct stat stat;
			int rc = lstat(filename.c_str(), &stat);
			if (rc == -1)
				throw errno_error("lstat()");

			if (S_ISREG(stat.st_mode) &&
			    filesystem_type(filename) == "iso9660")
				return filename;
		} else {
			return string();
		}
	}
}

struct tempdir
{
	tempdir() {
		char pattern[] = "/tmp/repoimage.XXXXXX";
		path = mkdtemp(pattern);
		if (path.empty()) {
			throw errno_error("mkdtemp()");
		}
	}

	~tempdir() {
		int status = rmdir(path.c_str());
		if (status == -1) {
			throw errno_error("rmdir()");
		}
	}

	string path;
};

mount::mount(const string &devpath)
{
	/* Find or make a mountpoint corresponding to devpath. */

	ifstream mounts("/proc/mounts");
	while (mounts) {
		string dev, mountpoint, rest;
		getline(mounts, dev, ' ');
		getline(mounts, mountpoint, ' ');
		getline(mounts, rest);

		if (dev == devpath) {
			_path = mountpoint;
			break;
		}
	}

	if (_path.empty()) {
		/* Call mount(2) to mount the partition. */

		_tempdir.reset(new tempdir());
		_path = _tempdir->path;

		pid_t child = fork();
		switch (child) {
		case -1:
			throw errno_error("forking");

		case 0: /* child */
			execl("/bin/mount",
				"mount", "-r",
				"-o", "loop", /* indeded needed on squeeze */
				devpath.c_str(),
				_path.c_str(),
				(char*)0);
			throw errno_error("exec'ing mount");

		default: /* parent */
			int rc, status;
			rc = waitpid(child, &status, 0);
			if (rc == -1)
				throw errno_error("waitpid()");

			if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
				throw failure();
			}
		}
	}
}

mount::~mount()
{
	if (_tempdir) {
		int rc = umount(_path.c_str());
		if (rc == -1)
			throw errno_error("unmounting");
	}
}
