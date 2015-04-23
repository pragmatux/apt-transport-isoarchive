#include <iostream>
#include "misc.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Removable drive partitions: \n";

	auto paths = removable_part_paths();
	for (auto p = paths->begin(); p != paths->end(); ++p) {
		cout << *p << "\n";
	}
}
