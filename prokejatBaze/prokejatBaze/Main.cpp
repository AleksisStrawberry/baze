#include "defs.h"


FILE *activeFile = NULL;
char activeFileName[20];





int get_int()
{
	char tempString[50];
	gets_s(tempString);
	return atoi(tempString);
}
Weapon EnterWeapon()
{
	Weapon w;
	char tempString[50];
	do {
		do {
			printf("Enter the identification number:\n");
			gets_s(tempString);
		} while (strlen(tempString) != 10);
		w.id = atoi(tempString);
	} while (w.id < 1000000000 || w.id > 9999999999);

	do {
		printf("Enter the maker:\n");
		gets_s(w.maker);
	} while (strlen(w.maker)>50 || strlen(w.maker) < 1);
	do {
		printf("Enter the mark:\n");
		gets_s(w.mark);
	} while (strlen(w.mark)>50 || strlen(w.mark) < 1);
	do {
		printf("Enter the price:\n");
		w.price = get_int();
	} while (w.price > 100000000 || w.price < 1);
	
	GetSystemTime(&w.date);

	return w;
}
//task 1
void CreateNewFile()
{
	FILE *file;
	char name[20];
	printf("Enter the file name:\n");

	fflush(stdin);
	gets_s(name);
	file = fopen(name, "wb");

	if (file == NULL) {
		printf("The file was not successfully created!\n");
	}
	else
		printf("'%s' successfully created!\n", name);

	fclose(file);
}

//task 2
void ChooseActiveFile()
{
	if (activeFile != NULL)
	{
		fclose(activeFile);
	}

	printf("Enter the name of active file:\n");
	fflush(stdin);
	gets_s(activeFileName);

	activeFile = fopen(activeFileName, "rb+");
	if (activeFile == NULL)
	{
		printf("%s File does not exist!", activeFileName);
	}
}

//task 3
void ShowActiveFile()
{
	if (activeFile != NULL)
	{
		printf("The name of active file is: %s\n", activeFileName);
	}
	else
		printf("The active file does not exist!\n");
}

//task 4
void SerialFileForming()
{
	char c[2];
	Weapon w;
	FILE *serialFile = NULL;
	serialFile = fopen("serialFile.bin", "wb");

	if (serialFile == NULL) {
		printf("Serial file was not successfully created!\n");
	}
	/*else
		printf("Serial file successfully created!\n");
	*/

	while (c[0] != 'n')
	{
		w = EnterWeapon();
		fwrite(&w, sizeof(Weapon), 1, serialFile);

		printf("Do you want to continue?(y/n)\n");
		gets_s(c);
	}


	fclose(serialFile);
}

//task 5
void SequentialFileForming()
{
	FILE *sequentialFile = NULL;
	FILE *serialFile = NULL;
	serialFile = fopen("serialFile.bin", "rb");
	sequentialFile = fopen("sequentialFile.bin", "wb+");

	if (serialFile == NULL || sequentialFile == NULL)
	{
		printf("Error with opening file!");
	}

	//citam iz serijske u stablo dokle god fread vraca broj podataka koji je razlicit od nula
	node *root = NULL;
	Weapon w;
	while (fread(&w, sizeof(Weapon), 1, serialFile) != 0)
	{
		root = insert_node(root, w);
	}

	treeIntoFile(root, sequentialFile);
	//display_tree(root);
	fseek(sequentialFile, 0, SEEK_SET);
	fread(&w, sizeof(Weapon), 1, sequentialFile);
	printf("ID is %d\nMaker is %s\nMark is %s\nPrice is %lf\nDate is: %u %u %u %u %u %u\n", w.id, w.maker, w.mark, w.price, w.date.wDay, w.date.wMonth, w.date.wYear, w.date.wHour, w.date.wMinute, w.date.wSecond);
	fread(&w, sizeof(Weapon), 1, sequentialFile);
	printf("ID is %d\nMaker is %s\nMark is %s\nPrice is %lf\nDate is: %u %u %u %u %u %u\n", w.id, w.maker, w.mark, w.price, w.date.wDay, w.date.wMonth, w.date.wYear, w.date.wHour, w.date.wMinute, w.date.wSecond);

}

