#include "misc.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	mount m(argv[1]);
	cout << m.path() << endl;
	sleep(30);

	return 0;
}
