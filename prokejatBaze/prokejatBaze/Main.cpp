#include "defs.h"


FILE *activeFile = NULL;
char activeFileName[20];





int get_int()
{
	char tempString[15];
	gets_s(tempString);
	return atoi(tempString);
}
Weapon EnterWeapon()
{
	Weapon w;
	printf("Enter the identification number:\n");
	w.id = get_int();
	printf("Enter the maker:\n");
	gets_s(w.maker);
	printf("Enter the mark:\n");
	gets_s(w.mark);
	printf("Enter the price:\n");
	w.price = get_int();

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

	/*fseek(serialFile, 0, SEEK_SET);
	fread(&w, sizeof(Weapon), 1, serialFile);
	printf("ID is %d\nMaker is %s\nMark is %s\nPrice is %lf\nDate is: %u %u %u %u %u %u\n", w.id, w.maker, w.mark, w.price, w.date.wDay, w.date.wMonth, w.date.wYear, w.date.wHour, w.date.wMinute, w.date.wSecond);
	fread(&w, sizeof(Weapon), 1, serialFile);
	printf("ID is %d\nMaker is %s\nMark is %s\nPrice is %lf\nDate is: %u %u %u %u %u %u\n", w.id, w.maker, w.mark, w.price, w.date.wDay, w.date.wMonth, w.date.wYear, w.date.wHour, w.date.wMinute, w.date.wSecond);
	*/
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
	int adresa = sizeof(int);
	fseek(activeFile, sizeof(int), SEEK_SET);	//ostavljam prazno mesto za zaglavlje
	FILE *sequentialFile = fopen("sequentialFile.bin", "rb");
	Weapon w;
	Weapon weapons[FB];
	int blockCounter = 0;
	int elementCounter = 0;
	while (fread(&w, sizeof(Weapon), 1, sequentialFile))
	{
		fwrite(&w, sizeof(Weapon), 1, activeFile);
		adresa += sizeof(Weapon);
		elementCounter++;
		
		if ((elementCounter+1) % FB == 0)
		{
			memset(&w, 0, sizeof(Weapon));
			fwrite(&w, sizeof(Weapon), 1, activeFile);
			adresa += sizeof(Weapon);
			elementCounter++;

			blockCounter++;
		}
	}

	fseek(activeFile, sizeof(int), SEEK_SET);
	while (fread(&w, sizeof(Weapon), 1, activeFile))
	{
		printf("SequZone: %d %s %.2lf\n", w.id, w.maker, w.price);
	}

	int j = (elementCounter % FB);
	if (j > 0) { //ako je nacet blok, da dopuni sa praznim slogovima
		while (j < FB) {
			memset(&w, 0, sizeof(Weapon));
			fwrite(&w, sizeof(Weapon), 1, activeFile);
			adresa += sizeof(Weapon);
			elementCounter++;
			j++;
		}
	}

	int height = ceil(log2(blockCounter));
	int nodeCount = CalculateCountOfNodes(blockCounter, height);
	IndTreeNode *tempIndexZone = (IndTreeNode *)malloc(nodeCount * sizeof(IndTreeNode));
	int indexZoneAddress = adresa;
	int tempIndexZoneAddr = 0;
	

	int i;
	int sizeOfPrimaryZone = blockCounter * FB * sizeof(Weapon);
	fseek(activeFile, 0, SEEK_SET);
	adresa = 0;
	fwrite(&sizeOfPrimaryZone, sizeof(int), 1, activeFile);
	adresa += sizeof(int);

	
	fseek(activeFile, sizeof(int), SEEK_SET);
	IndTreeNode node;	//sta ako na pocetku nemam paran broj blokova?
	int nodeCounter = 0;
	//formiramo poslednji nivo stabla, elementi koji pokazuju na same blokove, i smestamo ih na pocetku indeks zone
	for (i = 0; i < blockCounter; i += 2) { 
		//preuzmemo adresu, jer kada ga ucitamo, pokazivac se pomera
		node.addrId1 = adresa;
		if (fread(&w, sizeof(Weapon), 1, activeFile) == 0) {
			break;
		}
		adresa += sizeof(Weapon);
		node.id1 = w.id; //preuzmemo id 0 elementa bloka jer je on najmanji
		//preskocimo 4 elementa, tj. idemo na pocetak sledeceg bloka
		
		adresa += 4 * sizeof(Weapon);
		fseek(activeFile, adresa, SEEK_SET);
		
		
		node.addrId2 = adresa;
		if (fread(&w, sizeof(Weapon), 1, activeFile) == 0) {
			memset(&w, 0, sizeof(Weapon));
			node.addrId2 = 0;
		}
		adresa += sizeof(Weapon);
		node.id2 = w.id;

		//upisemo cvor stabla u indeks zonu
		memcpy(tempIndexZone + nodeCounter, &node, sizeof(IndTreeNode));
		indexZoneAddress += sizeof(IndTreeNode);
		nodeCounter++;

		adresa += 4 * sizeof(Weapon);
		fseek(activeFile, adresa, SEEK_SET);
	}

	for (int i = 0; i < nodeCounter; i++) {
		printf("%d %d | %d %d\n", tempIndexZone[i].id1, tempIndexZone[i].addrId1, tempIndexZone[i].id2, tempIndexZone[i].addrId2);
	}
	printf("========\n");

	IndTreeNode * pointer = tempIndexZone;
	IndTreeNode tempNode;
	adresa = indexZoneAddress;
	int levelNodeCount = nodeCounter;
	//formiramo ostatak stabla na osnovu formiranog poslednjeg nivoa
	for (i = 0; i < height-1; i++) {
		for (j = 0; j < levelNodeCount; j+=2 ) {
			node.addrId1= adresa;
			adresa += sizeof(IndTreeNode);
			node.id1 = pointer->id1;
			pointer++;
			printf("%d %d\n", node.addrId1, node.id1);
			
			
			if (levelNodeCount % 2 != 0 && j == levelNodeCount - 1) {
				node.id2 = 0;
				node.addrId2 = 0;
			}
			else {
				node.addrId2 = adresa;
				node.id2 = pointer->id1;
				adresa += sizeof(IndTreeNode);
				pointer++;	
			}
			tempIndexZone[nodeCounter++] = node;
			indexZoneAddress += sizeof(IndTreeNode);
			printf("%d %d\n", node.addrId2, node.id2);
		}
		levelNodeCount = (levelNodeCount / 2) + (levelNodeCount % 2);
		printf("****************\n");
	}

	free(tempIndexZone);

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
		printf("6. formiranje aktivne datoteke tako što æe se popunjavati primarna zona datoteke sa\n");
		printf("7. upis novog sloga u aktivnu datoteku direktnim unosom podataka u realnom vremenu\n");
		printf("8. traženje proizvoljnog sloga u aktivnoj datoteci i njegov prikaz zajedno sa adresom bloka i\n");
		printf("9. logièko brisanje aktuelnog sloga iz aktivne datoteke \n");
		printf("10. promenu vrednosti obeležja datum i vreme uvrštenja u ponudu u zadatom slogu iz\n");
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
		case 6: IndexSequentialFileForming();	//doadati uslov da li je datoteka otvorena
			break;
		case 7: /*Call function here to do the required operation*/
			break;
		case 8: /*Call function here to do the required operation*/
			break;
		case 9: /*Call function here to do the required operation*/
			break;
		case 10: /*Call function here to do the required operation*/
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

