#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <time.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



typedef SYSTEMTIME DateTime;

typedef struct weapon
{
	int id;
	char maker[50];
	char mark[50];
	double price;
	DateTime date;
}Weapon;

Weapon EnterWeapon()
{	
	Weapon w;
	printf("Enter the identification number:\n");
	scanf("%d", &(w.id));
	printf("Enter the maker:\n");
	scanf("%s", w.maker);
	printf("Enter the mark:\n");
	scanf("%s", w.mark);
	printf("Enter the price:\n");
	scanf("%lf", &w.price);

	GetSystemTime(&w.date);

	return w;
}

int main()
{
		
	
		EnterWeapon();
	int choice;

	do
	{
		printf("Meni:\n\n");
		printf("1. formiranje prazne datoteke pri èemu korisnik zadaje naziv nove datoteke\n");
		printf("2. izbor aktivne datoteke zadavanjem njenog naziva\n");
		printf("3. prikaz naziva aktivne datoteke\n");
		printf("4. formiranje serijske datoteke direktnim unosom podataka u realnom vremenu\n");
		printf("5. formiranje sekvencijalne datoteke tako što æe se uèitati slogovi iz serijske datoteke u\n");
		printf("6. formiranje aktivne datoteke tako što æe se popunjavati primarna zona datoteke sa\n");
		printf("7. upis novog sloga u aktivnu datoteku direktnim unosom podataka u realnom vremenu\n");
		printf("8. traženje proizvoljnog sloga u aktivnoj datoteci i njegov prikaz zajedno sa adresom bloka i\n");
		printf("9. logièko brisanje aktuelnog sloga iz aktivne datoteke \n");
		printf("10. promenu vrednosti obeležja datum i vreme uvrštenja u ponudu u zadatom slogu iz\n");
		printf("11. Exit!\n");
		scanf_s("%d", &choice);

		switch (choice)
		{
		case 1: /*Call function here to do the required operation*/
			break;
		case 2: /*Call function here to do the required operation*/
			break;
		case 3: /*Call function here to do the required operation*/
			break;
		case 4: /*Call function here to do the required operation*/
			break;
		case 5: /*Call function here to do the required operation*/
			break;
		case 6: /*Call function here to do the required operation*/
			break;
		case 7: /*Call function here to do the required operation*/
			break;
		case 8: /*Call function here to do the required operation*/
			break;
		case 9: /*Call function here to do the required operation*/
			break;
		case 10: /*Call function here to do the required operation*/
			break;
		case 11: printf("Goodbye\n\n");
			break;
		default: printf("Wrong Choice. Enter again\n");
			break;
		}

	} while (choice != 11);

	//_getch();

	return 0;
}