//task 6
int CalculateCountOfNodes(int blockCounter, int height) {
	int count = 0;
	for (int i = 0; i < height; i++) {
		blockCounter = (blockCounter / 2) + (blockCounter % 2);
		count += blockCounter;
	}

	return count;
}

void IndexSequentialFileForming()
{
	if (activeFile == NULL)
	{
		printf("First chouse active file in meni(2).\n");
		return;
	}

	int adresa = sizeof(Header);
	fseek(activeFile, sizeof(Header), SEEK_SET);	//ostavljam prazno mesto za zaglavlje
	FILE *sequentialFile = fopen("sequentialFile.bin", "rb");
	Weapon w;
	Weapon weapons[FB];
	int weaponsInd = 0;
	int blockCounter = 0;
	int elementCounter = 0;
	while (fread(&weapons[weaponsInd++], sizeof(Weapon), 1, sequentialFile))
	{
		elementCounter++;

		if ((elementCounter + 1) % FB == 0)
		{
			memset(&weapons[weaponsInd++], 0, sizeof(Weapon));
			fwrite(weapons, sizeof(Weapon), FB, activeFile);
			adresa += FB * sizeof(Weapon);
			elementCounter++;
			blockCounter++;
			weaponsInd = 0;
		}
	}

	//ako je nacet blok, da dopuni sa praznim slogovima
	weaponsInd = (elementCounter % FB);
	if (weaponsInd > 0) {
		while (weaponsInd < FB) {
			memset(&weapons[weaponsInd++], 0, sizeof(Weapon));
			elementCounter++;
		}

		fwrite(weapons, sizeof(Weapon), FB, activeFile);
		adresa += FB * sizeof(Weapon);
	}

	int height = ceil(log2(blockCounter));
	int nodeCount = CalculateCountOfNodes(blockCounter, height);

	IndTreeNode *tempIndexZone = (IndTreeNode *)malloc(nodeCount * sizeof(IndTreeNode));
	int indexZoneAddress = adresa;

	Header header;
	header.sizeOfPrimary = blockCounter * FB * sizeof(Weapon);


	fseek(activeFile, sizeof(Header), SEEK_SET);
	adresa = sizeof(Header);
	IndTreeNode node;	//sta ako na pocetku nemam paran broj blokova?
	int i, nodeCounter = 0;
	//formiramo poslednji nivo stabla, elementi koji pokazuju na same blokove, i smestamo ih na pocetku indeks zone
	for (i = 0; i < blockCounter; i += 2) {
		if (fread(weapons, sizeof(Weapon), 5, activeFile) == 0) {
			break;
		}
		node.addrId1 = adresa;
		node.id1 = weapons[0].id; //preuzmemo id 0 elementa bloka jer je on najmanji

		adresa += 5 * sizeof(Weapon);

		if (fread(weapons, sizeof(Weapon), 5, activeFile) == 0) {
			memset(&weapons[0], 0, sizeof(Weapon));
			node.addrId2 = 0;
		}
		else {
			node.addrId2 = adresa;
			node.id2 = weapons[0].id;
			adresa += 5 * sizeof(Weapon);
		}

		//upisemo cvor stabla u indeks zonu
		memcpy(tempIndexZone + nodeCounter, &node, sizeof(IndTreeNode)); // tempIndexZone[nodeCounter] = node;
		nodeCounter++;
	}


	adresa = header.sizeOfPrimary + sizeof(Header);
	int levelNodeCount = nodeCounter;
	int j, current = 0;
	//formiramo ostatak stabla na osnovu formiranog poslednjeg nivoa
	for (i = 0; i < height - 1; i++) {
		for (j = 0; j < levelNodeCount; j += 2) {
			node.addrId1 = adresa;
			node.id1 = tempIndexZone[current].id1;
			current++;
			adresa += sizeof(IndTreeNode);

			if (levelNodeCount % 2 != 0 && j == levelNodeCount - 1) {
				node.id2 = 0;
				node.addrId2 = 0;
			}
			else {
				node.addrId2 = adresa;
				node.id2 = tempIndexZone[current].id1;
				current++;
				adresa += sizeof(IndTreeNode);
			}

			memcpy(tempIndexZone + nodeCounter, &node, sizeof(IndTreeNode)); // tempIndexZone[nodeCounter] = node;
			nodeCounter++;

			//printf("%d %d | %d %d\n", node.id1, node.addrId1, node.id2, node.addrId2);
		}

		levelNodeCount = (levelNodeCount / 2) + (levelNodeCount % 2); // = ceil(levelNodeCout / 2.0)
	}
	header.root = header.sizeOfPrimary + sizeof(Header) + ((nodeCounter - 1) * sizeof(IndTreeNode));
	fseek(activeFile, 0, SEEK_SET);
	fwrite(&header, sizeof(Header), 1, activeFile);

	fseek(activeFile, header.sizeOfPrimary + sizeof(Header), SEEK_SET);
	fwrite(tempIndexZone, sizeof(IndTreeNode), nodeCounter, activeFile);
	free(tempIndexZone);

	/*fseek(activeFile, header.root, SEEK_SET);
	fread(&node, sizeof(IndTreeNode), 1, activeFile);
	fseek(activeFile, node.addrId1, SEEK_SET);
	fread(&node, sizeof(IndTreeNode), 1, activeFile);
	printf("\n\n%d %d | %d %d\n\n", node.id1, node.addrId1, node.id2, node.addrId2);*/
}
int FindWeapon(int id, int * pindex) {
	Header header;
	fseek(activeFile, 0, SEEK_SET);
	fread(&header, sizeof(Header), 1, activeFile);
	*pindex = 0;
	if (header.root == 0)
	{
		return 0;
	}
	else
	{
		int is_left = 0;
		int r = 0;
		int cursorAddr = header.root;
		int prevAddr = 0;
		IndTreeNode cursor, prev;


		while (cursorAddr >= header.sizeOfPrimary)
		{
			fseek(activeFile, cursorAddr, SEEK_SET);
			fread(&cursor, sizeof(IndTreeNode), 1, activeFile);

			prev = cursor;
			if (id < cursor.id2 && id >= cursor.id1)
			{
				is_left = 1;
				cursorAddr = cursor.addrId1;
			}
			else if (id >= cursor.id2)
			{
				is_left = 0;
				cursorAddr = cursor.addrId2;
			}
			else {
				return 0;
			}
		}
		Weapon weapons[FB];
		fseek(activeFile, cursorAddr, SEEK_SET);
		fread(weapons, sizeof(Weapon), 5, activeFile);

		for (int i = 0; i < FB; i++) {
			if (weapons[i].id == 0) {
				return 0;
			}

			if (weapons[i].id == id && weapons[i].deleted == 0) {
				*pindex = i;
				return cursorAddr + (i * sizeof(Weapon));
			}
		}

		//trazi u listi prekoracioca!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	}
	return 0;
}

//task 7
void AddWeapon() {
	char c[2];
	Weapon w;

	while (c[0] != 'n')
	{
		w = EnterWeapon();

		Header header;
		fseek(activeFile, 0, SEEK_SET);
		fread(&header, sizeof(Header), 1, activeFile);

		if (header.root != 0)
		{
			int is_left = 0;
			int r = 0;
			int cursorAddr = header.root;
			int prevAddr = 0;
			IndTreeNode cursor, prev;


			while (cursorAddr >= header.sizeOfPrimary)
			{
				fseek(activeFile, cursorAddr, SEEK_SET);
				fread(&cursor, sizeof(IndTreeNode), 1, activeFile);

				prev = cursor;
				if (w.id < cursor.id2 && w.id >= cursor.id1)
				{
					is_left = 1;
					cursorAddr = cursor.addrId1;
				}
				else if (w.id >= cursor.id2)
				{
					is_left = 0;
					cursorAddr = cursor.addrId2;
				}
				else {
					printf("Error? AddWeapon\n");
				}
			}
			Weapon weapons[FB];
			fseek(activeFile, cursorAddr, SEEK_SET);
			fread(weapons, sizeof(Weapon), 5, activeFile);
			int i;
			for (i = 0; i < FB; i++) {
				if (weapons[i].id == w.id && weapons[i].deleted == 0) {
					printf("Id already exist.\n");
					break;
				}

				if (weapons[i].id == 0) {
					weapons[i] = w;
					fseek(activeFile, cursorAddr, SEEK_SET);
					fwrite(weapons, sizeof(Weapon), 5, activeFile);
					break;
				}
			}
			if (i == FB) {
				printf("There is no space in block.\n");
			}

			printf("Do you want to continue?(y/n)\n");
			gets_s(c);
		}
	}
}
//task 8
void SearchAndPrintWeapon() {
	char tempString[10];
	int id;

	printf("Enter weapon id: ");
	gets_s(tempString);
	id = atoi(tempString);

	int index;
	int address = FindWeapon(id, &index);
	if (address == 0) {
		printf("Weapon with id %d does not exists.\n", id);
	}
	else {
		if (index == 5) {
			printf("Prekoracilac:");
		}
		else {
			printf("Block address: %d\n", address - (index * sizeof(Weapon)));
			printf("Index in block: %d\n", index);
		}

		Weapon w;
		fseek(activeFile, address, SEEK_SET);
		fread(&w, sizeof(Weapon), 1, activeFile);
		printf("ID:\t%d\n", w.id);
		printf("Maker:\t%s\n", w.maker);
		printf("Mark:\t%s\n", w.mark);
		printf("Price:\t%.2lf\n", w.price);
		printf("Date:\t%d.%d.%d %d:%d:%d\n", w.date.wDay, w.date.wMonth, w.date.wYear, w.date.wHour, w.date.wMinute, w.date.wSecond);

	}
}

//task 9
void DeleteWeapon()
{
	char tempString[10];
	int id;

	printf("Enter weapon id: ");
	gets_s(tempString);
	id = atoi(tempString);

	int index;
	int address = FindWeapon(id, &index);

	if (address == 0) {
		printf("Weapon with id %d does not exists.\n", id);
	}
	else {
		if (index == 5) {
			printf("Prekoracilac:");
		}
		else {
			Weapon w;
			fseek(activeFile, address, SEEK_SET);
			fread(&w, sizeof(Weapon), 1, activeFile);
			w.deleted = 1;
			fseek(activeFile, address, SEEK_SET);
			fwrite(&w, sizeof(Weapon), 1, activeFile);
		}
	}
}
//task 10
void EditDateAndTime()
{
	char tempString[10];
	int id;

	printf("Enter weapon id: ");
	gets_s(tempString);
	id = atoi(tempString);

	int index;
	int address = FindWeapon(id, &index);

	if (address == 0) {
		printf("Weapon with id %d does not exists.\n", id);
	}
	else {
		if (index == 5) {
			printf("Prekoracilac:");
		}
		else {
			Weapon w;
			fseek(activeFile, address, SEEK_SET);
			fread(&w, sizeof(Weapon), 1, activeFile);
			GetSystemTime(&w.date);
			fseek(activeFile, address, SEEK_SET);
			fwrite(&w, sizeof(Weapon), 1, activeFile);
		}
	}
}
int main()
{

	char choice[10];

	do
	{
		printf("Meni:\n\n");
		printf("1. Create an empty file\n");
		printf("2. Choose the active file\n");
		printf("3. Show the file name\n");
		printf("4. Create the serial file\n");
		printf("5. Create the sequential file\n");
		printf("6. Create active file from sequential\n");
		printf("7. Add new weapon to the active file\n");
		printf("8. Search weapon\n");
		printf("9. Delete weapon from active file\n");
		printf("10.Change the value of date and time\n");
		printf("11. Exit!\n");

		gets_s(choice);

		switch (atoi(choice))
		{
		case 1: CreateNewFile();
			break;
		case 2: ChooseActiveFile();
			break;
		case 3: ShowActiveFile();
			break;
		case 4: SerialFileForming();
			break;
		case 5: SequentialFileForming();
			break;
		case 6:
			IndexSequentialFileForming();
			break;
		case 7:
			AddWeapon();
			break;
		case 8:
			SearchAndPrintWeapon();
			break;
		case 9:
			DeleteWeapon();
			break;
		case 10:
			EditDateAndTime();
			break;
		case 11:
			printf("Goodbye\n\n");
			return 0;
		default: printf("Wrong Choice. Enter again\n");
			break;
		}

	} while (1);

	//_getch();

	return 0;
}

