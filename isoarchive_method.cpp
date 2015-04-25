#include "isoarchive_method.h"
#include <apt-pkg/hashes.h>
#include <apt-pkg/fileutl.h>

using namespace std;

isoarchive_method::isoarchive_method(unique_ptr<repo> r)
: pkgAcqMethod("1.0", SingleInstance | NeedsCleanup), _repo(move(r))
{
}

bool isoarchive_method::Fetch(FetchItem *item)
{
	if (!_repo) {
		SetFailReason("Cannot find a valid repo.");
		return false;
	}

	FetchResult res;
	struct stat stat;
	res.Filename = _repo->get(item->Uri, stat);

	if (!res.Filename.empty()) {
		URIStart(res);
		res.Size = stat.st_size;
		Hashes hash;
		FileFd fd(res.Filename, FileFd::ReadOnly);
		hash.AddFD(fd.Fd(), fd.Size());
		res.TakeHashes(hash);
		URIDone(res);
		return true;
	} else {
		SetFailReason("file not found");
		return false;
	}
}
