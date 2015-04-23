#include "repo.h"
#include "misc.h"
#include <unistd.h>

using namespace std;

repo::repo(unique_ptr<mount> p, unique_ptr<mount> i)
: partition(move(p))
, iso(move(i))
{
}

string repo::get(const string &uri, struct stat &res)
{
	const string prefix = "isoarchive:search-removable";
	int pos = uri.find(prefix);
	if (pos != 0)
		throw invalid_uri();

	string path = iso->path() + "/archive" + uri.substr(prefix.length());

	int status = lstat(path.c_str(), &res);
	if (status == 0) {
		return path;
	} else {
		return string();
	}
}

bool repo_is_valid(const string &path)
{
	string testpath = path + "/archive/dists/master/main";
	return access(testpath.c_str(), R_OK) == 0;
}

unique_ptr<repo> find_repo()
{
	unique_ptr<repo> _repo;
	auto parts = removable_part_paths();

	for (auto part = parts->begin(); part != parts->end(); ++part)
	try {
		unique_ptr<mount> part_mount(new mount(*part));
		string iso(get_first_iso_path(part_mount->path()));
		if (iso.empty())
			continue;

		unique_ptr<mount> iso_mount(new mount(iso));
		if (repo_is_valid(iso_mount->path())) {
			_repo.reset(new repo(move(part_mount), move(iso_mount)));
			break;
		}
	}
	catch (const mount::failure &e)
	{
		continue;
	}

	return _repo;
}

