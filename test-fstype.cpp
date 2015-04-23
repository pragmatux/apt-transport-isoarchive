#include "misc.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	cout << filesystem_type(argv[1]) << endl;
}
