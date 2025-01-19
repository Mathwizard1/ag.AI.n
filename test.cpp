#include "gameMaster.h"

int main()
{
	gameMaster gm;

	std::vector<const char*> stringList = {
		"work",
		"if $me.energy < 30 {",
		"goto $zone.lunch ",
		"eat",
		"goto $me.workplace }",
		"jump !somewhere"
	};

	std::vector<short int> listSize = {5, 21, 18, 4, 21, 16};
	int Lsize = listSize.size();

	//gm.getCode(stringList, Lsize, listSize);
	//gm.processCode();

	return 0;
}