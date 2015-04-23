#include "repo.h"
#include "isoarchive_method.h"

using namespace std;

int main(int argc, char *argv[])
{
	isoarchive_method method(find_repo());
	return method.Run();
}
