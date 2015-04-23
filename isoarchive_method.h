#ifndef ISOARCHIVE_METHOD_INCLUDED
#define ISOARCHIVE_METHOD_INCLUDED

#include <apt-pkg/acquire-method.h>
#include "repo.h"

class isoarchive_method : public pkgAcqMethod
{
	public:
	isoarchive_method(std::unique_ptr<repo>);
	bool Fetch(FetchItem *);

	private:
	std::unique_ptr<repo> _repo;
};

#endif // ISOARCHIVE_METHOD_INCLUDED
