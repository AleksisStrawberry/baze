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

void IndexSequentialFileForming()
{
	fseek(activeFile, sizeof(int), SEEK_SET);	//ostavljam prazno mesto za zaglavlje
	FILE *sequentialFile = fopen("sequentialFile.bin", "rb");
	Weapon w;
	Weapon weapons[FB];
	int blockCounter = 0;
	int elementCounter = 0;
	while (fread(&w, sizeof(Weapon), 1, sequentialFile))
	{
		fwrite(&w, sizeof(Weapon), 1, activeFile);
		elementCounter++;
		if (elementCounter % (FB - 1) == 0)
		{
			printf("%d\n", elementCounter);
			memset(&w, 0, sizeof(Weapon));
			fwrite(&w, sizeof(Weapon), 1, activeFile);
			elementCounter++;
			blockCounter++;
		}
	}

	int j = (elementCounter % FB);
	if (j > 0) { //ako je nacet blok, da dopuni sa praznim slogovima
		while (j < FB) {
			memset(&w, 0, sizeof(Weapon));
			fwrite(&w, sizeof(Weapon), 1, activeFile);
			elementCounter++;
			j++;
		}
	}
	FILE *indexZone = activeFile;
	FILE *startFile = activeFile;
	fseek(startFile, 0, SEEK_SET);

	int sizeOfPrimaryZone = blockCounter * FB * sizeof(Weapon);
	fseek(activeFile, 0, SEEK_SET);
	fwrite(&sizeOfPrimaryZone, sizeof(int), 1, activeFile);

	fseek(activeFile, sizeof(int), SEEK_SET);
	
	int i;
	IndTreeNode node, tempNode;	//sta ako na pocetku nemam paran broj blokova?

	//formiramo poslenji nivo stabla, elementi koji pokazuju na same blokove, i smestamo ih na pocetku indeks zone
	for (i = 0; i < blockCounter; i += 2) { 
		//preuzmemo adresu, jer kada ga ucitamo, pokazivac se pomera
		node.addrId1 = activeFile - startFile;
		fread(&w, sizeof(Weapon), 1, activeFile);
		node.id1 = w.id; //preuzmemo id 0 elementa bloka jer je on najmanji

		//preskocimo 4 elementa, tj. idemo na pocetak sledeceg bloka
		fseek(activeFile, node.addrId1 + 4 * sizeof(Weapon), SEEK_SET);
		node.addrId2 = activeFile - startFile;
		fread(&w, sizeof(Weapon), 1, activeFile);
		node.id2 = w.id;
		//upisemo cvor stabla u indeks zonu
		fwrite(&node, sizeof(IndTreeNode), 1, indexZone);
	}
	//formiramo ostatak stabla na osnovu formiranog poslednjeg nivoa
	for (i = 0; i < ceil(log2(blockCounter)); i++) {
		for (j = 0; j < blockCounter; j +=2 ) {
			node.addrId1= activeFile - startFile; 
			fread(&tempNode, sizeof(IndTreeNode), 1, activeFile);
			node.id1 = tempNode.id1 < tempNode.id2 ? tempNode.id1 : tempNode.id2;

			node.addrId2 = activeFile - startFile;
			fread(&tempNode, sizeof(IndTreeNode), 1, activeFile);
			node.id2 = tempNode.id1 < tempNode.id2 ? tempNode.id1 : tempNode.id2;
			
			fwrite(&node, sizeof(IndTreeNode), 1, indexZone);
		}
		blockCounter = (blockCounter / 2) + (blockCounter % 2);
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

