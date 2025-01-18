#include "gameMaster.h"

int main()
{
	gameMaster gm;

	std::vector<const char*> stringList = {
		//"work 10",
		"if($rahul.working ==) {",
		"if =true) ",
		"Goto ( 10 )",
		"Zone $ lunch)",
		"break ( 5"
	};

	std::vector<short int> listSize = { 29, 13, 12, 13, 10 };
	int Lsize = listSize.size();

	gm.getCode(stringList, Lsize, listSize);

	return 0;
}