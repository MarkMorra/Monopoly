//By Mark Morra & Ben Potter
//Ms Dzeiwa
//Earth Date December 9, 2016

//Star Date 10_2 01001010101110001101
//Star Date 10_10 -306061.6176191561
//Star Date 10_12 -129151;74#2#4#028
//Star Date 10_60 -306061 Deg -37 Min -3.429 Sec

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>
#include <tchar.h>
#include <direct.h>


// define a structure to hold the game state variables
typedef struct {

	int playerLocation[4]; //saves each player's location on the game board
	int playerMoney[4]; //saves each player's money
	int playerAmount; //saves how many players there are playing
	int jailed[4]; //0 is the player is free, 1 if they're in jail
	int PropertieOwner[40]; //0 if player 1 owns the property, 1 if player 2 owns it, etc
	int squarePrices[40][7]; //stores the price of each square and the cost for each upgrade. 0 is cost to buy, 1 is rent no houses, 2 is rent with 1 house...6 is rent with hotel
	int dice[2]; //saves the dice values each roll
	int activePlayer; //stores who the current active player is (whose turn it is)
	int sqaureStatus[40]; //saves the status of each square. 0 is unowned, 1 means owned with no houses, 2 means it has 1 house...6 means it has a hotel, 7 means it is mortgaged
	int housesLeft; //stores how many houses are left availible to buy
	int hotelsLeft; //stores how many hotels are left availible to buy
	int communityChestCards[17]; //stores the order of the community chest card deck
	int chanceCards[16]; //stores the order of the chance card deck
	int communityChestDeckLocation; //saves how far through the community chest deck you are
	int chanceDeckLocation; //saves how far through the chance cards deck you are
	int communityGOOJFUsed; //says whether or not the get out of jail free card in the community chest deck is held by a player
	int chanceGOOJFUsed; //says whether or not the get out of jail free card in the chance cards deck is held by a player
	int communityGOOJF[4]; //saves which player has the get out of jail free card from the community chest deck
	int chanceGOOJF[4]; //saves which player has the get out of jail free card from the chance deck
	char playerNames[4][3]; //stores the 2 letter code that acts as the player's name
	char squareNames[45][3]; //store the 2 letter code that refers to each square
	char consoleLog[4][40][100]; //used to save what is said in the log
	int logPosition[4]; //used to save the position in the log array
	int logLength; //saves how many lines are to be displayed in the log (set in option menu)
	int timesPassedGo[4]; //tarcks how many times the players passes go
	int numOfDoubles, rollagain; //checks if the user rolled doubles and how many times in a row they rolled them
	int turnsInJail[4]; //saves how amny turns a player has been in jail
	int showBuildingAmount; //0 means prices will be displaed on board, 1 means the amount of houses/hotels will be dispalyed
	int bankrupt[4]; // 0 means they still have money or propertues unmortgaged, 1 is bankrupt (out)
	int collectRentInJail; //0 means you dont collect money in jail, 1 means you do (set in setting menu)
	int difficulty[4]; // 0 is easy, 1 is normal, 2 is hard (1 is the default)
	int winner; //if -1 no one won, 0-3 measn the respective player won

} Game;

struct Bar { //used to return multiple values when calling the getPropertiesOnStreet function

	int x; //property one
	int y; //property two
	int z; //property three (NULL when there is only two properties)

};

// this says to use "gm" as the pointer for the Game structure
Game gm;

struct Bar getPropertiesOnStreet(int property);

//define all the functions
void start(void);
void startGame(void);
int getCommand(void);
void getPlayerNames(void); 
void move(void);
void rollDice(void);
int checkSquares(void);
int landOnProperty(void);
int specialSquares(int);
void mixDecks(int);
void sortDecks(void);
void textColour(int,int);
void ownerColour(int squareNumer);
void drawBoard(void);
void saveLog(char saveLogSentence[100]);
void genFiles();
int checkFiles(char *path);
void printFrame(int frm);
void welcome();
int menu();
void calculateRail();
int preferences();
void help();
int buySellHouses(char *token1,char *token2);
int jail();
int checkIfBankrupt(int allPlayers, int bankrupter);
int bankrupt(int player, int bankrupter);
int getPropertNumber(char *proprtyName);
int morgage(char *propertyName, int unMortgage);
int exitScreen(int ingame);
void highscores();
void saveHighscores();
int saveGame();
int loadGame();
void readPrefrances();
void savePrefrances();


main () {

	int quit; //saves what the user chose to do when asked if they want to quit
	int load; //saves if the user would like to load a game or not

	memset(&gm, 0, sizeof(gm)); // initialize Game to zero
	
	//makes sure all the files are present and readable
	genFiles();

	//call the start function, which initializes a lot of the permanent game settings (window size, tile names, etc)
	start();

	//displays the welcome screen
	welcome();

	//flush keyboard input
	fflush(stdin);

	
	quit = menu(); //stores if the user wants to quit

	if (quit == 10) //returns 10 if they want to quit
	{
		return; //end function, quitting to windows
	}
	else if (quit == 11) //returns 11 if they want to return to welcome screen
	{
		main();
		return;
	}

	system("cls");

	load = loadGame(); // runs the load game function, which asks if the user wants to load a game. returns 0 if they dont (starts new game) and 1 if they do (loads specified game)

	if (load == 0)	// this is true if they start a new game
	{
		// gets player names from keyboard
		getPlayerNames();

		// sets up money, etc using user settings
		startGame();
	}

	while (gm.winner == -1) //checks if the game is over
	{
		if (gm.bankrupt[gm.activePlayer] == 0) //checks if the active player is bankrupt
		{
			if (gm.jailed[gm.activePlayer] == 1) //chacks if the active player is in jail
			{
				quit = jail(); //calls the jail function

				if (quit == 10) //checks if the user wants to quit to windows
				{
					return;
				}
				else if (quit == 11) //checks if the user wants to quit to main menu
				{
					main();
					return;
				}
			}
			else
			{
				rollDice();	// generates two random nums for dice
				if (gm.jailed[gm.activePlayer] == 1) //checks if the user got sent to jail
				{
					getCommand(); //calls get command
				}
				else
				{
					move();// moves the player based on the dice roll
					quit = checkSquares(); //check what square the user landed on
					if (quit == 10)
					{
						return;
					}
					else if (quit == 11)
					{
						main();
						return;
					}
					if (gm.winner == -1)
					{
						//drawBoard();
						quit = getCommand(); //calls get command
						if (quit == 10)
						{
							return;
						}
						else if (quit == 11)
						{
							main();
							return;
						}
					}

				}
				
			}
		}
		else
		{
			gm.activePlayer++; // if the player is bankrupt it skips there turn
			if (gm.activePlayer == gm.playerAmount) {gm.activePlayer = 0;} //if active player is grater then player amount it resets it to zero
		}
	}
	quit = win(); //calls the win function if the game is over

	if (quit == 10)
	{
		return;
	}
	else if (quit == 11)
	{
		main();
		return;
	}
}

int jail() { //runs when the player is in jail

	char log[100]; //saves strings to be printed using saveLog
	char command[100]; //saves command entered by user
	char *token[3]; //used to divide command into seperate arguments
	char temp[2]; //used when converting ints to alphanumeric
	int i; //loop control var
	int returnValue; //saves the value returned by the buildings function
	int turnIsOver = 0; //whether or not the users turn is over
	int quit = 0; //saves whether or not the user wants to quit

	
	
	do
	{
		strcpy(log, gm.playerNames[gm.activePlayer]); //creats the string to tell the user they are in jail
		strcat(log, ", you are in jail. Enter a command (type help for help): ");
		saveLog(log); //prints the string
		printf("> "); //asks user to input command
		fflush(stdin);
		gets(command);
		strcpy(log, "> "); //creats the string to print what the user said
		strcat(log, command);
		strupr(command); //converts command to upper case
		saveLog(log); //prints the string


		token[0] = strtok(command, " "); //gets the first token (first word user enters)
		i = 1;
		do
		{
			token[i] = strtok(NULL, " "); //gets subsiqant tokens (words)
			i++;

		} while (!(token[i - 1] == NULL || i == 3)); //ends loop then there are three words (max arguments) or there are no more words

		if (token[0] == NULL) { token[0] = " "; } //sets null tokens to spaces to avoid errors
		if (token[1] == NULL) { token[1] = " "; }
		if (token[2] == NULL) { token[2] = " "; }

		if (!((strcmp(token[0], "ROLL") == 0) || (strcmp(token[0], "GOOJF") == 0) || (strcmp(token[0], "PAY") == 0) || (strcmp(token[0], "HELP") == 0) || (strcmp(token[0], "BUILDINGS") == 0) || (strcmp(token[0], "QUIT") == 0) || (strcmp(token[0], "END") == 0) || (strcmp(token[0], "MORTGAGE") == 0) || (strcmp(token[0], "UNMORTGAGE") == 0))) //error traps the users input
		{
			strcpy(log, "Error, unknown command "); //creates error msg string
			strcat(log, token[0]);
			saveLog(log); //prints the string
		}

		if (strcmp(token[0], "HELP") == 0) //displays commands
		{
			saveLog("HELP - shows help, END - end your turn"); //prints help
			saveLog("GOOJF - use get out of jail free card");
			saveLog("ROLL - roll the dice to try and get out");
			saveLog("BUILDINGS - building options, type BUILDINGS for more help");
			saveLog("MORTGAGE/UNMORTGAGE - mortgage/unmortgage a property");
			saveLog("PAY - pay $50 and get out of jail");
			saveLog("QUIT - quits the game");
		}
		else if (strcmp(token[0], "BUILDINGS") == 0) //checks if the user says buildings
		{
			returnValue = buySellHouses(token[1], token[2]); //calls buy sell function passes through what the user said
			if (returnValue == 0) //0 means invalid argument
			{
				strcpy(log, "Error, invalid argument "); //creates error message
				strcat(log, token[1]);
				saveLog(log); //prints error
				saveLog("BUILDINGS [BUY/SELL/PRICE] [Property NAME (eg: OA)]"); //displays syntax
				saveLog("Used to buy or sell houses/hotels on a property you own");
				saveLog("Buying a house on a property with four houses will build a hotel");
			}
		}
		else if (strcmp(token[0], "GOOJF") == 0) //check if user says GOOJF (get out of jail free)
		{
			if (gm.chanceGOOJF[gm.activePlayer] == 1) //checks if they have the chance goojf card
			{
				gm.chanceGOOJF[gm.activePlayer] = 0; //removes player's card
				gm.chanceGOOJFUsed = 0; //marks the card as unused
				gm.jailed[gm.activePlayer] = 0; //gets player out of jail
				gm.playerLocation[gm.activePlayer] = 10; //moves them to just visiting
				turnIsOver = 1; //ends turn
				strcpy(log, gm.playerNames[gm.activePlayer]); //creates message to be displayed
				strcat(log, ", you are now free from jail");
				saveLog(log); //prints message
				gm.turnsInJail[gm.activePlayer] = 0; //resets turns in jail counter
				getCommand(); //calls get command
				return;
			}
			else if (gm.communityGOOJF[gm.activePlayer] == 1) //checks if they have the community chest goojf card
			{
				gm.communityGOOJF[gm.activePlayer] = 0;//removes player's card
				gm.communityGOOJFUsed = 0; //marks the card as unused
				gm.jailed[gm.activePlayer] = 0; //gets player out of jail
				gm.playerLocation[gm.activePlayer] = 10; //moves them to just visiting
				turnIsOver = 1; //ends turn
				strcpy(log, gm.playerNames[gm.activePlayer]); //creates message to be displayed
				strcat(log, ", you are now free from jail");
				saveLog(log); //prints message
				gm.turnsInJail[gm.activePlayer] = 0; //resets turns in jail counter
				getCommand();  //calls get command
				return;
			}
			else
			{
				saveLog("You don't have a GOOJF card"); //prints error if they dont have a goojf card
			}
		}
		else if (strcmp(token[0], "ROLL") == 0) //checks if the user said to roll
		{
			turnIsOver = 1; //marks their turn as over
			gm.dice[0] = rand() % 6 + 1; //generates dice
			gm.dice[1] = rand() % 6 + 1;

			if (gm.dice[0] == gm.dice[1]) //checks if they roll doubles
			{
				strcpy(log,"You rolled double "); //creates message
				strcat(log, itoa(gm.dice[0], temp , 10));
				strcat(log, "'s and got out of jail");
				saveLog(log); //prints message
				gm.jailed[gm.activePlayer] = 0; // sets the player as free
				gm.playerLocation[gm.activePlayer] = 10; //moves them to just visiting
				gm.turnsInJail[gm.activePlayer] = 0; //resets turns in jail counter
				getCommand(); //calls get command
				return;
			}
			else
			{
				strcpy(log, gm.playerNames[gm.activePlayer]); //creates message
				strcat(log, ", you did not roll doubles");
				saveLog(log); //prints that they didnt roll doubles
				gm.turnsInJail[gm.activePlayer]++; //increments turns in jail
				if (gm.turnsInJail[gm.activePlayer] == 3) //checks if they spent more than 3 turns
				{
					saveLog("You have spent 3 turns and now must pay a $50 fine"); //prints message
					saveLog("Press ENTER to pay your fine...");
					while (getchar() != '\n');
					gm.playerMoney[gm.activePlayer] -= 50; //removes money
					quit = checkIfBankrupt(0, -1); //calls check if bankrupt
					if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
					turnIsOver = 1; //end there turn
					gm.jailed[gm.activePlayer] = 0; //releases them from jail
					gm.playerLocation[gm.activePlayer] = 10; //moves them 
					gm.turnsInJail[gm.activePlayer] = 0; //resets turns in jail counter
					getCommand(); //calls get command
					return;
				}
				getCommand(); //calls get command
			}
		}
		else if (strcmp(token[0], "MORTGAGE") == 0) //checks if they said mortgage
		{
			morgage(token[1], 0); //calls mortgage with the property entered
		}
		else if (strcmp(token[0], "UNMORTGAGE") == 0) //checks if they said unmortgage
		{
			morgage(token[1], 1); //calls unmortgage with property entered
		}
		else if (strcmp(token[0], "PAY") == 0) //checks if they said unmortgage
		{
			if (gm.playerMoney[gm.activePlayer] < 50)
			{
				strcpy(log, gm.playerNames[gm.activePlayer]);
				strcat(log, ", you don't have enough money");
				saveLog(log);
			}
			else
			{
				gm.playerMoney[gm.activePlayer] -= 50; //removes money
				turnIsOver = 1; //ends there turn
				gm.jailed[gm.activePlayer] = 0; //releases them from jail
				gm.playerLocation[gm.activePlayer] = 10; //moves them 
				gm.turnsInJail[gm.activePlayer] = 0; //resets turns in jail counter
				getCommand(); //calls get command
				strcpy(log, gm.playerNames[gm.activePlayer]);
				strcat(log, ", you are out of jail!");
				saveLog(log);
			}
		}
		else if (strcmp(token[0], "QUIT") == 0) //checks if they said quit
		{
			quit = exitScreen(1); //calls exitscreen and returns choice
			if (quit == 10 || quit == 11) { return quit; } //checks if they want to quit and returns the value
		}
	} while (turnIsOver != 1); //checks if the turn is over

	return 1;

}

void welcome() { //displays the welcome screen

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 1400, 900, TRUE);    // sets console size and pos

	SetConsoleTitle(_T("Monopoly - Mark and Ben")); //sets the console title

	while (!kbhit()) { //checks if a key has been pressed without stopping the program

		printFrame(3); //calls printframe with the frame to print
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
		if (kbhit()) { break; } //breaks if a key has been pressed
		printFrame(2); //calls printframe with the frame to print
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
		printFrame(1); //calls printframe with the frame to print
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
		printFrame(2); //calls printframe with the frame to print
		if (kbhit()) { break; } //breaks if a key has been pressed
		Sleep(500); //sleeps for half a second
	}

	fflush(stdin); //flushes input stream

	getch(); //eats carriage return character

}

void printFrame(int frm) { //prints animation frames for the welcome screen

	FILE *frame; //file pointer for keyframes
	char temp[200]; //stores the frame to print it out

	system("cls"); //clears screen

	switch (frm) { //frm saves which file to print
	case 1:
		frame = fopen("Monopoly_Data/hatup.ani", "r"); //opens file
		if (frame == NULL)
		{
			printf("Error reading frame 1"); //prints error
			return; //ends function
		}
		while (fgets(temp, 200, frame)) { printf(temp); } //prints file
		break;
	case 2:
		frame = fopen("Monopoly_Data/hatmid.ani", "r"); //opens file
		if (frame == NULL)
		{
			printf("Error reading frame 2"); //prints error
			return; //ends function
		}
		while (fgets(temp, 200, frame)) { printf(temp); } //prints file
		break;
	case 3:
	default:
		frame = fopen("Monopoly_Data/hatdown.ani", "r"); //opens file
		if (frame == NULL)
		{
			printf("Error reading frame 3"); //prints error
			return; //ends function
		}
		while (fgets(temp, 200, frame)) { printf(temp); } //prints file
		break;
	}

	fclose(frame); //closes file

}

int menu() { //displays menu and gets user decision

	HWND console = GetConsoleWindow();
	RECT r;
	int selection; //what menu option the user chooses
	int quit; //whether or not the user would like to quit


	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 1400, 900, TRUE);    // sets console size and pos

	SetConsoleTitle(_T("Monopoly - Mark and Ben")); //sets the console title
	
	system("cls");

	//displays menu

	printf("\n\n\n\n\n\t\t\t\t\t\t\t\t     __       __                               \n\t\t\t\t\t\t\t\t    /  \\     /  |                              \n\t\t\t\t\t\t\t\t    $$  \\   /$$ |  ______   _______   __    __ \n\t\t\t\t\t\t\t\t    $$$  \\ /$$$ | /      \\ /       \\ /  |  /  |\n\t\t\t\t\t\t\t\t    $$$$  /$$$$ |/$$$$$$  |$$$$$$$  |$$ |  $$ |\n\t\t\t\t\t\t\t\t    $$ $$ $$/$$ |$$    $$ |$$ |  $$ |$$ |  $$ |\n\t\t\t\t\t\t\t\t    $$ |$$$/ $$ |$$$$$$$$/ $$ |  $$ |$$ \\__$$ |\n\t\t\t\t\t\t\t\t    $$ | $/  $$ |$$       |$$ |  $$ |$$    $$/ \n\t\t\t\t\t\t\t\t    $$/      $$/  $$$$$$$/ $$/   $$/  $$$$$$/  \n");

	printf("\n\n\n\t\t\t\t\t\t\t\t\t\t1: Play Monopoly!\n\n\n\t\t\t\t\t\t\t\t\t\t2: Game Options\n\n\n\t\t\t\t\t\t\t\t\t\t3: Help\n\n\n\t\t\t\t\t\t\t\t\t\t4: Highscores\n\n\n\t\t\t\t\t\t\t\t\t\t5: View Welcome Screen\n\n\n\t\t\t\t\t\t\t\t\t\t6: Exit\n\n\n\n");

	do { //loops until valid input

		fflush(stdin);

		selection = getch(); //gets user selection

	} while (!(selection == 49 || selection == 50 || selection == 51 || selection == 52 || selection == 53 || selection == 54));

	switch (selection) {
	case 49:
		return 1; //exits function and continues to game
	case 50:
		preferences(); //calls options screen
		savePrefrances(); //saves user preferences to a file
		quit = menu(); //recalls menu screen
		if (quit == 10 || quit == 11) //if they want to quit returns value 
		{
			return quit;
		}
		return 1;
	case 51:
		help(); //calls help screen
		quit = menu(); //recalls menu screen
		if (quit == 10 || quit == 11) //if they want to quit returns value 
		{
			return quit;
		}
		return 1;
		break;
	case 52:
		highscores(); //calls highscore screen
		quit = menu(); //recalls menu screen
		if (quit == 10 || quit == 11) //if they want to quit returns value 
		{
			return quit;
		}
		return 1;
	case 53:
		main(); //calls main to go back to welcome screen
		return 10;
	case 54:
		quit = exitScreen(0); //calls exit screen
		if (quit == 10 || quit == 11) //if they want to quit returns value 
		{
			return quit;
		}
		return 1;
	}
}

void highscores() { //shows highscores screen

	char names[10][3]; //saves the names of top 10 players temporarily
	char choice = ' '; //saves what menu option the user chooses
	int score[10]; //saves scores of the top 10 players temporarily
	int money[10]; //saves the money values of the top 10 players temporarily
	int i; //loop control var
	FILE *file; //file pointer to highscores file

	system("cls"); //clears screen

	//prints ascii art
	printf("  _    _ _       _                                 \n | |  | (_)     | |                                \n | |__| |_  __ _| |__  ___  ___ ___  _ __ ___  ___ \n |  __  | |/ _` | '_ \\/ __|/ __/ _ \\| '__/ _ \\/ __|\n | |  | | | (_| | | | \\__ \\ (_| (_) | | |  __/\\__ \\\n |_|  |_|_|\\__, |_| |_|___/\\___\\___/|_|  \\___||___/\n            __/ |                                  \n           |___/                                   \n");
	
	//prints header
	printf("\t\t\tNAME\tMONEY\tSCORE\n");
	
	file = fopen("Monopoly_Data/highscores.sav", "r"); //opens highscores file

	if (file == NULL)
	{
	printf("Error reading file, try regening it in prefernces"); //print error msg if the file could not be read
	}
	else
	{
		for (i = 0; i < 10; i++) //loops through the file and reads all the data
		{
			fscanf(file, "%[^~]~%d~%d~", names[i], &money[i], &score[i]); //reads values
			if (!((strcmp(names[i], "NA") == 0) && (money[i] == 0) && (score[i] == 0))) //checks to make sure the entry is not blank
			{
				printf("\t\t\t%s\t%6d\t%6d\n", names[i], money[i], score[i]); //prints the users name, money and score
			}
		}

		fclose(file); //closes the file
	}

	printf("\n1: Return to menu\n\n2: Clear highscores"); //prints promt for the user to press a key

	do {
		
		fflush(stdin); 
		choice = getch(); //gets the users choice

	} while (!(choice == '1' || choice == '2')); //loops untill they press a valid key

	switch (choice)
	{
	case '1':
		return;
	case '2':
		_mkdir("Monopoly_Data");
		_mkdir("Monopoly_Data/saves"); //makes sure the Monopoly_Data directory exists

		file = fopen("Monopoly_Data/highscores.sav", "w"); //opens file
		//writes data to file
		fprintf(file, "NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~"); //creates blank file
		fclose(file); //close file

		system("cls"); //clears screen

		highscores(); //recalls highscore function
		return;
	}
}

int preferences() { //shows the preferences screen

	FILE *file; //file pointer for settings file
	int temp; //saves the log length entered by the user after its converted to an integer, error traps it, then loads it into the variable used in game
	int i; //loop control var
	char choice; //saves user menu choice
	char length[5]; //used in the gets function for getting the log length

	system("cls"); //clears screen

	readPrefrances(); //calls readPrefrances

	//prints ascii art
	printf("\n\n                                          ,ad8888ba,    88888888ba  888888888888  88    ,ad8888ba,    888b      88   ad88888ba   \n                                         d8\"'    `\"8b   88      \"8b      88       88   d8\"'    `\"8b   8888b     88  d8\"     \"8b  \n                                        d8'        `8b  88      ,8P      88       88  d8'        `8b  88 `8b    88  Y8, \n                                        88          88  88aaaaaa8P'      88       88  88          88  88  `8b   88  `Y8aaaaa,    \n                                        88          88  88\"\"\"\"\"\"'        88       88  88          88  88   `8b  88    `\"\"\"\"\"8b,  \n                                        Y8,        ,8P  88               88       88  Y8,        ,8P  88    `8b 88          `8b  \n                                         Y8a.    .a8P   88               88       88   Y8a.    .a8P   88     `8888  Y8a     a8P  \n                                          `\"Y8888Y\"'    88               88       88    `\"Y8888Y\"'    88      `888   \"Y88888P\"   \n\n\n");

	//creats promt for user
	printf("                                                                      1: Change log length\n\n                                                                      2: Change difficulty\n\n                                                                      3: Collect money in jail - Currently ");
	printf((gm.collectRentInJail == 0) ? ("OFF") : ("ON")); //displays on or off depending if collect rent in jail is true or flase
	printf("\n\n                                                                      4: Regenerate files\n\n                                                                      5: Back to menu");
	
	do {

		fflush(stdin);

		choice = getch(); //gets input

	} while (!(choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == '5')); //loops till the user press a valid key

	switch (choice)
	{
	case '1':
		system("cls"); //cleras screen
		do {
			printf("Enter how many lines to show in the console below the game (8-40): "); //gives promt
			gets(length); //gets desired log length
			
			temp = atoi(length); //changes input from keyboard to an int

			if (temp > 40 || temp < 8) //error traps
			{
				temp = 0;
				printf("\nError, you can't make the log that length. 20 is recommended.\n"); //displayes error msg
			}
		} while (temp == 0); //loops till the user enters a vlid number

		gm.logLength = temp; //sets what the user enterd as the new log length

		savePrefrances(); //calls save prefrances
		preferences(); //calls prefrances
		return;
	case '2':
		do
		{
			system("cls");

			printf("Current difficulties:\n"); //displays the current difculty of each player
			for (i = 0; i < 4; i++)
			{
				printf("P%d - ", (i + 1));
				switch (gm.difficulty[i])
				{
				case 0:
					printf("Easy\n");
					break;
				default:
				case 1:
					printf("Normal\n");
					break;
				case 2:
					printf("Hard\n");
					break;
				}
			}

			printf("\n1: Change difficulties\n\n2: Return to preferences"); //promts user

			fflush(stdin);
			choice = getch();

		} while (!(choice == '1' || choice == '2')); //loops till user presses a vlid key
		
		switch (choice)
		{
		case '1':
			for (i = 0; i < 4; i++)
			{
				system("cls");
				printf("Select difficulty for P%d\n\n1: Easy - Start with $3000\n2: Normal - Start with $1500, Classic Monopoly\n3: Hard - Start with $1000", (i + 1)); //promts user to select dificulty level for players
				do {

					fflush(stdin);
					choice = getch(); //gets input

				} while (!(choice == '1' || choice == '2' || choice == '3')); //loops till the user presses a vlid key

				switch (choice) //sets the difficluty according to what the user entered
				{
				case '1':
					gm.difficulty[i] = 0;
					break;
				default:
				case '2':
					gm.difficulty[i] = 1;
					break;
				case '3':
					gm.difficulty[i] = 2;
					break;
				}
			}
		case 2:
			break;
		}

		savePrefrances();
		preferences();
		return;
	case '3':
		(gm.collectRentInJail == 0) ? (gm.collectRentInJail = 1) : (gm.collectRentInJail = 0);
		savePrefrances();
		preferences();
		return;
	case '4':
		system("cls");
		printf("1: Regenerate most files, but keep highscores\n2: Regenerate ALL files\n3: Cancel");
		choice = _getch();

		switch (choice)
		{
		case '2':
			_mkdir("Monopoly_Data");
			_mkdir("Monopoly_Data/saves");

			file = fopen("Monopoly_Data/highscores.sav", "w"); //opens file
			//writes data to file
			fprintf(file, "NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~");
			fclose(file); //close file
		case '1':

			system("cls");

			printf("Regenerating...");
			
			_mkdir("Monopoly_Data");
			_mkdir("Monopoly_Data/saves");

			file = fopen("Monopoly_Data/propertyValues.csv", "w"); //opens file
			//writes data to file
			fprintf(file, "0,60,0,60,0,200,100,0,100,120,0,140,150,160,160,200,180,0,180,200,0,220,0,220,240,200,260,260,150,280,0,300,300,0,320,200,0,350,0,400,-1,0,2,0,4,0,0,6,0,6,8,0,10,0,10,12,0,14,0,14,16,0,18,0,18,20,0,22,22,0,24,0,26,26,0,28,0,0,35,0,50,-1,0,10,0,20,0,0,30,0,30,40,0,50,0,50,60,0,70,0,70,80,0,90,0,90,100,0,110,110,0,120,0,130,130,0,150,0,0,175,0,200,-1,0,30,0,60,0,0,90,0,90,100,0,150,0,150,200,0,200,0,200,220,0,250,0,250,300,0,330,330,0,360,0,390,390,0,450,0,0,500,0,600,-1,0,90,0,180,0,0,270,0,270,300,0,450,0,450,500,0,550,0,550,600,0,700,0,700,750,0,800,800,0,850,0,900,900,0,1000,0,0,1100,0,1400,-1,0,160,0,320,0,0,400,0,400,450,0,625,0,625,700,0,750,0,750,800,0,875,0,875,925,0,975,975,0,1025,0,1100,1100,0,1200,0,0,1300,0,1700,-1,0,250,0,450,0,0,550,0,550,600,0,750,0,750,900,0,950,0,950,1000,0,1050,0,1050,1100,0,1150,1150,0,1200,0,1275,1275,0,1400,0,0,1500,0,2000,-2");
			fclose(file); //close file

			file = fopen("Monopoly_Data/hatup.ani", "w"); //opens file
			//writes data to file
			fprintf(file, "         .----------------.  .----------------.  .-----------------. .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n        | | ____    ____ | || |     ____     | || | ____  _____  | || |     ____     | || |   ______     | || |     ____     | || |   _____      | || |  ____  ____  | |\n        | ||_   \\  /   _|| || |   .'    `.   | || ||_   \\|_   _| | || |   .'    `.   | || |  |_   __ \\   | || |   .'    `.   | || |  |_   _|     | || | |_  _||_  _| | |\n        | |  |   \\/   |  | || |  /  .--.  \\  | || |  |   \\ | |   | || |  /  .--.  \\  | || |    | |__) |  | || |  /  .--.  \\  | || |    | |       | || |   \\ \\  / /   | |\n        | |  | |\\  /| |  | || |  | |    | |  | || |  | |\\ \\| |   | || |  | |    | |  | || |    |  ___/   | || |  | |    | |  | || |    | |   _   | || |    \\ \\/ /    | |\n        | | _| |_\\/_| |_ | || |  \\  `--'  /  | || | _| |_\\   |_  | || |  \\  `--'  /  | || |   _| |_      | || |  \\  `--'  /  | || |   _| |__/ |  | || |    _|  |_    | |\n        | ||_____||_____|| || |   `.____.'   | || ||_____|\\____| | || |   `.____.'   | || |  |_____|     | || |   `.____.'   | || |  |________|  | || |   |______|   | |\n        | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n         '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n                                                                                      ./yhdh:                                                                       \n                                                                                   `/ymmdyyhds.                                                                     \n                                                                                 .odmmmmmmhyydd/                                                                    \n                                                                               .ommmmmmmmmmmhyhdy-    ```                                                           \n                                                                             .ohshmmmmmmmmmmmdyhddy+oyhhhhs-                                                        \n                                                                            +hsooosdmmmmmmmmmhyhoymhyyyyyyhd-                                                       \n                                                                          -yyooooooosdmmmmdyydmydhyyyyyyyyym/                                                       \n                                                                         +dsoooooooooosddyymmdhdyyyhmmmyyydh.                                                       \n                                                                        smsooooooooooohhohmmhhhyyhmmmmdyyhd:                                                        \n                                                                       :mmmhsoooooooydsoooshhhyyhmmmmdyyhd/                                                         \n                                                                       .ymmmmdysoosdyoooooohhyydmmmdhyydh-                                                          \n                                                                         ./ymmmmhydsoooooohhyydmmdhyyddo`                                                           \n                                                                           `-odmmdhyoooooyhyyddhyyhdh+.                                                             \n                                                                   `..`       ./hmmmmysoyhyydyyhhyo:`  ````                                                         \n                                                                   `h/+o:````````/dmmmmhhyhdhso/.`./osooooooo++:.`                                                  \n                                                                 ...os `+h+/oh-:++oymmdyyhh:`  .+so/.``     ``.-+o+-                                                \n                                                                /o.-om:  s` `-++++oyhyyhds.  .os/`               `-oo-                                              \n                                                                `+s` +h  :`+/-.omhhhhhho-   :y:                     -so`                                            \n                                                                 `yo  /`    .://:/+o/-`    :y.  `         /oo+/`    `.sy.                                           \n                                                                  /m`   ``       -y-      .h. :o++/`     `-`  ``   .+++hs`                                          \n                                                                  `d/   `:+/`    ho       +o  .   `                 -:-:h+:                                         \n                                                                   oh`     .-   /m:      `h:    ``        `os/         ```/o                                        \n                                                                   `hs        `/h/       `d:   :o+`        ```            -h                                        \n                                                                    .ss.    -/+do        `h/   `   .-                   `.yo                                        \n                                                                    `-sho+++:.`yh:        +s       ++.`.::::.```-:/     hy/`                                        \n                                                                    .-o-..`.:ohmmd+`      .h:  .//:.//.```.-:+++:.o`   -my:`                                        \n                                                                    ./s+oyhdmmmmmmmy:`    /sh//:.                .s   `ymmmho:`                                     \n                                                                     smmmmmmmmmmmmmmmhs+.`o+.`       .:.`      ./o`  `smmmmmmmho:.`                ``````           \n                                                                     `ymmmmmmmmmmmmmmmmmds+oo/-.``.-+o-:+o+++oo+-  `:hmmmmmmmmmmmdhyyo:`         -oooo+/+o++/+-`    \n                                                                      .ymmmmmmmmmmmmmmmmmmmddddddho:.     ````   ./ymmmmmmmmmmmmmmmmmmmhs+/::::/+y. `` .``..../s:   \n                                                                       `+dmmmmmmmmmmmmmmmmmmmmmmmmmho/--.....--/oommmmmmmmmmmmmmmmmmmmmmmmmmmmmmm: .`  -o-.s. -.h/  \n                                                                         -ymmmmmmmmmmmmmmmmmmmmmmmmmmmy:dyyyyyyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd`-sy+` `y .s o`+h` \n                                                                          `:ymmmmmmmmmmmmmmmmmmmmmmmmmd.dyyyyhyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmsysoho  o  o : sh` \n                                                                            `:sdmmmmmmmmmmmmmmmmmmmmmmm:ss:``:oh+/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmho+osyd` -  ` `/y-  \n                                                                               ./ydmmmmmmmmmmmmmmmmmmmmo`  `/` ``-Nmmmmmmmmmmmmmmmmmmmmmmmmmmmmdoo+yh.ys` .:+yy+`   \n                                                                                  .:+shdmmmmmmmmmmmmmmmh   `+`   `mmmmmmmmmmmmdddmmmmmmmmmmmmmmy+oomh+yyyohyodo     \n                                                                                      `.-/+osyyhdmmmmmmd`   ``    ymmmmmmmmmmmmmddhhhhdddddddmmh++yh:-``-yd+om/     \n                                                                                               `-dmmmmmm`   s/    /mmmmmmmmmmmmmmmmmd/`......-:ssyy-     od+om/     \n                                                                                                 smmmmmm.         `mmmmmmmmmmmmmmmmmm+          ``       oh+om/     \n                                                                                                 /mmmmmm`   -+     ommmmmmmmmmmmmmmmmo                   oh+om/     \n                                                                                                 /mmmmmd    .:     `dmmmmmmmmmmmmmmmmo                   oyoom:     \n          ___                                            _                                      .ymmmmmh    `.      :mmmmmmmmmmmmmmmmy`                  syosd.     \n         |  _ \\  _ __ ___  ___ ___    __ _ _ __  _   _  | | _____ _   _                         -hmmmmmmo. .sys:`  `:hmmmmmmmmmmmmmmmd-                 `ysosd`     \n         | |_| || '__/ _ \\/ __/ __|  / _` | '_ \\| | | | | |/ / _ \\ | | |                       `/mmmmmmyshshsoshyo/yhssmmmmmmmmmmmmmmmo                 `hsoyd`     \n         |  __/ | | |  __/\\__ \\__ \\ | (_| | | | | |_| | |   <  __/ |_| |_ _ _                  -hmmmmmyoooooooooosysooosdmmmmmmmmmmmmmd:                `dsoyd`     \n         |_|    |_|  \\___||___/___/  \\__,_|_| |_|\\__, | |_|\\_\\___|\\__, (_|_|_)               `:ymmmmmyoooooooooooooooooooydmmmmmmmmmmmmy`               `do+yd`     \n                                                 |___/            |___/                      :hmmmdmdooooooooosooooooooooooyhmmmmmmmmmmms`              `do+hy`     \n                                                                                             `.--:ymsooooooooyyoooooooooooooooyhddmmmmmmm+              `do+hs      \n                                                                                                 -dyooooooooomoooooooooooooooooooodmo/:::.              `do+ds      \n                                                                                                `ydooooooooohdooooooooooooooooooosmy`                   -d++ds      \n                                                                                                /myooooooooomhooooooooooooooooooodd.                    :d++ds      \n                                                                                                ymoooooooooymyoooooooooooooooooodd/                     :d++m+      \n                                                                                               `hdoooooooooyNyoooooooooooooooosdd/                      :d+om:      \n                                                                                               .mdoooooooooymyooooooooooooooooshdy:                     :d+om:      \n                                                                                               .mmooooooooosmhooooooooooooooooooodd`                    /h+om:      \n");
			fclose(file); //closes file

			file = fopen("Monopoly_Data/hatmid.ani", "w"); //opens file
			//writes data to file
			fprintf(file, "         .----------------.  .----------------.  .-----------------. .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n        | | ____    ____ | || |     ____     | || | ____  _____  | || |     ____     | || |   ______     | || |     ____     | || |   _____      | || |  ____  ____  | |\n        | ||_   \\  /   _|| || |   .'    `.   | || ||_   \\|_   _| | || |   .'    `.   | || |  |_   __ \\   | || |   .'    `.   | || |  |_   _|     | || | |_  _||_  _| | |\n        | |  |   \\/   |  | || |  /  .--.  \\  | || |  |   \\ | |   | || |  /  .--.  \\  | || |    | |__) |  | || |  /  .--.  \\  | || |    | |       | || |   \\ \\  / /   | |\n        | |  | |\\  /| |  | || |  | |    | |  | || |  | |\\ \\| |   | || |  | |    | |  | || |    |  ___/   | || |  | |    | |  | || |    | |   _   | || |    \\ \\/ /    | |\n        | | _| |_\\/_| |_ | || |  \\  `--'  /  | || | _| |_\\   |_  | || |  \\  `--'  /  | || |   _| |_      | || |  \\  `--'  /  | || |   _| |__/ |  | || |    _|  |_    | |\n        | ||_____||_____|| || |   `.____.'   | || ||_____|\\____| | || |   `.____.'   | || |  |_____|     | || |   `.____.'   | || |  |________|  | || |   |______|   | |\n        | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n         '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n                                                                                                                                                                    \n                                                                                         `.:+oso:`                                                                  \n                                                                                      `-+ydmmyyhmo.                                                                 \n                                                                                    .+ymmmmmmmhyhms-                                                                \n                                                                                 `:shmmmmmmmmmmhyymh:                                                               \n                                                                               .+yhsohmmmmmmmmmmdyydd+. ``..`                                                       \n                                                                             .+hyoooooymmmmmmmmmmdyhydysyhhhys:`                                                    \n                                                                           .+hhooooooooohmmmmmdhyhdshdhyyyyyyhd/`                                                   \n                                                                          :ydsooooooooooosmmyyhmmhhhyyyyyyyyyymo`                                                   \n                                                                         -ymmhsoooooooooyhysdmmhhhyyyhdmmmyyydd:                                                    \n                                                                         .smmmmhsoooooyhysoosdhhhyyhdmmmmhyyhh/`                                                    \n                                                                          `.odmmmhsoydysoooooydyyydmmmmdyyydy-                                                      \n                                                                            `.odmmmdhsooooooydyyhmmmmdyyydh:`                                                       \n                                                                    `-:-`     `.smmhdyooooohhyydmddhyyhdh/.` `                                                      \n                                                                    .y+/o/`   ` `-ymmmdsoshhyhdyyyyhddysso+++//:.`                                                  \n                                                                 /+/+sy `os///+::-`+mmmmhhyyddhhdhs/.``     ``.-++/.                                                \n                                                                `os`.sm. `s``-s+::++ymmdyyhs/+ys/`               `-++.`                                             \n                                                                 -so `y/  /-/` .shsyhhyyhs-`/y:                     -s+`                                            \n                                                                 `od` .:  ` :o+ohyhhyhy+- `/y.  `         /oo+/`    `.so`                                           \n                                                                 `+m.   `     ````//..`   :h. :o++/`     `-`  ``   .+++y+`                                          \n                                                                  /m-   ./+-    `oo`     .oo  .   `                 -:-:h::`                                        \n                                                                  :ho     `+`   +d`      -h:    ``        `os/         ```/+`                                       \n                                                                  `od.        `+h:`      -d:   :o+`        ```            -y`                                       \n                                                                   .sy-     -/sy-`       -h/   `   .-                   `.y/`                                       \n                                                                   `/oo/++++:.`yy-       .os       ++.`.::::.```-:/     hs-`                                        \n                                                                   ..-o-..`.:ohmmd:`      :h:  .//:.//.```.-:+++:.o`   -m+-`                                        \n                                                                   ``:s+oyhdmmmmmmms-`   `+sh//:.                .s   `ymmdy+-`                                     \n                                                                    -ymmmmmmmmmmmmmmmhs+.`o+.`       .:.`      ./o`  `smmmmmmmho:.`                ``````           \n                                                                     :hmmmmmmmmmmmmmmmmmds+oo/-.``.-+o-:+o+++oo+-  `:hmmmmmmmmmmmdhyyo:`         -oooo+/+o++/+-`    \n                                                                      :ymmmmmmmmmmmmmmmmmmmddddddho:.     ````   ./ymmmmmmmmmmmmmmmmmmmhs+/::::/+y. `` .``..../s:   \n                                                                       -smmmmmmmmmmmmmmmmmmmmmmmmmmho/--.....--/oommmmmmmmmmmmmmmmmmmmmmmmmmmmmmm: .`  -o-.s. -.h/  \n                                                                        `/hmmmmmmmmmmmmmmmmmmmmmmmmmmmy:dyyyyyyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd`-sy+` `y .s o`+h` \n                                                                          .+hmmmmmmmmmmmmmmmmmmmmmmmmmd.dyyyyhyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmsysoho  o  o : sh` \n                                                                            .+ydmmmmmmmmmmmmmmmmmmmmmmm:ss:``:oh+/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmho+osyd` -  ` `/h-  \n                                                                              `:oydmmmmmmmmmmmmmmmmmmmmo`  `/` ``-Nmmmmmmmmmmmmmmmmmmmmmmmmmmmmdoo+yh.ys` .:+yy+`   \n                                                                                 `-/oyddmmmmmmmmmmmmmmmh   `+`   `mmmmmmmmmmmmdddmmmmmmmmmmmmmmy+oomh+yyyohyodo     \n                                                                                     `.-/+osyyhhdmmmmmmd`   ``    ymmmmmmmmmmmmmddhhhhdddddddmmh++yh:-``-yd+om/     \n                                                                                           ````./dmmmmmm`   s/    /mmmmmmmmmmmmmmmmmd/`......-:ssyy-     od+om/     \n                                                                                                .ymmmmmm.         `mmmmmmmmmmmmmmmmmm+          ``       oh+om/     \n                                                                                                .ommmmmm`   -+     ommmmmmmmmmmmmmmmmo                   oh+om/     \n                                                                                                .ommmmmd    .:     `dmmmmmmmmmmmmmmmmo                   oyoom:     \n          ___                                            _                                      .ymmmmmh    `.      :mmmmmmmmmmmmmmmmy`                  syosd.     \n         |  _ \\  _ __ ___  ___ ___    __ _ _ __  _   _  | | _____ _   _                         -hmmmmmmo. .sys:`  `:hmmmmmmmmmmmmmmmd-                 `ysosd`     \n         | |_| || '__/ _ \\/ __/ __|  / _` | '_ \\| | | | | |/ / _ \\ | | |                       `/mmmmmmyshshsoshyo/yhssmmmmmmmmmmmmmmmo                 `hsoyd`     \n         |  __/ | | |  __/\\__ \\__ \\ | (_| | | | | |_| | |   <  __/ |_| |_ _ _                  -hmmmmmyoooooooooosysooosdmmmmmmmmmmmmmd:                `dsoyd`     \n         |_|    |_|  \\___||___/___/  \\__,_|_| |_|\\__, | |_|\\_\\___|\\__, (_|_|_)               `:ymmmmmyoooooooooooooooooooydmmmmmmmmmmmmy`               `do+yd`     \n                                                 |___/            |___/                      :hmmmdmdooooooooosooooooooooooyhmmmmmmmmmmms`              `do+hy`     \n                                                                                             .---:ymsooooooooyyoooooooooooooooyhddmmmmmmm+              `do+hs      \n                                                                                                 -dyooooooooomoooooooooooooooooooodmo/:::.              .do+ds      \n                                                                                                `ydooooooooohdooooooooooooooooooosmy`                   -d++ds      \n                                                                                                /myooooooooomhooooooooooooooooooodd.                    :d++ds      \n                                                                                                ymoooooooooymyoooooooooooooooooodd/                     :d++m+      \n                                                                                               .hdoooooooooyNyoooooooooooooooosdd/                      :d+om:      \n                                                                                               .mdoooooooooymyooooooooooooooooshdy:                     :d+om:      \n                                                                                               .mmooooooooosmhooooooooooooooooooodd`                    /h+om:      \n");
			fclose(file); //closes file

			file = fopen("Monopoly_Data/hatdown.ani", "w"); //opens file
			//writes data to file
			fprintf(file, "         .----------------.  .----------------.  .-----------------. .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n        | | ____    ____ | || |     ____     | || | ____  _____  | || |     ____     | || |   ______     | || |     ____     | || |   _____      | || |  ____  ____  | |\n        | ||_   \\  /   _|| || |   .'    `.   | || ||_   \\|_   _| | || |   .'    `.   | || |  |_   __ \\   | || |   .'    `.   | || |  |_   _|     | || | |_  _||_  _| | |\n        | |  |   \\/   |  | || |  /  .--.  \\  | || |  |   \\ | |   | || |  /  .--.  \\  | || |    | |__) |  | || |  /  .--.  \\  | || |    | |       | || |   \\ \\  / /   | |\n        | |  | |\\  /| |  | || |  | |    | |  | || |  | |\\ \\| |   | || |  | |    | |  | || |    |  ___/   | || |  | |    | |  | || |    | |   _   | || |    \\ \\/ /    | |\n        | | _| |_\\/_| |_ | || |  \\  `--'  /  | || | _| |_\\   |_  | || |  \\  `--'  /  | || |   _| |_      | || |  \\  `--'  /  | || |   _| |__/ |  | || |    _|  |_    | |\n        | ||_____||_____|| || |   `.____.'   | || ||_____|\\____| | || |   `.____.'   | || |  |_____|     | || |   `.____.'   | || |  |________|  | || |   |______|   | |\n        | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n         '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                ``.-/+oos+/`                                                        \n                                                                                           `.:+syddmmmmmyydy.                                                       \n                                                                                      `.:+syyyhmmmmmmmmmhyhm/                                                       \n                                                                                   `-+shysoooosmmmmmmmmmmyymy`                                                      \n                                                                                 .+yhysooooooooymmmmmmmmmhyhm:                                                      \n                                                                                .ymmhooooooooooohmmmmmmmmmyymy.                                                     \n                                                                                .smmmdooooooooooodmmmhhhhhhssms:--.`                                                \n                                                                                 .smmmdoooooooyyyyhhhdmmdhhhhdhhhhhho.                                              \n                                                                         .++-`    .ommmdooshhhssooymmdhhhhyyyyyyyyyydh.                                             \n                                                                     //:./h-so.    .ommmdhysooooooohhhhyyyyysydys+++yy.                                             \n                                                                    -y:-syy `ho:.`  .smddhoooooooshhyyyhyo:.``.:+o+-`                                               \n                                                                    -y/ -m/  y/:ss//-:hmmmhooooyhhyyhho/`        `-oo-                                              \n                                                                    /d: `d` ./. `y/-o++dmmmssyhhyhhds-              -so`                                            \n                                                                   .sh`  -  . s- `oo:oshmmmhhyyhdy+-      /oo+/`    `.sy.                                           \n                                                                   /d:   `    .+o+dhhyhhyyyhhhyo+o/`     `-`  ``   .+++hs`                                          \n                                                                  `od`   .+:    `.-+syhyyyhs:-.   `                 -:-:h+:`                                        \n                                                                  `sh     `s`    .oo:..```h:    ``        `os/         ```/o                                        \n                                                                   +d`     `   `oh/`     `d:   :o+`        ```            -h                                        \n                                                                   .s/`     `-ohy:       `h/   `   .-                   `.yo                                        \n                                                                    -//ooo/+:--yh/`       +s       ++.`.::::.```-:/     hy/`                                        \n                                                                   .--o-..`.:ohmmd+.      .h:  .//:.//.```.-:+++:.o`   -my:`                                        \n                                                                   `:/s+oyhdmmmmmmmy/.`   /sh//:.                .s   `ymmmho:`                                     \n                                                                    -ymmmmmmmmmmmmmmmhs+.`o+.`       .:.`      ./o`  `smmmmmmmho:.`                ``````           \n                                                                     :hmmmmmmmmmmmmmmmmmds+oo/-.``.-+o-:+o+++oo+-  `:hmmmmmmmmmmmdhyyo:`         -oooo+/+o++/+-`    \n                                                                      :ymmmmmmmmmmmmmmmmmmmddddddho:.     ````   ./ymmmmmmmmmmmmmmmmmmmhs+/::::/+y. `` .``..../s:   \n                                                                       -smmmmmmmmmmmmmmmmmmmmmmmmmmho/--.....--/oommmmmmmmmmmmmmmmmmmmmmmmmmmmmmm: .`  -o-.s. -.h/  \n                                                                        `/hmmmmmmmmmmmmmmmmmmmmmmmmmmmy:dyyyyyyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd`-sy+` `y .s o`+h` \n                                                                          .+hmmmmmmmmmmmmmmmmmmmmmmmmmd.dyyyyhyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmsysoho  o  o : sh` \n                                                                            .+ydmmmmmmmmmmmmmmmmmmmmmmm:ss:``:oh+/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmho+osyd` -  ` `/h-  \n                                                                              `:oydmmmmmmmmmmmmmmmmmmmmo`  `/` ``-Nmmmmmmmmmmmmmmmmmmmmmmmmmmmmdoo+yh.ys` .:+yy+`   \n                                                                                 `-/oyddmmmmmmmmmmmmmmmh   `+`   `mmmmmmmmmmmmdddmmmmmmmmmmmmmmy+oomh+yyyohyodo     \n                                                                                     `.-/+osyyhhdmmmmmmd`   ``    ymmmmmmmmmmmmmddhhhhdddddddmmh++yh:-``-yd+om/     \n                                                                                           ````./dmmmmmm`   s/    /mmmmmmmmmmmmmmmmmd/`......-:ssyy-     od+om/     \n                                                                                                .ymmmmmm.         `mmmmmmmmmmmmmmmmmm+          ``       oh+om/     \n                                                                                                .ommmmmm`   -+     ommmmmmmmmmmmmmmmmo                   oh+om/     \n                                                                                                .ommmmmd    .:     `dmmmmmmmmmmmmmmmmo                   oyoom:     \n          ___                                            _                                      .ymmmmmh    `.      :mmmmmmmmmmmmmmmmy`                  syosd.     \n         |  _ \\  _ __ ___  ___ ___    __ _ _ __  _   _  | | _____ _   _                         -hmmmmmmo. .sys:`  `:hmmmmmmmmmmmmmmmd-                 `ysosd`     \n         | |_| || '__/ _ \\/ __/ __|  / _` | '_ \\| | | | | |/ / _ \\ | | |                       `/mmmmmmyshshsoshyo/yhssmmmmmmmmmmmmmmmo                 `hsoyd`     \n         |  __/ | | |  __/\\__ \\__ \\ | (_| | | | | |_| | |   <  __/ |_| |_ _ _                  -hmmmmmyoooooooooosysooosdmmmmmmmmmmmmmd:                `dsoyd`     \n         |_|    |_|  \\___||___/___/  \\__,_|_| |_|\\__, | |_|\\_\\___|\\__, (_|_|_)               `:ymmmmmyoooooooooooooooooooydmmmmmmmmmmmmy`               `do+yd`     \n                                                 |___/            |___/                      :hmmmdmdooooooooosooooooooooooyhmmmmmmmmmmms`              `do+hy`     \n                                                                                             .---:ymsooooooooyyoooooooooooooooyhddmmmmmmm+              `do+hs      \n                                                                                                 -dyooooooooomoooooooooooooooooooodmo/:::.              .do+ds      \n                                                                                                `ydooooooooohdooooooooooooooooooosmy`                   -d++ds      \n                                                                                                /myooooooooomhooooooooooooooooooodd.                    :d++ds      \n                                                                                                ymoooooooooymyoooooooooooooooooodd/                     :d++m+      \n                                                                                               .hdoooooooooyNyoooooooooooooooosdd/                      :d+om:      \n                                                                                               .mdoooooooooymyooooooooooooooooshdy:                     :d+om:      \n                                                                                               .mmooooooooosmhooooooooooooooooooodd`                    /h+om:      \n");
			fclose(file); //closes file

			file = fopen("Monopoly_Data/streets.csv", "w"); //opens file
			//writes data to file
			fprintf(file, "1,3,-1,6,8,9,-1,11,13,14,-1,16,18,19,-1,21,23,24,-1,26,27,29,-1,31,32,34,-1,37,39,-2");
			fclose(file); //closes file

			file = fopen("Monopoly_Data/settings.cfg", "w"); //opens file
			fprintf(file, "10~1~1~1~1~1");
			fclose(file); //close file
			
			printf("\nDone!\n\nPress any key to continue...");

			getch();

			preferences();
			return;
		default:
		case '3':
			preferences();
			return;
		}
		return;
	case '5':
		break;
	}


}

void help() { //displays the help screen

	int choice; //saves menu choice made by user

	system("cls");

	printf("\n\n                                                                                              .---.                        \n                                                                 .              __.....__     |   |_________   _...._      \n                                                               .'|          .-''         '.   |   |\\        |.'      '-.   \n                                                              <  |         /     .-''\"'-.  `. |   | \\        .'```'.    '. \n                                                               | |        /     /________\\   \\|   |  \\      |       \\     \\\n                                                               | | .'''-. |                  ||   |   |     |        |    |\n                                                               | |/.'''. \\\\    .-------------'|   |   |      \\      /    . \n                                                               |  /    | | \\    '-.____...---.|   |   |     |\\`'-.-'   .'  \n                                                               | |     | |  `.             .' |   |   |     | '-....-'`    \n                                                               | |     | |    `''-...... -'   '---'  .'     '.             \n                                                               | '.    | '.                        '-----------'           \n                                                               '---'   '---'                                               \n\n\n");

	printf("                                                           All of these rules are based on the official Monopoly rule book!\n\n                                                                              1: Object of the game\n\n                                                                              2: Basic play\n\n                                                                              3: Buying/Selling Property\n\n                                                                              4: Tile improvements\n\n                                                                              5: Railroads and utilities\n\n                                                                              6: Back to menu\n\n\n\n\n");

	do {
		choice = _getch();
	} while (!(choice == 49 || choice == 50 || choice == 51 || choice == 52 || choice == 53 || choice == 54));

	switch (choice) {
	case 49: //object of the game
		printf("\t\t\t    OBJECT: The object of the game is to become the wealthiest player through buying, renting and selling property.");
		printf("\n\n\t\t\tPress any key to continue...");
		_getch();
		system("cls");
		help();
		break;
	case 50: //basic gameplay
		printf("\t\t\t    BASIC PLAY: Each player gets a turn, starting with player 1. At the beginning of the player's\n\t\t\tturn, they roll the dice to determine how many squares they will move. The game will ask them to press ENTER to roll the dice, and it\n\t\t\twill automatically move them the number of squares they roll. The player is given the option to buy the property they land on (see BUYING\n\t\t\tPROPERTY). Rent is charged when the player lands on another player's square, which is automatically deducted from the player's balance.\n\t\t\tThe amount paid depends on the tile improvements present on the square, and no rent is paid when the property is mortgaged (Mrtg). Every\n\t\t\ttime the player passes GO, they are awarded $200.\n\n\t\t\tYou can type HELP at any time in the game to get a list of commands.");
		printf("\n\n\t\t\tPress any key to continue...");
		_getch();
		system("cls");
		help();
		break;
	case 51: //buying and selling properties
		printf("\t\t\t    BUYING/SELLING PROPERTIES: Whenever the player lands on unowned property, they may choose to buy that property. The game will prompt\n\t\t\tthem with a message: \"P1, would you like to buy MA?\". P1 is replaced by the player's two character personal code, and MA by the property\n\t\t\tthey are being offered. The player can then type \"Y\" or \"N\", Y for yes and N for no.");
		printf("\n\n\t\t\tPress any key to continue...");
		_getch();
		system("cls");
		help();
		break;
	case 52: //buildings/tile improvements
		printf("\t\t\t    TILE IMPROVEMENTS/MORTGAGING: When a player owns all three properties in a colour group, they may choose to buy houses for those properties.\n\t\t\tWith the use of the BUILDINGS command, they may choose to view the number of houses on every square on the board (VIEW) or buy or sell houses\n\t\t\ton any square (BUY/SELL).\n\n\t\t\tMORTGAGING PROPERTIES: You can mortgage a property with the MORTGAGE command, and later UNMORTGAGE it. A mortgaged property does not\n\t\t\tcharge rent when another player lands on it, and houses cannot be built on a street with a mortgaged property.");
		printf("\n\n\t\t\tPress any key to continue...");
		_getch();
		system("cls");
		help();
		break;
	case 53: //utilities
		printf("\t\t\t    RAILROADS: The rent for landing on a railroad depends on how many other railroads the square owner also possesses. If one railroad is\n\t\t\towned, the rent for landing on it is $25. If two are owned, rent is $50. Three is $100. Four is $200.\n\n\t\t\t    UTILITIES: EC, or electric company, and WW, or water works, are the two utilities on the Monopoly game board. When the player lands on\n\t\t\tone of these properties, their dice roll is multiplied by 4 or 10. It is multiplied by 4 if only one utility is possessed by the square\n\t\t\towner, but is multiplied by 10 if both utilities are owned.");
		printf("\n\n\t\t\tPress any key to continue...");
		_getch();
		system("cls");
		help();
		break;
	default:
	case 54:
		system("cls");
		break;
	}

}

void readPrefrances() //reads saves preferences from the settings file
{
	FILE *file; //file pointer

	file = fopen("Monopoly_Data/settings.cfg", "r"); //opens file

	if (file == NULL) //error traps
	{
		printf("Error reading preferences file, press ENTER to continue...");
		while (getchar() != '\n');
		return;
	}
	else
	{

		fscanf(file, "%d~", &gm.logLength); //reads saved file data into variables
		fscanf(file, "%d~", &gm.difficulty[0]);
		fscanf(file, "%d~", &gm.difficulty[1]);
		fscanf(file, "%d~", &gm.difficulty[2]);
		fscanf(file, "%d~", &gm.difficulty[3]);
		fscanf(file, "%d", &gm.collectRentInJail);

		fclose(file); //closes file and then exits function

	}
}

void savePrefrances() //saves preferences in settings file
{
	FILE *file;

	file = fopen("Monopoly_Data/settings.cfg", "w"); //opens file for writing

	if (file == NULL) //error traps
	{
		printf("Error reading preferences file, press ENTER to continue...");
		while (getchar() != '\n');
		return;
	}
	else
	{

		fprintf(file, "%d~", gm.logLength); //saves all settings variables in the file
		fprintf(file, "%d~", gm.difficulty[0]);
		fprintf(file, "%d~", gm.difficulty[1]);
		fprintf(file, "%d~", gm.difficulty[2]);
		fprintf(file, "%d~", gm.difficulty[3]);
		fprintf(file, "%d", gm.collectRentInJail);

		fclose(file); //closes file

	}

}

void start() { //initializes permanent game settings

	int i;

	/*HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 700, 900, TRUE);	// sets console size and pos

	SetConsoleTitle(_T("Monopoly - Mark and Ben")); //sets the console title*/

	textColour(0, 15); //sets the text color

	readPrefrances(); //loads data from prefrances file

	srand((unsigned int)time(NULL)); //initilizes the random number generator

	strcpy(gm.squareNames[0], "GO"); //sets the name of all the squares on the board
	strcpy(gm.squareNames[1], "MA");
	strcpy(gm.squareNames[2], "CC");
	strcpy(gm.squareNames[3], "BA");
	strcpy(gm.squareNames[4], "TX");
	strcpy(gm.squareNames[5], "RR");
	strcpy(gm.squareNames[6], "OA");
	strcpy(gm.squareNames[7], "C?");
	strcpy(gm.squareNames[8], "VT");
	strcpy(gm.squareNames[9], "CA");
	strcpy(gm.squareNames[10], "JV");
	strcpy(gm.squareNames[11], "SC");
	strcpy(gm.squareNames[12], "EC");
	strcpy(gm.squareNames[13], "SA");
	strcpy(gm.squareNames[14], "VI");
	strcpy(gm.squareNames[15], "RR");
	strcpy(gm.squareNames[16], "SJ");
	strcpy(gm.squareNames[17], "CC");
	strcpy(gm.squareNames[18], "TA");
	strcpy(gm.squareNames[19], "NY");
	strcpy(gm.squareNames[20], "FP");
	strcpy(gm.squareNames[21], "KA");
	strcpy(gm.squareNames[22], "C?");
	strcpy(gm.squareNames[23], "IN");
	strcpy(gm.squareNames[24], "IL");
	strcpy(gm.squareNames[25], "RR");
	strcpy(gm.squareNames[26], "AA");
	strcpy(gm.squareNames[27], "VA");
	strcpy(gm.squareNames[28], "WW");
	strcpy(gm.squareNames[29], "MG");
	strcpy(gm.squareNames[30], "TJ");
	strcpy(gm.squareNames[31], "PC");
	strcpy(gm.squareNames[32], "NC");
	strcpy(gm.squareNames[33], "CC");
	strcpy(gm.squareNames[34], "PA");
	strcpy(gm.squareNames[35], "RR");
	strcpy(gm.squareNames[36], "C?");
	strcpy(gm.squareNames[37], "PP");
	strcpy(gm.squareNames[38], "TX");
	strcpy(gm.squareNames[39], "BW");
	strcpy(gm.squareNames[40], "JL");
	strcpy(gm.squareNames[41], "JL");
	strcpy(gm.squareNames[42], "JL");
	strcpy(gm.squareNames[43], "JL");
	strcpy(gm.squareNames[44], "  ");

}

void genFiles() { //checks if a file exists, and creates it if it doesnt

	FILE *file;	// file to save the menu screen animations
	int exists;

	_mkdir("Monopoly_Data");
	_mkdir("Monopoly_Data/saves");

	exists = checkFiles("Monopoly_Data/settings.cfg"); //checks if file exists

	if (exists == 0) { //creates file if it dosent exist

		file = fopen("Monopoly_Data/settings.cfg", "w"); //opens file

		//writes data to file
		fprintf(file, "10~1~1~1~1~1");

		fclose(file); //close file

	}

	exists = checkFiles("Monopoly_Data/propertyValues.csv"); //checks if file exists

	if (exists == 0) { //creates file if it dosent exist

		file = fopen("Monopoly_Data/propertyValues.csv", "w"); //opens file
					
		//writes data to file
		fprintf(file, "0,60,0,60,0,200,100,0,100,120,0,140,150,160,160,200,180,0,180,200,0,220,0,220,240,200,260,260,150,280,0,300,300,0,320,200,0,350,0,400,-1,0,2,0,4,0,0,6,0,6,8,0,10,0,10,12,0,14,0,14,16,0,18,0,18,20,0,22,22,0,24,0,26,26,0,28,0,0,35,0,50,-1,0,10,0,20,0,0,30,0,30,40,0,50,0,50,60,0,70,0,70,80,0,90,0,90,100,0,110,110,0,120,0,130,130,0,150,0,0,175,0,200,-1,0,30,0,60,0,0,90,0,90,100,0,150,0,150,200,0,200,0,200,220,0,250,0,250,300,0,330,330,0,360,0,390,390,0,450,0,0,500,0,600,-1,0,90,0,180,0,0,270,0,270,300,0,450,0,450,500,0,550,0,550,600,0,700,0,700,750,0,800,800,0,850,0,900,900,0,1000,0,0,1100,0,1400,-1,0,160,0,320,0,0,400,0,400,450,0,625,0,625,700,0,750,0,750,800,0,875,0,875,925,0,975,975,0,1025,0,1100,1100,0,1200,0,0,1300,0,1700,-1,0,250,0,450,0,0,550,0,550,600,0,750,0,750,900,0,950,0,950,1000,0,1050,0,1050,1100,0,1150,1150,0,1200,0,1275,1275,0,1400,0,0,1500,0,2000,-2");

		fclose(file); //close file

	}

	exists = checkFiles("Monopoly_Data/hatup.ani");//checks if file exists

	if (exists == 0) { //creates file if it dosent exist

		file = fopen("Monopoly_Data/hatup.ani", "w"); //opens file

		//writes data to file
		fprintf(file, "         .----------------.  .----------------.  .-----------------. .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n        | | ____    ____ | || |     ____     | || | ____  _____  | || |     ____     | || |   ______     | || |     ____     | || |   _____      | || |  ____  ____  | |\n        | ||_   \\  /   _|| || |   .'    `.   | || ||_   \\|_   _| | || |   .'    `.   | || |  |_   __ \\   | || |   .'    `.   | || |  |_   _|     | || | |_  _||_  _| | |\n        | |  |   \\/   |  | || |  /  .--.  \\  | || |  |   \\ | |   | || |  /  .--.  \\  | || |    | |__) |  | || |  /  .--.  \\  | || |    | |       | || |   \\ \\  / /   | |\n        | |  | |\\  /| |  | || |  | |    | |  | || |  | |\\ \\| |   | || |  | |    | |  | || |    |  ___/   | || |  | |    | |  | || |    | |   _   | || |    \\ \\/ /    | |\n        | | _| |_\\/_| |_ | || |  \\  `--'  /  | || | _| |_\\   |_  | || |  \\  `--'  /  | || |   _| |_      | || |  \\  `--'  /  | || |   _| |__/ |  | || |    _|  |_    | |\n        | ||_____||_____|| || |   `.____.'   | || ||_____|\\____| | || |   `.____.'   | || |  |_____|     | || |   `.____.'   | || |  |________|  | || |   |______|   | |\n        | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n         '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n                                                                                      ./yhdh:                                                                       \n                                                                                   `/ymmdyyhds.                                                                     \n                                                                                 .odmmmmmmhyydd/                                                                    \n                                                                               .ommmmmmmmmmmhyhdy-    ```                                                           \n                                                                             .ohshmmmmmmmmmmmdyhddy+oyhhhhs-                                                        \n                                                                            +hsooosdmmmmmmmmmhyhoymhyyyyyyhd-                                                       \n                                                                          -yyooooooosdmmmmdyydmydhyyyyyyyyym/                                                       \n                                                                         +dsoooooooooosddyymmdhdyyyhmmmyyydh.                                                       \n                                                                        smsooooooooooohhohmmhhhyyhmmmmdyyhd:                                                        \n                                                                       :mmmhsoooooooydsoooshhhyyhmmmmdyyhd/                                                         \n                                                                       .ymmmmdysoosdyoooooohhyydmmmdhyydh-                                                          \n                                                                         ./ymmmmhydsoooooohhyydmmdhyyddo`                                                           \n                                                                           `-odmmdhyoooooyhyyddhyyhdh+.                                                             \n                                                                   `..`       ./hmmmmysoyhyydyyhhyo:`  ````                                                         \n                                                                   `h/+o:````````/dmmmmhhyhdhso/.`./osooooooo++:.`                                                  \n                                                                 ...os `+h+/oh-:++oymmdyyhh:`  .+so/.``     ``.-+o+-                                                \n                                                                /o.-om:  s` `-++++oyhyyhds.  .os/`               `-oo-                                              \n                                                                `+s` +h  :`+/-.omhhhhhho-   :y:                     -so`                                            \n                                                                 `yo  /`    .://:/+o/-`    :y.  `         /oo+/`    `.sy.                                           \n                                                                  /m`   ``       -y-      .h. :o++/`     `-`  ``   .+++hs`                                          \n                                                                  `d/   `:+/`    ho       +o  .   `                 -:-:h+:                                         \n                                                                   oh`     .-   /m:      `h:    ``        `os/         ```/o                                        \n                                                                   `hs        `/h/       `d:   :o+`        ```            -h                                        \n                                                                    .ss.    -/+do        `h/   `   .-                   `.yo                                        \n                                                                    `-sho+++:.`yh:        +s       ++.`.::::.```-:/     hy/`                                        \n                                                                    .-o-..`.:ohmmd+`      .h:  .//:.//.```.-:+++:.o`   -my:`                                        \n                                                                    ./s+oyhdmmmmmmmy:`    /sh//:.                .s   `ymmmho:`                                     \n                                                                     smmmmmmmmmmmmmmmhs+.`o+.`       .:.`      ./o`  `smmmmmmmho:.`                ``````           \n                                                                     `ymmmmmmmmmmmmmmmmmds+oo/-.``.-+o-:+o+++oo+-  `:hmmmmmmmmmmmdhyyo:`         -oooo+/+o++/+-`    \n                                                                      .ymmmmmmmmmmmmmmmmmmmddddddho:.     ````   ./ymmmmmmmmmmmmmmmmmmmhs+/::::/+y. `` .``..../s:   \n                                                                       `+dmmmmmmmmmmmmmmmmmmmmmmmmmho/--.....--/oommmmmmmmmmmmmmmmmmmmmmmmmmmmmmm: .`  -o-.s. -.h/  \n                                                                         -ymmmmmmmmmmmmmmmmmmmmmmmmmmmy:dyyyyyyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd`-sy+` `y .s o`+h` \n                                                                          `:ymmmmmmmmmmmmmmmmmmmmmmmmmd.dyyyyhyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmsysoho  o  o : sh` \n                                                                            `:sdmmmmmmmmmmmmmmmmmmmmmmm:ss:``:oh+/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmho+osyd` -  ` `/y-  \n                                                                               ./ydmmmmmmmmmmmmmmmmmmmmo`  `/` ``-Nmmmmmmmmmmmmmmmmmmmmmmmmmmmmdoo+yh.ys` .:+yy+`   \n                                                                                  .:+shdmmmmmmmmmmmmmmmh   `+`   `mmmmmmmmmmmmdddmmmmmmmmmmmmmmy+oomh+yyyohyodo     \n                                                                                      `.-/+osyyhdmmmmmmd`   ``    ymmmmmmmmmmmmmddhhhhdddddddmmh++yh:-``-yd+om/     \n                                                                                               `-dmmmmmm`   s/    /mmmmmmmmmmmmmmmmmd/`......-:ssyy-     od+om/     \n                                                                                                 smmmmmm.         `mmmmmmmmmmmmmmmmmm+          ``       oh+om/     \n                                                                                                 /mmmmmm`   -+     ommmmmmmmmmmmmmmmmo                   oh+om/     \n                                                                                                 /mmmmmd    .:     `dmmmmmmmmmmmmmmmmo                   oyoom:     \n          ___                                            _                                      .ymmmmmh    `.      :mmmmmmmmmmmmmmmmy`                  syosd.     \n         |  _ \\  _ __ ___  ___ ___    __ _ _ __  _   _  | | _____ _   _                         -hmmmmmmo. .sys:`  `:hmmmmmmmmmmmmmmmd-                 `ysosd`     \n         | |_| || '__/ _ \\/ __/ __|  / _` | '_ \\| | | | | |/ / _ \\ | | |                       `/mmmmmmyshshsoshyo/yhssmmmmmmmmmmmmmmmo                 `hsoyd`     \n         |  __/ | | |  __/\\__ \\__ \\ | (_| | | | | |_| | |   <  __/ |_| |_ _ _                  -hmmmmmyoooooooooosysooosdmmmmmmmmmmmmmd:                `dsoyd`     \n         |_|    |_|  \\___||___/___/  \\__,_|_| |_|\\__, | |_|\\_\\___|\\__, (_|_|_)               `:ymmmmmyoooooooooooooooooooydmmmmmmmmmmmmy`               `do+yd`     \n                                                 |___/            |___/                      :hmmmdmdooooooooosooooooooooooyhmmmmmmmmmmms`              `do+hy`     \n                                                                                             `.--:ymsooooooooyyoooooooooooooooyhddmmmmmmm+              `do+hs      \n                                                                                                 -dyooooooooomoooooooooooooooooooodmo/:::.              `do+ds      \n                                                                                                `ydooooooooohdooooooooooooooooooosmy`                   -d++ds      \n                                                                                                /myooooooooomhooooooooooooooooooodd.                    :d++ds      \n                                                                                                ymoooooooooymyoooooooooooooooooodd/                     :d++m+      \n                                                                                               `hdoooooooooyNyoooooooooooooooosdd/                      :d+om:      \n                                                                                               .mdoooooooooymyooooooooooooooooshdy:                     :d+om:      \n                                                                                               .mmooooooooosmhooooooooooooooooooodd`                    /h+om:      \n");

		fclose(file); //closes file

	}

	exists = checkFiles("Monopoly_Data/hatmid.ani");//checks if file exists

	if (exists == 0) { //creates file if it dosent exist

		file = fopen("Monopoly_Data/hatmid.ani", "w"); //opens file

		//writes data to file
		fprintf(file, "         .----------------.  .----------------.  .-----------------. .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n        | | ____    ____ | || |     ____     | || | ____  _____  | || |     ____     | || |   ______     | || |     ____     | || |   _____      | || |  ____  ____  | |\n        | ||_   \\  /   _|| || |   .'    `.   | || ||_   \\|_   _| | || |   .'    `.   | || |  |_   __ \\   | || |   .'    `.   | || |  |_   _|     | || | |_  _||_  _| | |\n        | |  |   \\/   |  | || |  /  .--.  \\  | || |  |   \\ | |   | || |  /  .--.  \\  | || |    | |__) |  | || |  /  .--.  \\  | || |    | |       | || |   \\ \\  / /   | |\n        | |  | |\\  /| |  | || |  | |    | |  | || |  | |\\ \\| |   | || |  | |    | |  | || |    |  ___/   | || |  | |    | |  | || |    | |   _   | || |    \\ \\/ /    | |\n        | | _| |_\\/_| |_ | || |  \\  `--'  /  | || | _| |_\\   |_  | || |  \\  `--'  /  | || |   _| |_      | || |  \\  `--'  /  | || |   _| |__/ |  | || |    _|  |_    | |\n        | ||_____||_____|| || |   `.____.'   | || ||_____|\\____| | || |   `.____.'   | || |  |_____|     | || |   `.____.'   | || |  |________|  | || |   |______|   | |\n        | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n         '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n                                                                                                                                                                    \n                                                                                         `.:+oso:`                                                                  \n                                                                                      `-+ydmmyyhmo.                                                                 \n                                                                                    .+ymmmmmmmhyhms-                                                                \n                                                                                 `:shmmmmmmmmmmhyymh:                                                               \n                                                                               .+yhsohmmmmmmmmmmdyydd+. ``..`                                                       \n                                                                             .+hyoooooymmmmmmmmmmdyhydysyhhhys:`                                                    \n                                                                           .+hhooooooooohmmmmmdhyhdshdhyyyyyyhd/`                                                   \n                                                                          :ydsooooooooooosmmyyhmmhhhyyyyyyyyyymo`                                                   \n                                                                         -ymmhsoooooooooyhysdmmhhhyyyhdmmmyyydd:                                                    \n                                                                         .smmmmhsoooooyhysoosdhhhyyhdmmmmhyyhh/`                                                    \n                                                                          `.odmmmhsoydysoooooydyyydmmmmdyyydy-                                                      \n                                                                            `.odmmmdhsooooooydyyhmmmmdyyydh:`                                                       \n                                                                    `-:-`     `.smmhdyooooohhyydmddhyyhdh/.` `                                                      \n                                                                    .y+/o/`   ` `-ymmmdsoshhyhdyyyyhddysso+++//:.`                                                  \n                                                                 /+/+sy `os///+::-`+mmmmhhyyddhhdhs/.``     ``.-++/.                                                \n                                                                `os`.sm. `s``-s+::++ymmdyyhs/+ys/`               `-++.`                                             \n                                                                 -so `y/  /-/` .shsyhhyyhs-`/y:                     -s+`                                            \n                                                                 `od` .:  ` :o+ohyhhyhy+- `/y.  `         /oo+/`    `.so`                                           \n                                                                 `+m.   `     ````//..`   :h. :o++/`     `-`  ``   .+++y+`                                          \n                                                                  /m-   ./+-    `oo`     .oo  .   `                 -:-:h::`                                        \n                                                                  :ho     `+`   +d`      -h:    ``        `os/         ```/+`                                       \n                                                                  `od.        `+h:`      -d:   :o+`        ```            -y`                                       \n                                                                   .sy-     -/sy-`       -h/   `   .-                   `.y/`                                       \n                                                                   `/oo/++++:.`yy-       .os       ++.`.::::.```-:/     hs-`                                        \n                                                                   ..-o-..`.:ohmmd:`      :h:  .//:.//.```.-:+++:.o`   -m+-`                                        \n                                                                   ``:s+oyhdmmmmmmms-`   `+sh//:.                .s   `ymmdy+-`                                     \n                                                                    -ymmmmmmmmmmmmmmmhs+.`o+.`       .:.`      ./o`  `smmmmmmmho:.`                ``````           \n                                                                     :hmmmmmmmmmmmmmmmmmds+oo/-.``.-+o-:+o+++oo+-  `:hmmmmmmmmmmmdhyyo:`         -oooo+/+o++/+-`    \n                                                                      :ymmmmmmmmmmmmmmmmmmmddddddho:.     ````   ./ymmmmmmmmmmmmmmmmmmmhs+/::::/+y. `` .``..../s:   \n                                                                       -smmmmmmmmmmmmmmmmmmmmmmmmmmho/--.....--/oommmmmmmmmmmmmmmmmmmmmmmmmmmmmmm: .`  -o-.s. -.h/  \n                                                                        `/hmmmmmmmmmmmmmmmmmmmmmmmmmmmy:dyyyyyyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd`-sy+` `y .s o`+h` \n                                                                          .+hmmmmmmmmmmmmmmmmmmmmmmmmmd.dyyyyhyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmsysoho  o  o : sh` \n                                                                            .+ydmmmmmmmmmmmmmmmmmmmmmmm:ss:``:oh+/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmho+osyd` -  ` `/h-  \n                                                                              `:oydmmmmmmmmmmmmmmmmmmmmo`  `/` ``-Nmmmmmmmmmmmmmmmmmmmmmmmmmmmmdoo+yh.ys` .:+yy+`   \n                                                                                 `-/oyddmmmmmmmmmmmmmmmh   `+`   `mmmmmmmmmmmmdddmmmmmmmmmmmmmmy+oomh+yyyohyodo     \n                                                                                     `.-/+osyyhhdmmmmmmd`   ``    ymmmmmmmmmmmmmddhhhhdddddddmmh++yh:-``-yd+om/     \n                                                                                           ````./dmmmmmm`   s/    /mmmmmmmmmmmmmmmmmd/`......-:ssyy-     od+om/     \n                                                                                                .ymmmmmm.         `mmmmmmmmmmmmmmmmmm+          ``       oh+om/     \n                                                                                                .ommmmmm`   -+     ommmmmmmmmmmmmmmmmo                   oh+om/     \n                                                                                                .ommmmmd    .:     `dmmmmmmmmmmmmmmmmo                   oyoom:     \n          ___                                            _                                      .ymmmmmh    `.      :mmmmmmmmmmmmmmmmy`                  syosd.     \n         |  _ \\  _ __ ___  ___ ___    __ _ _ __  _   _  | | _____ _   _                         -hmmmmmmo. .sys:`  `:hmmmmmmmmmmmmmmmd-                 `ysosd`     \n         | |_| || '__/ _ \\/ __/ __|  / _` | '_ \\| | | | | |/ / _ \\ | | |                       `/mmmmmmyshshsoshyo/yhssmmmmmmmmmmmmmmmo                 `hsoyd`     \n         |  __/ | | |  __/\\__ \\__ \\ | (_| | | | | |_| | |   <  __/ |_| |_ _ _                  -hmmmmmyoooooooooosysooosdmmmmmmmmmmmmmd:                `dsoyd`     \n         |_|    |_|  \\___||___/___/  \\__,_|_| |_|\\__, | |_|\\_\\___|\\__, (_|_|_)               `:ymmmmmyoooooooooooooooooooydmmmmmmmmmmmmy`               `do+yd`     \n                                                 |___/            |___/                      :hmmmdmdooooooooosooooooooooooyhmmmmmmmmmmms`              `do+hy`     \n                                                                                             .---:ymsooooooooyyoooooooooooooooyhddmmmmmmm+              `do+hs      \n                                                                                                 -dyooooooooomoooooooooooooooooooodmo/:::.              .do+ds      \n                                                                                                `ydooooooooohdooooooooooooooooooosmy`                   -d++ds      \n                                                                                                /myooooooooomhooooooooooooooooooodd.                    :d++ds      \n                                                                                                ymoooooooooymyoooooooooooooooooodd/                     :d++m+      \n                                                                                               .hdoooooooooyNyoooooooooooooooosdd/                      :d+om:      \n                                                                                               .mdoooooooooymyooooooooooooooooshdy:                     :d+om:      \n                                                                                               .mmooooooooosmhooooooooooooooooooodd`                    /h+om:      \n");

		fclose(file); //closes file

	}

	exists = checkFiles("Monopoly_Data/hatdown.ani");//checks if file exists

	if (exists == 0) { //creates file if it dosent exist

		file = fopen("Monopoly_Data/hatdown.ani", "w"); //opens file

		//writes data to file
		fprintf(file, "         .----------------.  .----------------.  .-----------------. .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n        | | ____    ____ | || |     ____     | || | ____  _____  | || |     ____     | || |   ______     | || |     ____     | || |   _____      | || |  ____  ____  | |\n        | ||_   \\  /   _|| || |   .'    `.   | || ||_   \\|_   _| | || |   .'    `.   | || |  |_   __ \\   | || |   .'    `.   | || |  |_   _|     | || | |_  _||_  _| | |\n        | |  |   \\/   |  | || |  /  .--.  \\  | || |  |   \\ | |   | || |  /  .--.  \\  | || |    | |__) |  | || |  /  .--.  \\  | || |    | |       | || |   \\ \\  / /   | |\n        | |  | |\\  /| |  | || |  | |    | |  | || |  | |\\ \\| |   | || |  | |    | |  | || |    |  ___/   | || |  | |    | |  | || |    | |   _   | || |    \\ \\/ /    | |\n        | | _| |_\\/_| |_ | || |  \\  `--'  /  | || | _| |_\\   |_  | || |  \\  `--'  /  | || |   _| |_      | || |  \\  `--'  /  | || |   _| |__/ |  | || |    _|  |_    | |\n        | ||_____||_____|| || |   `.____.'   | || ||_____|\\____| | || |   `.____.'   | || |  |_____|     | || |   `.____.'   | || |  |________|  | || |   |______|   | |\n        | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n         '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                                                                                    \n                                                                                                ``.-/+oos+/`                                                        \n                                                                                           `.:+syddmmmmmyydy.                                                       \n                                                                                      `.:+syyyhmmmmmmmmmhyhm/                                                       \n                                                                                   `-+shysoooosmmmmmmmmmmyymy`                                                      \n                                                                                 .+yhysooooooooymmmmmmmmmhyhm:                                                      \n                                                                                .ymmhooooooooooohmmmmmmmmmyymy.                                                     \n                                                                                .smmmdooooooooooodmmmhhhhhhssms:--.`                                                \n                                                                                 .smmmdoooooooyyyyhhhdmmdhhhhdhhhhhho.                                              \n                                                                         .++-`    .ommmdooshhhssooymmdhhhhyyyyyyyyyydh.                                             \n                                                                     //:./h-so.    .ommmdhysooooooohhhhyyyyysydys+++yy.                                             \n                                                                    -y:-syy `ho:.`  .smddhoooooooshhyyyhyo:.``.:+o+-`                                               \n                                                                    -y/ -m/  y/:ss//-:hmmmhooooyhhyyhho/`        `-oo-                                              \n                                                                    /d: `d` ./. `y/-o++dmmmssyhhyhhds-              -so`                                            \n                                                                   .sh`  -  . s- `oo:oshmmmhhyyhdy+-      /oo+/`    `.sy.                                           \n                                                                   /d:   `    .+o+dhhyhhyyyhhhyo+o/`     `-`  ``   .+++hs`                                          \n                                                                  `od`   .+:    `.-+syhyyyhs:-.   `                 -:-:h+:`                                        \n                                                                  `sh     `s`    .oo:..```h:    ``        `os/         ```/o                                        \n                                                                   +d`     `   `oh/`     `d:   :o+`        ```            -h                                        \n                                                                   .s/`     `-ohy:       `h/   `   .-                   `.yo                                        \n                                                                    -//ooo/+:--yh/`       +s       ++.`.::::.```-:/     hy/`                                        \n                                                                   .--o-..`.:ohmmd+.      .h:  .//:.//.```.-:+++:.o`   -my:`                                        \n                                                                   `:/s+oyhdmmmmmmmy/.`   /sh//:.                .s   `ymmmho:`                                     \n                                                                    -ymmmmmmmmmmmmmmmhs+.`o+.`       .:.`      ./o`  `smmmmmmmho:.`                ``````           \n                                                                     :hmmmmmmmmmmmmmmmmmds+oo/-.``.-+o-:+o+++oo+-  `:hmmmmmmmmmmmdhyyo:`         -oooo+/+o++/+-`    \n                                                                      :ymmmmmmmmmmmmmmmmmmmddddddho:.     ````   ./ymmmmmmmmmmmmmmmmmmmhs+/::::/+y. `` .``..../s:   \n                                                                       -smmmmmmmmmmmmmmmmmmmmmmmmmmho/--.....--/oommmmmmmmmmmmmmmmmmmmmmmmmmmmmmm: .`  -o-.s. -.h/  \n                                                                        `/hmmmmmmmmmmmmmmmmmmmmmmmmmmmy:dyyyyyyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd`-sy+` `y .s o`+h` \n                                                                          .+hmmmmmmmmmmmmmmmmmmmmmmmmmd.dyyyyhyho/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmsysoho  o  o : sh` \n                                                                            .+ydmmmmmmmmmmmmmmmmmmmmmmm:ss:``:oh+/mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmho+osyd` -  ` `/h-  \n                                                                              `:oydmmmmmmmmmmmmmmmmmmmmo`  `/` ``-Nmmmmmmmmmmmmmmmmmmmmmmmmmmmmdoo+yh.ys` .:+yy+`   \n                                                                                 `-/oyddmmmmmmmmmmmmmmmh   `+`   `mmmmmmmmmmmmdddmmmmmmmmmmmmmmy+oomh+yyyohyodo     \n                                                                                     `.-/+osyyhhdmmmmmmd`   ``    ymmmmmmmmmmmmmddhhhhdddddddmmh++yh:-``-yd+om/     \n                                                                                           ````./dmmmmmm`   s/    /mmmmmmmmmmmmmmmmmd/`......-:ssyy-     od+om/     \n                                                                                                .ymmmmmm.         `mmmmmmmmmmmmmmmmmm+          ``       oh+om/     \n                                                                                                .ommmmmm`   -+     ommmmmmmmmmmmmmmmmo                   oh+om/     \n                                                                                                .ommmmmd    .:     `dmmmmmmmmmmmmmmmmo                   oyoom:     \n          ___                                            _                                      .ymmmmmh    `.      :mmmmmmmmmmmmmmmmy`                  syosd.     \n         |  _ \\  _ __ ___  ___ ___    __ _ _ __  _   _  | | _____ _   _                         -hmmmmmmo. .sys:`  `:hmmmmmmmmmmmmmmmd-                 `ysosd`     \n         | |_| || '__/ _ \\/ __/ __|  / _` | '_ \\| | | | | |/ / _ \\ | | |                       `/mmmmmmyshshsoshyo/yhssmmmmmmmmmmmmmmmo                 `hsoyd`     \n         |  __/ | | |  __/\\__ \\__ \\ | (_| | | | | |_| | |   <  __/ |_| |_ _ _                  -hmmmmmyoooooooooosysooosdmmmmmmmmmmmmmd:                `dsoyd`     \n         |_|    |_|  \\___||___/___/  \\__,_|_| |_|\\__, | |_|\\_\\___|\\__, (_|_|_)               `:ymmmmmyoooooooooooooooooooydmmmmmmmmmmmmy`               `do+yd`     \n                                                 |___/            |___/                      :hmmmdmdooooooooosooooooooooooyhmmmmmmmmmmms`              `do+hy`     \n                                                                                             .---:ymsooooooooyyoooooooooooooooyhddmmmmmmm+              `do+hs      \n                                                                                                 -dyooooooooomoooooooooooooooooooodmo/:::.              .do+ds      \n                                                                                                `ydooooooooohdooooooooooooooooooosmy`                   -d++ds      \n                                                                                                /myooooooooomhooooooooooooooooooodd.                    :d++ds      \n                                                                                                ymoooooooooymyoooooooooooooooooodd/                     :d++m+      \n                                                                                               .hdoooooooooyNyoooooooooooooooosdd/                      :d+om:      \n                                                                                               .mdoooooooooymyooooooooooooooooshdy:                     :d+om:      \n                                                                                               .mmooooooooosmhooooooooooooooooooodd`                    /h+om:      \n");
		
		fclose(file); //closes file

	}

	exists = checkFiles("Monopoly_Data/streets.csv");//checks if file exists

		if (exists == 0) { //creates file if it dosent exist

			file = fopen("Monopoly_Data/streets.csv", "w"); //opens file

			//writes data to file
			fprintf(file, "1,3,-1,6,8,9,-1,11,13,14,-1,16,18,19,-1,21,23,24,-1,26,27,29,-1,31,32,34,-1,37,39,-2");
			
			fclose(file); //closes file

		}

	exists = checkFiles("Monopoly_Data/highscores.sav"); //checks if file exists

	if (exists == 0) { //creates file if it dosent exist

		file = fopen("Monopoly_Data/highscores.sav", "w"); //opens file

		//writes data to file
		fprintf(file, "NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~NA~0~0~");

		fclose(file); //close file

	}

}

int checkFiles(char *path) { //checks if the file exists

	FILE *toCheck; //file pointer

	toCheck = fopen(path, "r"); //opens file

	if (toCheck == NULL) { //if it dosent exist return 0
		return 0;
	}
	else { //if it exists return 1
		fclose(toCheck); //closes file
		return 1;
	}

}

void startGame() { //initalizes the game with user defined settings

	FILE *propertyValues;
	HWND console = GetConsoleWindow();
	RECT r;

	int i,j,stop = 0,temp;

	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 565, 850, TRUE);    // sets console size and pos

	system("COLOR 70"); //sets the colour of text to black on grey

	for (i = 0; i < 4; i++) {

		gm.playerLocation[i] = 0; //moves all player to go
		gm.communityGOOJF[i] = 0; //puts community get out of jail free cards back in the decks
		gm.chanceGOOJF[i] = 0; //puts chance get out of jail free cards back in the decks
		gm.jailed[i] = 0; //gats player out of jail
		gm.logPosition[i] = 0; //resets log positions
		gm.timesPassedGo[i] = 0;

		switch (gm.difficulty[i])
		{
		case 0:
			gm.playerMoney[i] = 3000; //resets player money for "easy" difficulty
			break;
		case 1:
			gm.playerMoney[i] = 1500; //resets all player money for "normal" difficulty
			break;
		case 2:
			gm.playerMoney[i] = 1000; //resets all player money for "hard" difficulty
			break;
		}
	}

	for (i = 0; i < 40; i++)
	{
		gm.PropertieOwner[i] = -1; //sets all properties to unowned
	}

	gm.rollagain = 0; //resets values
	gm.activePlayer = 0;
	gm.communityGOOJFUsed = 0;
	gm.chanceGOOJFUsed = 0;
	gm.winner = -1;

	sortDecks(); //sort both decks decks

	mixDecks(0); //calls sort deck function and sorts chest and chance decks

	/*currentLogLine = shownLogLineAmount
	
	FILE *log;

	log = fopen("log.dat", "w");

	for (int i = 0; i < shownLogLineAmount; i++)
	{
		fprintf(log, "%d%c%s\n", i, " ");
		printf("\n");
	}

	fclose(log);

	printf("Got past start game");*/

	propertyValues = fopen("Monopoly_Data/propertyValues.csv", "r"); //opens file to get the prices of squares

	if (propertyValues == NULL) //checks the file exists
	{
		saveLog("!!! Unable to read propertyValues.csv");
		saveLog("!!! Try re-gening the files in the properties menu");
		saveLog("!!! And run the program as administator");
		return;
	}

	j = 0; //initializes variables
	i = 0;

	while (stop == 0)
	{

		fscanf(propertyValues, "%d,", &temp); //reads value from .cvs file
		if (temp == -1 || temp == -2) //temp == -1 means increase the fist index, -2 == end of file
		{
			if (temp == -1) //increase first index
			{
				j++;
				i = 0;
			}
			else
			{
				stop = 1;
			}
		}
		else
		{
			gm.squarePrices[i][j] = temp; //puts value into memory
			i++;
		}
		if (i == 42) //cheacks to see if an error occured
		{

			saveLog("!!! An error occured when reading the property values file"); //prints error msg
			saveLog("!!! This might cause the game to not work as expected");
			saveLog("!!! Try re-gening the files in the properties menu");
			return; //exits function

		}
	}
}

int getCommand() { //gets the command from the user

	char command[100], log[100], *token[3];
	int i, returnValue,quit = 0, savedCorrectly;

	if (gm.bankrupt[gm.activePlayer] == 1) {return 1;} //checks if the user is bankrupt

	do {
		strcpy(log, gm.playerNames[gm.activePlayer]); //creates string to ask user to enter command
		strcat(log,", enter command (type help for help): ");
		saveLog(log);

		printf("> ");
		fflush(stdin);
		gets(command); //gets the command
		strcpy(log, "> "); //adds > to begining of command
		strcat(log,command);
		strupr(command); //converts command to upper case
		saveLog(log); //saves the command


		token[0] = strtok(command, " "); //gets the first token (first word user enters)
		i = 1;
		do
		{
			token[i] = strtok(NULL, " "); //gets subsiqant tokens (words)
			i++;

		} while (!(token[i - 1] == NULL || i == 3)); //ends loop then there are three words (max arguments) or there are no more words

		if (token[0] == NULL) { token[0] = " "; } //sets null token to spaces to avoid a crash
		if (token[1] == NULL) { token[1] = " "; }
		if (token[2] == NULL) { token[2] = " "; }

		//chacks to see what the user enterd
		if (!((strcmp(token[0], "HELP") == 0) || (strcmp(token[0], "END") == 0) || (strcmp(token[0], "BUILDINGS") == 0) || (strcmp(token[0], "MORTGAGE") == 0) || (strcmp(token[0], "QUIT") == 0) || (strcmp(token[0], "UNMORTGAGE") == 0) || (strcmp(token[0], "SAVE") == 0))) //error traps the users input
		{
			//runs if a player enters an invalid command
			if (token[0] == NULL)
			{
				saveLog("Error, unknown command"); //displays error msg
				token[0] = " ";
			}
			else
			{
			strcpy(log, "Error, unknown command ");//displays error msg
			strcat(log, token[0]);
			saveLog(log);
			}
		}

		if (strcmp(token[0], "HELP") == 0) 
		{
			//runs if user types help
			saveLog("HELP - shows help, END - end your turn"); //displays help
			saveLog("BUILDINGS - building options, type BUILDINGS for more help");
			saveLog("MORTGAGE/UNMORTGAGE - mortgage/unmortgage a property");
			saveLog("QUIT - quit the game");
			saveLog("SAVE - save the game and end your turn");
		}
		else if (strcmp(token[0], "BUILDINGS") == 0)
		{
			//runs if user types buildings
			returnValue = buySellHouses(token[1], token[2]); //calls buy sell houses and pushes through the paramaters that the user enterd

			if (returnValue == 2) //checks to see if there was a error
			{
				strcpy(log, "Error, invalid argument "); //diaplys error msg
				strcat(log, token[1]);
				saveLog(log);
				saveLog("BUILDINGS [BUY/SELL/PRICE/VIEW] [PROPERTY NAME (eg: OA)]");
				saveLog("When using view you do not need a property name");
				saveLog("Used to buy or sell houses/hotels on a property you own");
				saveLog("Buying a house on a property with four houses will build a hotel");
			}
			else if (returnValue == 3) //checks to see if there was a error
			{
				strcpy(log, "Error, invalid argument "); //diaplys error msg
				strcat(log, token[2]);
				saveLog(log);
				saveLog("BUILDINGS [BUY/SELL/PRICE/VIEW] [PROPERTY NAME (eg: OA)]");
				saveLog("When using view you do not need a property name");
				saveLog("Used to buy or sell houses/hotels on a property you own");
				saveLog("Buying a house on a property with four houses will build a hotel");
			}
		}
		else if (strcmp(token[0], "MORTGAGE") == 0)
		{
			morgage(token[1], 0); //calls mortgage function with pramaters enterd by user
		}
		else if (strcmp(token[0], "UNMORTGAGE") == 0)
		{
			morgage(token[1], 1); //calls mortgage function with pramaters enterd by user
		}
		else if (strcmp(token[0], "SAVE") == 0)
		{
			saveGame(); //calls saveGame
		}
		else if (strcmp(token[0], "QUIT") == 0)
		{
			quit = exitScreen(2); //calls quit screen (return 10 or 11 if user wants to quit)
			if (quit == 10 || quit == 11) { return quit; } //return value of quit if user wants to quit
		}

	} while (!(strcmp(token[0], "END") == 0)); //ends the users turn

	if (gm.rollagain == 0)
	{
		gm.numOfDoubles = 0; //resets the number of doubles rolled in a row veriable
		gm.activePlayer++; //changes whos turn it is
		if (gm.activePlayer > gm.playerAmount - 1)
		{
			gm.activePlayer = 0;
		}
	}


}

int buySellHouses(char *token1, char *token2) //used when the player would like to buy or sell houses on a property, or switch view mode/view prices
{	
	int i, propertyNumber, housePrice;
	char log[100],tempString[6];
	struct Bar properties;

	if (!(strcmp(token1, "BUY") == 0 || strcmp(token1, "SELL") == 0 || strcmp(token1, "VIEW") == 0 || strcmp(token1, "PRICE") == 0 )) //checsk to make sure the second argument enterd by the player is valid
	{
		return 2;
	}
	else if (strcmp(token1, "BUY") == 0)
	{
		//runs if the user said buy
		propertyNumber = getPropertNumber(token2); //gets the name of the property that the user enterd into a number
		if (propertyNumber != -1) //checks to make sure it was a valid proprty
		{
			if (!(propertyNumber == 0 || propertyNumber == 2 || propertyNumber == 4 || propertyNumber == 5 || propertyNumber == 7 || propertyNumber == 10 || propertyNumber == 12 || propertyNumber == 15 || propertyNumber == 17 || propertyNumber == 20 || propertyNumber == 22 || propertyNumber == 25 || propertyNumber == 28 || propertyNumber == 30 || propertyNumber == 33 || propertyNumber == 35 || propertyNumber == 36 || propertyNumber == 38 || propertyNumber == 40 || propertyNumber == 41 || propertyNumber == 42 || propertyNumber == 43 || propertyNumber == 44))
			{
				properties = getPropertiesOnStreet(propertyNumber); //returns the value of the other properties on the strret
				if (gm.PropertieOwner[properties.x] == gm.activePlayer && gm.PropertieOwner[properties.y] == gm.activePlayer && (gm.PropertieOwner[properties.z] == gm.activePlayer || properties.z == 0)) //make sure the user owns all the properties on the street
				{
					if (gm.sqaureStatus[properties.x] != 7 && gm.sqaureStatus[properties.y] != 7 && (gm.sqaureStatus[properties.z] != 7 || properties.z == 0)) //makes sure none of the properties are mortaged
					{
						if (gm.sqaureStatus[propertyNumber] != 6) //makes sure the proprties dosent have a hotel
						{
							if ((gm.sqaureStatus[properties.x] == gm.sqaureStatus[propertyNumber] || (gm.sqaureStatus[properties.x] == gm.sqaureStatus[propertyNumber] + 1)) && (gm.sqaureStatus[properties.y] == gm.sqaureStatus[propertyNumber] || (gm.sqaureStatus[properties.y] == gm.sqaureStatus[propertyNumber] + 1)) && (gm.sqaureStatus[properties.z] == gm.sqaureStatus[propertyNumber] || (gm.sqaureStatus[properties.z] == gm.sqaureStatus[propertyNumber] + 1) || properties.z == 0)) //amkes sure no street has two more houses then anyother property on the street
							{
								if (propertyNumber > 30) //gets the value of a house on that property														
								{
									housePrice = 200;
								}
								else if (propertyNumber > 20)
								{
									housePrice = 150;
								}
								else if (propertyNumber > 10)
								{
									housePrice = 100;
								}
								else
								{
									housePrice = 50;
								}

								if (gm.playerMoney[gm.activePlayer] < housePrice) //makes sure the user has enough money
								{
									saveLog("Error, you dont have enough cash"); //diaplys error msg
								}
								else
								{
									gm.sqaureStatus[propertyNumber]++; //upgrades poroperty
									gm.playerMoney[gm.activePlayer] -= housePrice;
									strcpy(log, "Successfully upgraded property ");
									strcat(log, gm.squareNames[propertyNumber]);
									saveLog(log); //prints msg
								}
							}
							else
							{
								saveLog("A property can not have two more houses then a property on the same street"); //display error msg
							}

						}
						else
						{
							saveLog("This property already has a hotel");//display error msg
						}
					}
					else
					{
						saveLog("one of the properties is mortgaged");//display error msg
					}

				}
				else
				{
					saveLog("You must own all proprties on the street");//display error msg
			

				}
			}
			else
			{
				saveLog("You can not build buildings on this square");//displays error msg
			}
		}
		else
		{
			return 3;
		}
	}
	else if (strcmp(token1, "SELL") == 0)
	{
		propertyNumber = getPropertNumber(token2); //gets the name of the property that the user enterd into a number
		if (propertyNumber != -1) //checks to make sure it was a valid proprty
		{
			if (!(propertyNumber == 0 || propertyNumber == 2 || propertyNumber == 4 || propertyNumber == 5 || propertyNumber == 7 || propertyNumber == 10 || propertyNumber == 12 || propertyNumber == 15 || propertyNumber == 17 || propertyNumber == 20 || propertyNumber == 22 || propertyNumber == 25 || propertyNumber == 28 || propertyNumber == 30 || propertyNumber == 33 || propertyNumber == 35 || propertyNumber == 36 || propertyNumber == 38 || propertyNumber == 40 || propertyNumber == 41 || propertyNumber == 42 || propertyNumber == 43 || propertyNumber == 44))
			{
				properties = getPropertiesOnStreet(propertyNumber); //returns the value of the other properties on the strret
				if (gm.sqaureStatus[propertyNumber] != 7) //amkes sure the propertie is unmortaged
				{
					if (gm.sqaureStatus[propertyNumber] != 1) //makes sure the property is owned
					{
						if ((gm.sqaureStatus[properties.x] == gm.sqaureStatus[propertyNumber] || (gm.sqaureStatus[properties.x] == gm.sqaureStatus[propertyNumber] - 1)) && (gm.sqaureStatus[properties.y] == gm.sqaureStatus[propertyNumber] || (gm.sqaureStatus[properties.y] == gm.sqaureStatus[propertyNumber] - 1)) && (gm.sqaureStatus[properties.z] == gm.sqaureStatus[propertyNumber] || (gm.sqaureStatus[properties.z] == gm.sqaureStatus[propertyNumber] - 1) || properties.z == 0)) //makes sure no property has two more houses then anyother propertie
						{
							if (propertyNumber > 30) //gets value of a house on that property
							{
								housePrice = 100;
							}
							else if (propertyNumber > 20)
							{
								housePrice = 75;
							}
							else if (propertyNumber > 10)
							{
								housePrice = 50;
							}
							else
							{
								housePrice = 25;
							}
							gm.sqaureStatus[propertyNumber]--; //downgades property
							gm.playerMoney[gm.activePlayer] += housePrice; //gives the player the money
							strcpy(log, "Sold house on "); //displays msg
							strcat(log, gm.squareNames[propertyNumber]);
							saveLog(log);
						}
						else
						{
							saveLog("A property can not have two more houses then a property on the same street"); //displays error msg
						}

					}
					else
					{
						saveLog("There are no houses or hotels on this property");//displays error msg
					}
				}
				else
				{
					saveLog("The property is mortgaged");//displays error msg
				}
			}
			else
			{
				printf("You can not sell buildings on this square");//displays error msg
			}

		}
		else
		{

		}
	}
	else if (strcmp(token1, "VIEW") == 0)
	{
		if (gm.showBuildingAmount == 0) //toggles showBuildingAmount
		{
			gm.showBuildingAmount = 1;
		}
		else
		{
			gm.showBuildingAmount = 0;
		}
	}
	else if (strcmp(token1, "PRICE") == 0)
	{
		propertyNumber = -1;
		for (i = 0; i < 40; i++) //gets the number value of the propertie enterd by the user
		{
			if (strcmp(gm.squareNames[i], token2) == 0)
			{
				propertyNumber = i;
			}
		}
		if (propertyNumber != -1) //checsk to make sure the propertie exists
		{
			//checsk to make sure the propertie can have houses
			if (propertyNumber == 0 || propertyNumber == 2 || propertyNumber == 4 || propertyNumber == 7 || propertyNumber == 10 || propertyNumber == 12 || propertyNumber == 15 || propertyNumber == 17 || propertyNumber == 20 || propertyNumber == 22 || propertyNumber == 25 || propertyNumber == 28 || propertyNumber == 30 || propertyNumber == 33 || propertyNumber == 35 || propertyNumber == 36 || propertyNumber == 38 || propertyNumber == 40 || propertyNumber == 41 || propertyNumber == 42 || propertyNumber == 43 || propertyNumber == 44)
			{
				saveLog("No price data avaliable"); //displays error msg
			}
			else
			{
				if (propertyNumber > 30) //gets price of a house on that street
				{
					housePrice = 200;
				}
				else if (propertyNumber > 20)
				{
					housePrice = 150;
				}
				else if (propertyNumber > 10)
				{
					housePrice = 100;
				}
				else
				{
					housePrice = 50;
				}

				strcpy(log, "Building houses on "); //displays all the infromation about that square
				strcat(log, gm.squareNames[propertyNumber]);
				strcat(log, " will cost $");
				itoa(housePrice, tempString, 10);
				strcat(log, tempString);
				strcat(log, ", and sell for $");
				itoa(housePrice/2, tempString, 10);
				strcat(log, tempString);
				saveLog(log);

				strcpy(log, "With no houses: $");
				itoa(gm.squarePrices[propertyNumber][1], tempString, 10);
				strcat(log, tempString);
				saveLog(log);

				strcpy(log, "  With 1 house: $");
				itoa(gm.squarePrices[propertyNumber][2], tempString, 10);
				strcat(log, tempString);
				saveLog(log);

				strcpy(log, " With 2 houses: $");
				itoa(gm.squarePrices[propertyNumber][3], tempString, 10);
				strcat(log, tempString);
				saveLog(log);

				strcpy(log, " With 3 houses: $");
				itoa(gm.squarePrices[propertyNumber][4], tempString, 10);
				strcat(log, tempString);
				saveLog(log);

				strcpy(log, " With 4 houses: $");
				itoa(gm.squarePrices[propertyNumber][5], tempString, 10);
				strcat(log, tempString);
				saveLog(log);

				strcpy(log, "    With Hotel: $");
				itoa(gm.squarePrices[propertyNumber][6], tempString, 10);
				strcat(log, tempString);
				saveLog(log);
			}
		}
		else
		{
			return 2;
		}
	}
	return 1;
}

int getPropertNumber(char *proprtyName) //gets number value of a property when a user enters a string
{
	int propertyNumber = -1, i, RRNumber = 0;
	char command[100], log[100];

	for (i = 0; i < 40; i++)
	{
		if (strcmp(gm.squareNames[i], proprtyName) == 0) //checks if the property name and the name enterd by the user match
		{
			propertyNumber = i; //sets property number to the square that was just checked
		}
	}

	if (propertyNumber == 35)
	{
		saveLog("Railroads are numbered clockwise, starting from GO");
		saveLog("Which RR did you mean (1-4): ");

		gets(command);
		strcpy(log, "> ");
		strcat(log, command);
		RRNumber = atoi(command);

		while (!(RRNumber == 1 || RRNumber == 2 || RRNumber == 3 || RRNumber == 4))
		{
			strcpy(log, "Error invalid number ");
			strcat(log, command);
			saveLog(log);

			gets(command);
			strcpy(log, "> ");
			strcat(log, command);

			RRNumber = atoi(command);
		}

		switch(RRNumber) {
		case 1:
			propertyNumber = 5;
			break;
		case 2:
			propertyNumber = 15;
			break;
		case 3:
			propertyNumber = 25;
			break;
		case 4:
			propertyNumber = 35;
			break;
		}

	}

	return propertyNumber;

}

void saveLog(char saveLogSentence[100]) //saves strings to be reprinted later
{

	int i = 0;

	strcpy(gm.consoleLog[gm.activePlayer][gm.logPosition[gm.activePlayer]], saveLogSentence); //saves the last sentance

	drawBoard(); //redraws board

	for (i = gm.logPosition[gm.activePlayer] + 1; i < gm.logLength; i++) //loops through what has been previously saved, and reprints everything before the last saved line
	{
		if (gm.consoleLog[gm.activePlayer][i] == NULL)
		{
			printf("\n");
		}
		else
		{
			printf("%s\n", gm.consoleLog[gm.activePlayer][i]);
		}

	}
	for (i = 0; i <= gm.logPosition[gm.activePlayer]; i++) //prints everything beyond the last saved line
	{
		if (gm.consoleLog[gm.activePlayer][i] == NULL)
		{
			printf("\n");
		}
		else
		{
			printf("%s\n", gm.consoleLog[gm.activePlayer][i]);
		}
	}
	gm.logPosition[gm.activePlayer]++; //ups the log position
	if (gm.logPosition[gm.activePlayer] == gm.logLength)
	{
		gm.logPosition[gm.activePlayer] = 0;
	}
}

void getPlayerNames() { //gets players to enter their names



	HWND console = GetConsoleWindow(); //used for resizing the window
	RECT r;

	char c, temp[100];
	int i;


	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 1100, 850, TRUE);    // sets console size and pos


	printf("  _    _                                                     _                          ___     _____        _  _ __  \n | |  | |                                                   | |                        |__ \\   / /__ \\      | || |\\ \\ \n | |__| | _____      __  _ __ ___   __ _ _ __  _   _   _ __ | | __ _ _   _  ___ _ __ ___  ) | | |   ) |_____| || |_| |\n |  __  |/ _ \\ \\ /\\ / / | '_ ` _ \\ / _` | '_ \\| | | | | '_ \\| |/ _` | | | |/ _ \\ '__/ __|/ /  | |  / /______|__   _| |\n | |  | | (_) \\ V  V /  | | | | | | (_| | | | | |_| | | |_) | | (_| | |_| |  __/ |  \\__ \\_|   | | / /_         | | | |\n |_|  |_|\\___/ \\_/\\_/   |_| |_| |_|\\__,_|_| |_|\\__, | | .__/|_|\\__,_|\\__, |\\___|_|  |___(_)   | ||____|        |_| | |\n                                                __/ | | |             __/ |                    \\_\\                /_/ \n                                               |___/  |_|            |___/                                            \n\n\n"); //ask how many player (betwwen 2 and 4)
	gets(temp);
	gm.playerAmount = atoi(temp);

	while (gm.playerAmount < 2 || gm.playerAmount > 4) // error traps
	{
		system("cls");
		printf("  _    _                                                     _                          ___     _____        _  _ __  \n | |  | |                                                   | |                        |__ \\   / /__ \\      | || |\\ \\ \n | |__| | _____      __  _ __ ___   __ _ _ __  _   _   _ __ | | __ _ _   _  ___ _ __ ___  ) | | |   ) |_____| || |_| |\n |  __  |/ _ \\ \\ /\\ / / | '_ ` _ \\ / _` | '_ \\| | | | | '_ \\| |/ _` | | | |/ _ \\ '__/ __|/ /  | |  / /______|__   _| |\n | |  | | (_) \\ V  V /  | | | | | | (_| | | | | |_| | | |_) | | (_| | |_| |  __/ |  \\__ \\_|   | | / /_         | | | |\n |_|  |_|\\___/ \\_/\\_/   |_| |_| |_|\\__,_|_| |_|\\__, | | .__/|_|\\__,_|\\__, |\\___|_|  |___(_)   | ||____|        |_| | |\n                                                __/ | | |             __/ |                    \\_\\                /_/ \n                                               |___/  |_|            |___/                                            \n\n\n"); //ask how many player (betwwen 2 and 4)
		printf("Error %d is not a valid player amount\n", gm.playerAmount);
		printf("How many players? (2-4)\n"); //ask how many player (betwwen 1 and 4)

		gets(temp);
		gm.playerAmount = atoi(temp);
	}

	//while ((c = getchar()) != '\n' && c != EOF);

	for (i = 0; i < gm.playerAmount; i++) //loops for all players
	{
		system("cls"); //displays ascii art per player
		if (i == 0) { printf("\nPPPPPPPPPPPPPPPPP     1111111   \nP::::::::::::::::P   1::::::1   \nP::::::PPPPPP:::::P 1:::::::1   \nPP:::::P     P:::::P111:::::1   \n  P::::P     P:::::P   1::::1   \n  P::::P     P:::::P   1::::1   \n  P::::PPPPPP:::::P    1::::1   \n  P:::::::::::::PP     1::::l   \n  P::::PPPPPPPPP       1::::l   \n  P::::P               1::::l   \n  P::::P               1::::l   \n  P::::P               1::::l   \nPP::::::PP          111::::::111\nP::::::::P          1::::::::::1\nP::::::::P          1::::::::::1\nPPPPPPPPPP          111111111111\n\n  ______       _              ___         _                          _                                           \n |  ____|     | |            |__ \\       | |                        | |                                        _ \n | |__   _ __ | |_ ___ _ __     ) |   ___| |__   __ _ _ __ __ _  ___| |_ ___ _ __   _ __   __ _ _ __ ___   ___(_)\n |  __| | '_ \\| __/ _ \\ '__|   / /   / __| '_ \\ / _` | '__/ _` |/ __| __/ _ \\ '__| | '_ \\ / _` | '_ ` _ \\ / _ \\  \n | |____| | | | ||  __/ |     / /_  | (__| | | | (_| | | | (_| | (__| ||  __/ |    | | | | (_| | | | | | |  __/_ \n |______|_| |_|\\__\\___|_|    |____|  \\___|_| |_|\\__,_|_|  \\__,_|\\___|\\__\\___|_|    |_| |_|\\__,_|_| |_| |_|\\___(_)\n\n"); }
		if (i == 1) { printf("\nPPPPPPPPPPPPPPPPP    222222222222222    \nP::::::::::::::::P  2:::::::::::::::22  \nP::::::PPPPPP:::::P 2::::::222222:::::2 \nPP:::::P     P:::::P2222222     2:::::2 \n  P::::P     P:::::P            2:::::2 \n  P::::P     P:::::P            2:::::2 \n  P::::PPPPPP:::::P          2222::::2  \n  P:::::::::::::PP      22222::::::22   \n  P::::PPPPPPPPP      22::::::::222     \n  P::::P             2:::::22222        \n  P::::P            2:::::2             \n  P::::P            2:::::2             \nPP::::::PP          2:::::2       222222\nP::::::::P          2::::::2222222:::::2\nP::::::::P          2::::::::::::::::::2\nPPPPPPPPPP          22222222222222222222\n\n  ______       _              ___         _                          _                                           \n |  ____|     | |            |__ \\       | |                        | |                                        _ \n | |__   _ __ | |_ ___ _ __     ) |   ___| |__   __ _ _ __ __ _  ___| |_ ___ _ __   _ __   __ _ _ __ ___   ___(_)\n |  __| | '_ \\| __/ _ \\ '__|   / /   / __| '_ \\ / _` | '__/ _` |/ __| __/ _ \\ '__| | '_ \\ / _` | '_ ` _ \\ / _ \\  \n | |____| | | | ||  __/ |     / /_  | (__| | | | (_| | | | (_| | (__| ||  __/ |    | | | | (_| | | | | | |  __/_ \n |______|_| |_|\\__\\___|_|    |____|  \\___|_| |_|\\__,_|_|  \\__,_|\\___|\\__\\___|_|    |_| |_|\\__,_|_| |_| |_|\\___(_)\n\n"); }
		if (i == 2) { printf("\nPPPPPPPPPPPPPPPPP    333333333333333   \nP::::::::::::::::P  3:::::::::::::::33 \nP::::::PPPPPP:::::P 3::::::33333::::::3\nPP:::::P     P:::::P3333333     3:::::3\n  P::::P     P:::::P            3:::::3\n  P::::P     P:::::P            3:::::3\n  P::::PPPPPP:::::P     33333333:::::3 \n  P:::::::::::::PP      3:::::::::::3  \n  P::::PPPPPPPPP        33333333:::::3 \n  P::::P                        3:::::3\n  P::::P                        3:::::3\n  P::::P                        3:::::3\nPP::::::PP          3333333     3:::::3\nP::::::::P          3::::::33333::::::3\nP::::::::P          3:::::::::::::::33 \nPPPPPPPPPP           333333333333333   \n\n  ______       _              ___         _                          _                                           \n |  ____|     | |            |__ \\       | |                        | |                                        _ \n | |__   _ __ | |_ ___ _ __     ) |   ___| |__   __ _ _ __ __ _  ___| |_ ___ _ __   _ __   __ _ _ __ ___   ___(_)\n |  __| | '_ \\| __/ _ \\ '__|   / /   / __| '_ \\ / _` | '__/ _` |/ __| __/ _ \\ '__| | '_ \\ / _` | '_ ` _ \\ / _ \\  \n | |____| | | | ||  __/ |     / /_  | (__| | | | (_| | | | (_| | (__| ||  __/ |    | | | | (_| | | | | | |  __/_ \n |______|_| |_|\\__\\___|_|    |____|  \\___|_| |_|\\__,_|_|  \\__,_|\\___|\\__\\___|_|    |_| |_|\\__,_|_| |_| |_|\\___(_)\n\n"); }
		if (i == 3) { printf("\nPPPPPPPPPPPPPPPPP        444444444  \nP::::::::::::::::P      4::::::::4  \nP::::::PPPPPP:::::P    4:::::::::4  \nPP:::::P     P:::::P  4::::44::::4  \n  P::::P     P:::::P 4::::4 4::::4  \n  P::::P     P:::::P4::::4  4::::4  \n  P::::PPPPPP:::::P4::::4   4::::4  \n  P:::::::::::::PP4::::444444::::444\n  P::::PPPPPPPPP  4::::::::::::::::4\n  P::::P          4444444444:::::444\n  P::::P                    4::::4  \n  P::::P                    4::::4  \nPP::::::PP                  4::::4  \nP::::::::P                44::::::44\nP::::::::P                4::::::::4\nPPPPPPPPPP                4444444444\n\n  ______       _              ___         _                          _                                           \n |  ____|     | |            |__ \\       | |                        | |                                        _ \n | |__   _ __ | |_ ___ _ __     ) |   ___| |__   __ _ _ __ __ _  ___| |_ ___ _ __   _ __   __ _ _ __ ___   ___(_)\n |  __| | '_ \\| __/ _ \\ '__|   / /   / __| '_ \\ / _` | '__/ _` |/ __| __/ _ \\ '__| | '_ \\ / _` | '_ ` _ \\ / _ \\  \n | |____| | | | ||  __/ |     / /_  | (__| | | | (_| | | | (_| | (__| ||  __/ |    | | | | (_| | | | | | |  __/_ \n |______|_| |_|\\__\\___|_|    |____|  \\___|_| |_|\\__,_|_|  \\__,_|\\___|\\__\\___|_|    |_| |_|\\__,_|_| |_| |_|\\___(_)\n\n"); }
		gets(temp);
		
		if (temp[0] == NULL) { (temp[0] = '_'); } //replaces illegal characters
		if (temp[1] == NULL) { (temp[1] = '_'); }

		if (temp[0] == ' ') { (temp[0] = '_'); }
		if (temp[1] == ' ') { (temp[1] = '_'); }

		if (temp[0] == '~') { (temp[0] = '-'); }
		if (temp[1] == '~') { (temp[1] = '-'); }

		gm.playerNames[i][0] = temp[0];
		gm.playerNames[i][1] = temp[1];
	}

}

void move() { //moves the player based on the dice rolled

	char log[100];
	char temp[5];
	int position = 0;

	gm.playerLocation[gm.activePlayer] += (gm.dice[0] + gm.dice[1]); //moves the player
	if (gm.playerLocation[gm.activePlayer] >= 40) //checks if user has passed go
	{
		position = gm.playerLocation[gm.activePlayer]; //stops them on go to collect money
		gm.playerLocation[gm.activePlayer] = 0;
		strcpy(log, gm.playerNames[gm.activePlayer]);
		strcat(log, ", you passed go!");
		saveLog(log);
		saveLog("press ENTER to collect $200");
		fflush(stdin);
		while ( getchar() != '\n' );
		gm.playerLocation[gm.activePlayer] = position - 40;
		gm.playerMoney[gm.activePlayer] += 200;
		gm.timesPassedGo[gm.activePlayer] ++;
	}

	if (gm.dice[0] == gm.dice[1]) { //checks for doubles
		strcpy(log, gm.playerNames[gm.activePlayer]); //prints the players roll
		strcat(log, ", you rolled double ");
		strcat(log, itoa(gm.dice[0], temp, 10));
		strcat(log, "'s and landed on ");
		strcat(log, gm.squareNames[gm.playerLocation[gm.activePlayer]]);
		saveLog(log);

	}
	else
	{
		strcpy(log, gm.playerNames[gm.activePlayer]); //prints player roll
		strcat(log, ", you rolled a ");
		strcat(log, itoa(gm.dice[0], temp, 10));
		strcat(log, " and a ");
		strcat(log, itoa(gm.dice[1], temp, 10));
		strcat(log, " and landed on ");
		strcat(log, gm.squareNames[gm.playerLocation[gm.activePlayer]]);
		saveLog(log);
	}
	if (gm.playerLocation[gm.activePlayer] == 10) { saveLog("Say \"Hi\" to Jake the Jailbird"); } //if they land on just visiting, display message

}

void rollDice() { //rolls the dice

	char log[100];
	if (gm.rollagain == 0)
	{
		strcpy(log, gm.playerNames[gm.activePlayer]); //gets user to press enter
		strcat(log,", press ENTER to roll the dice...");
		saveLog(log);
		fflush(stdin);
		while ( getchar() != '\n' ); //waits for enter key

		gm.dice[0] = rand() % 6 + 1; //rolls dice
		gm.dice[1] = rand() % 6 + 1;

		if (gm.dice[0] == gm.dice[1])
		{
			gm.rollagain = 1; //checks if they got doubles
			gm.numOfDoubles ++;
		}
		else { gm.rollagain = 0; } //resets roll again if they dont roll doubles
	}
	else
	{
		strcpy(log, gm.playerNames[gm.activePlayer]); //prompts user to press enter
		strcat(log," rolled doubles, press ENTER key to roll the dice again...");
		saveLog(log);
		fflush(stdin);
		while ( getchar() != '\n' ); //waits for enter key

		gm.dice[0] = rand() % 6 + 1; //rolls dice
		gm.dice[1] = rand() % 6 + 1;

		if (gm.dice[0] == gm.dice[1]) 
		{
			gm.rollagain = 1; //checks if they got doubles
			gm.numOfDoubles ++;
			if (gm.numOfDoubles == 3) //if they roll doubles three times send em to jail
			{
				strcpy(log, gm.playerNames[gm.activePlayer]); //prints message
				strcat(log, ", you got sent to jail for rolling doubles thrice in a row");
				saveLog(log);
				saveLog("Press ENTER to contiune...");
				while (getchar() != '\n');

				gm.jailed[gm.activePlayer] = 1; //changes there state to jailed
				gm.playerLocation[gm.activePlayer] = (40 + gm.activePlayer); //moves them into jail
				gm.rollagain = 0;
				gm.numOfDoubles = 0;
			}
		} 	
		else 
		{ 
			gm.rollagain = 0;
			gm.numOfDoubles = 0;
		}
	}
}

int checkSquares() { //will call functions based on what square the player lands on

	int quit = 0;

	switch (gm.playerLocation[gm.activePlayer])
	{
	case 0:
		break; //landed on go!
	case 2: //First community chest
		quit = specialSquares(1);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 4: //Tax
		specialSquares(4);
		break;
	case 7: //First chance
		quit = specialSquares(2);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 10: //just visiting
		break;
	case 17: //2nd community chest
		quit = specialSquares(1);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 20: //free parking
		break;
	case 22: //2nd chance
		quit = specialSquares(2);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 30: //Go to jail
		quit = specialSquares(3);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 33: //3rd community chest
		quit = specialSquares(1);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 36: //3rd chance
		quit = specialSquares(1);
		if (quit == 10 || quit == 11) { return quit; }
		break;
	case 38:
		specialSquares(5);
		break;
	case 40: //these are all jail squares
	case 41:
	case 42:
	case 43:
		//YOU IN JAIL
		break;
	default: //if you did not land on any of the above sqaures you must be on a property
		quit = landOnProperty();
		if (quit == 10 || quit == 11) { return quit; }
	}

}

int landOnProperty() { //runs when a player lands on a property square

	char command[100]; //saves commands a user enters
	char log[100],temp[20]; //saves stuff that needs to be printed to the log
	int quit; //saves whther or not the user wants to quit
	int amountOwed; //used when the player lands on water works or electric company


	if (gm.sqaureStatus[gm.playerLocation[gm.activePlayer]] == 0) { //checks that the square is unowned, which is 0

		do {
			strcpy(log, gm.playerNames[gm.activePlayer]); //creates the string to be printed to the log
			strcat(log, ", ");
			strcat(log, gm.squareNames[gm.playerLocation[gm.activePlayer]]);
			strcat(log," is unowned, do you want to buy it? (type help for help): "); //asks user if they want to buy it if it is unowned
			saveLog(log); // saves log line
			
			printf("> "); //gets the user input, saves it to the log and uppercases it
			gets(command);
			strcpy(log, "> ");
			strcat(log,command);
			saveLog(log);
			strupr(command);

			while (!((strcmp(command, "HELP") == 0) || (strcmp(command, "Y") == 0) || (strcmp(command, "N") == 0) || (strcmp(command, "QUIT") == 0))) //checks the user entered a valid command
			{
				saveLog("Error unknown command"); // display error and ask for command again if they entered an invalid one
				strcpy(log, gm.playerNames[gm.activePlayer]);
				strcat(log, ", enter command (type help for help): ");
				saveLog(log);
				printf("> ");
				gets(command);
				strcpy(log, "> ");
				strcat(log, command);
				strupr(command);
			}

			if (strcmp(command, "HELP") == 0)	// if they entered "HELP" and prints help
			{
				saveLog("Y - buy the sqaure, N - Don't buy the sqaure");
				saveLog("HELP - shows help, QUIT - quits the game");

			}
			else if (strcmp(command, "Y") == 0)	// "Y" means they want to buy the property
			{
				if (gm.playerMoney[gm.activePlayer] < gm.squarePrices[gm.playerLocation[gm.activePlayer]][0]) // checks that they have enough money
				{

					strcpy(log, gm.playerNames[gm.activePlayer]); // displays error if they don't
					strcat(log, ", you don't have enough money");
					saveLog(log);

				}
				else
				{
					gm.playerMoney[gm.activePlayer] -= gm.squarePrices[gm.playerLocation[gm.activePlayer]][0];	//subtracts money from player
					gm.sqaureStatus[gm.playerLocation[gm.activePlayer]] = 1;									//sets the square as owned
					gm.PropertieOwner[gm.playerLocation[gm.activePlayer]] = gm.activePlayer;					//sets the active player as the property owner
					if (gm.playerLocation[gm.activePlayer] == 5 || gm.playerLocation[gm.activePlayer] == 15 || gm.playerLocation[gm.activePlayer] == 25 || gm.playerLocation[gm.activePlayer] == 35) { calculateRail(); } //checks if its a railroad, and if so, calls the checkRail function
					strcpy(log, gm.playerNames[gm.activePlayer]); //displays message showing they bought it
					strcat(log, ", bought ");
					strcat(log, gm.squareNames[gm.playerLocation[gm.activePlayer]]);
					saveLog(log);
				
				}
			}
			else if (strcmp(command, "N") == 0) // "N" means they dont want to buy it
			{
				strcpy(log,"You chose not to buy "); //displays message showing they didn't buy it
				strcat(log, gm.squareNames[gm.playerLocation[gm.activePlayer]]);
				saveLog(log);
			}
			else if ((strcmp(command, "QUIT") == 0)) // "QUIT" means they want to quit
			{
				quit = exitScreen(1); //calls exitScreen function
				if (quit == 10 || quit == 11) { return quit; } //returns value of choice on exit screen
			}
		} while (!((strcmp(command, "Y") == 0) || (strcmp(command, "N") == 0))); //checks to make sure the user said "Y" or "N"

	}
	else if (gm.PropertieOwner[gm.playerLocation[gm.activePlayer]] != gm.activePlayer) //if the square is owned, it enters here
	{
		if (gm.jailed[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]] == 1 && gm.collectRentInJail == 0) //checks if the owner of the property is in jail and if collect money in jail is enabled
		{
			strcpy(log, gm.playerNames[gm.activePlayer]); //displays message that the player doesn't owe money
			strcat(log, ", does not owe ");
			strcat(log, gm.playerNames[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]]);
			strcat(log, " anything becasue they are in jail");
			saveLog(log);
		}
		else
		{
			if (gm.playerLocation[gm.activePlayer] == 12 || gm.playerLocation[gm.activePlayer] == 28) //checks if it's a utility
			{
				if (gm.PropertieOwner[12] == gm.PropertieOwner[28]) //checks if the user needs to be charged x4 or x10 their roll
				{
					amountOwed = (gm.dice[0] + gm.dice[1]) * 10; //calcs amount owed
					strcpy(log, gm.playerNames[gm.activePlayer]); //creates the string to be printed to the log
					strcat(log, ", owes ");
					strcat(log, gm.playerNames[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]]);
					strcat(log, " 10x their dice roll");
				}
				else
				{
					amountOwed = (gm.dice[0] + gm.dice[1]) * 4; //calcs amount owed
					strcpy(log, gm.playerNames[gm.activePlayer]); //creates the string to be printed to the log
					strcat(log, ", owes ");
					strcat(log, gm.playerNames[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]]);
					strcat(log, " 4x their dice roll");
				}
				saveLog(log);
				strcpy(log, gm.playerNames[gm.activePlayer]); //creates the string to be printed to the log
				strcat(log, ", owes ");
				strcat(log, gm.playerNames[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]]);
				strcat(log, " $");
				itoa(amountOwed, temp, 10);
				strcat(log, temp);
				saveLog(log);
				saveLog("Press Enter to pay...");
				fflush(stdin);
				while (getchar() != '\n');
				gm.playerMoney[gm.activePlayer] -= amountOwed; //charges the amount owed
				gm.playerMoney[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]] += amountOwed; //gives money to property owner
				quit = checkIfBankrupt(0, gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]); //checks if user has negative money (bankrupt)
				if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			}
			else //enters here if it's a regular property
			{
				strcpy(log, gm.playerNames[gm.activePlayer]); //creates the string to be printed to the log
				strcat(log, ", owes ");
				strcat(log, gm.playerNames[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]]);
				strcat(log, " $");
				itoa(gm.squarePrices[gm.playerLocation[gm.activePlayer]][gm.sqaureStatus[gm.playerLocation[gm.activePlayer]]], temp, 10);
				strcat(log, temp);
				strcat(log, " for landing on ");
				strcat(log, gm.squareNames[gm.playerLocation[gm.activePlayer]]);
				saveLog(log);

				saveLog("Press Enter to pay...");
				fflush(stdin);
				while (getchar() != '\n');
				gm.playerMoney[gm.activePlayer] -= gm.squarePrices[gm.playerLocation[gm.activePlayer]][gm.sqaureStatus[gm.playerLocation[gm.activePlayer]]]; //charges player
				gm.playerMoney[gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]] += gm.squarePrices[gm.playerLocation[gm.activePlayer]][gm.sqaureStatus[gm.playerLocation[gm.activePlayer]]]; //gives money to property owner
				quit = checkIfBankrupt(0, gm.PropertieOwner[gm.playerLocation[gm.activePlayer]]); //checks if player went into negative money
				if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			}


		}
	}
	else //enters here if they land on their own property
	{
		saveLog("Safe at home, you don't pay rent."); //displays message
	}

}

int specialSquares(int specialSqaure) { //called when user lands on non-purchasable squares

	int drawnCard, i, quit = 0;
	int amountToPay; //used for street repairs;
	char log[100], temp[5] = {0,0,0,0,0};

	if (specialSqaure == 1 || specialSqaure == -1) //landed on community chest
	{
		if (specialSqaure == 1)
		{
			strcpy(log, gm.playerNames[gm.activePlayer]); //prompts the user for enter
			strcat(log, ", Press ENTER to draw a card...");
			saveLog(log);
			while (getchar() != '\n');
		}


		drawnCard = (gm.communityChestCards[gm.communityChestDeckLocation]) + 20; //ups by 20 if its a chest card as to not be confused with chance
		switch (drawnCard)
		{
		case 20: //go to go
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Go to Go");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerLocation[gm.activePlayer] = 0;
			gm.playerMoney[gm.activePlayer] += 200;
			break;
		case 21: //bank error
			strcat(log, ", bank error in your favor, collect $200");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 200;
			strcpy(log, gm.playerNames[gm.activePlayer]);
			break;
		case 22: //doctors fees
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", pay doctor's fee of $50");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] -= 50;
			quit = checkIfBankrupt(0, -1);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 23: //sale of stock
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", from sale of stock you get $50");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 50;
			break;
		case 24: //got out of jail free
			if (gm.communityGOOJFUsed == 1) //checks if another player has a get out of jail free card
			{
				gm.communityChestDeckLocation++; //draws a new card
				if (gm.communityChestDeckLocation > 16) {
					mixDecks(2);
				}
				specialSquares(-1);
				return;
			}
			else
			{
				strcpy(log, gm.playerNames[gm.activePlayer]);
				strcat(log, ", you got a Get Out Of Jail Free card");
				saveLog(log);
				saveLog("Press ENTER to continue...");
				while (getchar() != '\n');
				gm.communityGOOJFUsed = 1; 
				gm.communityGOOJF[gm.activePlayer] = 1; //gives player the get out of jail free card

			}
			break;
		case 25: // go to jail
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", go to Jail");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerLocation[gm.activePlayer] = 40 + gm.activePlayer;
			gm.rollagain = 0;
			gm.jailed[gm.activePlayer] = 1;
			break;
		case 26: //opera house opening
			saveLog("Grand Opera Night Opening, collect $50 from each player");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += (50 * (gm.playerAmount - 1));
			strcpy(log, gm.playerNames[gm.activePlayer]);
			for (i = 0; i < gm.playerAmount; i++)
			{
				if (i != gm.activePlayer)
				{
					gm.playerMoney[i] -= 50;
				}
			}

			quit = checkIfBankrupt(1, gm.activePlayer);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 27: //holiday funds
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Holiday Fund matures - Receive $100");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 100;

			break;
		case 28: //income tax refund
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Income tax refund - Collect $20");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 20;
			break;
		case 29: //its your birthday
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", it is your birthday - Collect $10 From Each Player");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 10 * (gm.playerAmount - 1);
			for (i = 0; i < gm.playerAmount; i++)
			{
				if (i != gm.activePlayer)
				{
					gm.playerMoney[i] -= 10;
				}
			}

			quit = checkIfBankrupt(1, gm.activePlayer);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 30: //life insurance matures
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Life insurance matures - Collect $100");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 100;

			break;
		case 31: //hospital fees
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Pay Hospital Fees Of $100");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] -= 100;

			quit = checkIfBankrupt(0, -1);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 32: //school fees
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Pay school fees of $150");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] -= 150;

			quit = checkIfBankrupt(0, -1);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 33: //recive 25
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Receive $25 consultancy fee");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 25;

			break;
		case 34: //street repairs
			amountToPay = 0;
			for (i = 0; i < 40; i++)
			{
				if (gm.PropertieOwner[i] == gm.activePlayer) //loops through all properties owned by this player
				{
					if (gm.sqaureStatus[i] > 1 && gm.sqaureStatus[i] < 6) //checks to see how many houses are on that property
						amountToPay += ((gm.sqaureStatus[i] - 1) * 40); //charges player 40$ per house
					else if (gm.sqaureStatus[i] == 6)
					{
						amountToPay += 115; //charges 115$ per hotel
					}
				}
			}
			strcpy(log, gm.playerNames[gm.activePlayer]); //prints out how much they have to pay
			strcat(log, ", you Are Assessed $");
			strcat(log, itoa(amountToPay, temp, 10));
			strcat(log, "For Street Repairs");
			saveLog(log);
			saveLog("$40 Per House and $115 Per Hotel");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');

			gm.playerMoney[gm.activePlayer] -= amountToPay; //charges them

			quit = checkIfBankrupt(0, -1); //checks if they went bankrupt
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 35: //second place in contest
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", you have won second prize in a beauty contest - Collect $10");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 10;

			break;
		case 36: //inherate 100
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", you inherit $100");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 100;

			break;
		}
		gm.communityChestDeckLocation++; //moves to next card
		if (gm.communityChestDeckLocation > 16) { //shuffles at the end of deck
			mixDecks(2);
		}
	}
	else if (specialSqaure == 2 || specialSqaure == -2) //landed on chance
	{

		if (specialSqaure == 2)
		{
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Press ENTER to draw a card...");
			saveLog(log);
			while (getchar() != '\n');
		}

		drawnCard = gm.chanceCards[gm.chanceDeckLocation];

		switch (drawnCard)
		{
		case 0: //go to go
			saveLog("Advance to Go!");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.playerLocation[gm.activePlayer] != 0) { gm.timesPassedGo[gm.activePlayer] ++; }
			gm.playerLocation[gm.activePlayer] = 0;
			gm.playerMoney[gm.activePlayer] += 200;

			break;
		case 1: //go to illinois
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Advance To Illinois Ave.");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.playerLocation[gm.activePlayer] > 24) //checks if user passes go
			{
				gm.playerMoney[gm.activePlayer] += 200; 
				gm.timesPassedGo[gm.activePlayer] ++;
			}
			gm.playerLocation[gm.activePlayer] = 24;

			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 2: //go to illinois
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Advance To St. Charles Place");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.playerLocation[gm.activePlayer] > 11) //checks if user passes go
			{
			gm.playerMoney[gm.activePlayer] += 200;
			gm.timesPassedGo[gm.activePlayer] ++;
			} 
			gm.playerLocation[gm.activePlayer] = 11;

			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 3:
			saveLog("Advance Token to Nearest Utility");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.playerLocation[gm.activePlayer] >= 28) //checks if user passes go
			{
				gm.playerMoney[gm.activePlayer] += 200;
				gm.timesPassedGo[gm.activePlayer] ++;
			}
			if (gm.playerLocation[gm.activePlayer] < 13 || gm.playerLocation[gm.activePlayer] > 27) //checks which utility they're nearest to
			{
				gm.playerLocation[gm.activePlayer] = 12;
			}
			else
			{
				gm.playerLocation[gm.activePlayer] = 28;
			}

			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 4:
			saveLog("Advance Token To Nearest Railroad!");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.playerLocation[gm.activePlayer] < 5 || gm.playerLocation[gm.activePlayer] > 34) //checks which railroad they are closest to
			{
				if (gm.playerLocation[gm.activePlayer] > 34)  //checks if user passes go
				{
					gm.playerMoney[gm.activePlayer] += 200;
					gm.timesPassedGo[gm.activePlayer] ++;
				}
					gm.playerLocation[gm.activePlayer] = 5;
			}
			else if (gm.playerLocation[gm.activePlayer] < 15) //checks which railroad they are closest to
			{
				gm.playerLocation[gm.activePlayer] = 15;
			}
			else if (gm.playerLocation[gm.activePlayer] < 25) //checks which railroad they are closest to
			{
				gm.playerLocation[gm.activePlayer] = 25;
			}
			else if (gm.playerLocation[gm.activePlayer] > 35) //checks which railroad they are closest to
			{
				gm.playerLocation[gm.activePlayer] = 35;
			}

			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 5: //bank pays dividend
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Bank pays you dividend of $50");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 50;

			break;
		case 6: //got out of jail free
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", you got a Get Out Of Jail Free card");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.chanceGOOJFUsed == 1) //checks if another player has a get out of jail free card
			{
				gm.chanceDeckLocation++; //draws a new card
				if (gm.chanceDeckLocation > 15) {
					mixDecks(2);
				}
				specialSquares(-2);
				return;
			}
			else
			{
				gm.chanceGOOJFUsed = 1;
				gm.chanceGOOJF[gm.activePlayer] = 1; //gives player the get out of jail free card

			}
			break;
		case 7:
			saveLog("Go back three spaces");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerLocation[gm.activePlayer] -= 3;
			if (gm.playerLocation[gm.activePlayer] < 0) { //checks if they move back past go

				gm.playerLocation[gm.activePlayer] += 40;

			}
			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 8: // go to jail
			strcpy(log, gm.playerNames[gm.activePlayer]);
			strcat(log, ", Go To Jail");
			saveLog(log);
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerLocation[gm.activePlayer] = 40 + gm.activePlayer;
			gm.rollagain = 0;
			gm.jailed[gm.activePlayer] = 1;

			break;
		case 9: //make general repairs to you property
			amountToPay = 0;
			for (i = 0; i < 40; i++)
			{
				if (gm.PropertieOwner[i] == gm.activePlayer) //loops through all player's properties
				{
					if (gm.sqaureStatus[i] > 1 && gm.sqaureStatus[i] < 6) //checks for houses on property
						amountToPay += ((gm.sqaureStatus[i] - 1) * 25); //charges 25 for each house
					else if (gm.sqaureStatus[i] == 6)
					{
						amountToPay += 100; // charges 100 for a hotel
					}
				}
			}
			
			strcpy(log, gm.playerNames[gm.activePlayer]); //prints message
			strcat(log, ", you Spent $");
			strcat(log, itoa(amountToPay, temp, 10));
			strcat(log, " On General Repairs To Your Poperties");
			saveLog(log);
			saveLog("$25 Per House and $100 Per Hotel");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] -= amountToPay; //charges player
			quit = checkIfBankrupt(0, -1); //checks if they're bankrupt
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
		case 10: //poor tax
			saveLog("Pay Poor Tax Of $15");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] -= 15;
			quit = checkIfBankrupt(0, -1);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 11: //go to reading railroad
			saveLog("Take A Ride On The Reading Railroad");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			if (gm.playerLocation[gm.activePlayer] > 5)
			{
				gm.playerLocation[gm.activePlayer] += 200;
				gm.timesPassedGo[gm.activePlayer] ++;
			}
			gm.playerLocation[gm.activePlayer] = 5;

			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 12: //go to boardwalk
			saveLog("Take A Walk On Boardwalk");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');

			if (gm.playerLocation[gm.activePlayer] >= 39)
			{
				gm.playerMoney[gm.activePlayer] += 200;
				gm.timesPassedGo[gm.activePlayer] ++;
			}

			gm.playerLocation[gm.activePlayer] = 39;

			quit = checkSquares();
			if (quit == 10 || quit == 11) { return quit; }
			break;
		case 13: //chairman of the board
			saveLog("You Have Been Elected Chairman Of The Board - Collect $50 from each player");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] -= (50 * (gm.playerAmount - 1));
			for (i = 0; i < gm.playerAmount; i++)
			{
				if (i != gm.activePlayer) { gm.playerMoney[i] += 50; }
			}

			quit = checkIfBankrupt(1, gm.activePlayer);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
			break;
		case 14: //building loan matures
			saveLog("Your Building Loan Matures - Collect $150");
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			gm.playerMoney[gm.activePlayer] += 150;
			break;
		case 15: //crossword competition won
			saveLog("You have won a crossword competition - Collect $100 ");
			gm.playerMoney[gm.activePlayer] += 150;
			saveLog("Press ENTER to continue...");
			while (getchar() != '\n');
			break;
		}
		gm.chanceDeckLocation++; //goes to next chance card
		if (gm.chanceDeckLocation > 15) //shuffles if at the end of the deck
		{
			mixDecks(1);
		}
		return 1;

	}
	else if (specialSqaure == 3) //goto jail
	{
		gm.playerLocation[gm.activePlayer] = 40 + gm.activePlayer;
		gm.jailed[gm.activePlayer] = 1;
		gm.rollagain = 0;
	}
	else if (specialSqaure == 4) //income tax
	{
		strcpy(log, gm.playerNames[gm.activePlayer]);
		strcat(log, ", you landed on income tax!");
		saveLog(log);
		if ((gm.playerMoney[gm.activePlayer] / 10) < 200) //checks if the user has more than 2000$
		{
			strcpy(log, "You owe 10%, which is $"); //prints message
			strcat(log, itoa(gm.playerMoney[gm.activePlayer] / 10 , temp , 10));
			saveLog(log);
			saveLog("Press ENTER to pay");
			while ( getchar() != '\n' );
			gm.playerMoney[gm.activePlayer] -= gm.playerMoney[gm.activePlayer] / 10; //charges 10% if they have less than 2k
			quit = checkIfBankrupt(0,-1);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
		}
		else
		{
			saveLog("You owe $200"); //prints message
			saveLog("Press ENTER to pay");
			while ( getchar() != '\n' );
			gm.playerMoney[gm.activePlayer] -= 200; //charges 200 if they have more than 2k
			quit = checkIfBankrupt(0,-1);
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
		}
	}
	else if (specialSqaure == 5) //luxury tax
	{
		strcpy(log, gm.playerNames[gm.activePlayer]); //prints message
		strcat(log, ", you landed on luxury tax!");
		saveLog(log);
		saveLog("Press ENTER to pay");
		while (getchar() != '\n');
		gm.playerMoney[gm.activePlayer] -= 75; //charges 75$
		quit = checkIfBankrupt(0, -1);
		if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
	}

}

void mixDecks(int decksToMix) { //shuffles the chance and chest decks

	int i, done = 0, temp, index;

	if (decksToMix == 0) {
		for (i = 0; i < 16; i++) //goes through each card and swaps it with another random card
		{
			index = rand() % 16; //generates the location to swap the card with
			temp = gm.chanceCards[i]; //swaps them
			gm.chanceCards[i] = gm.chanceCards[index];
			gm.chanceCards[index] = temp;

			index = rand() % 17; //generates location for chest
			gm.communityChestCards[i] = temp; //swaps chest cards
			gm.communityChestCards[i] = gm.communityChestCards[index];
			gm.communityChestCards[index] = temp;

			if (i == 15) //communinty chest has an extra card so need to be done an extra time
			{
				i = 16;
				index = rand() % 17;
				temp = gm.communityChestCards[i];
				gm.communityChestCards[i] = gm.communityChestCards[index];
				gm.communityChestCards[index] = temp;
			}


		}
		gm.communityChestDeckLocation = 0; //resets deck positions
		gm.chanceDeckLocation = 0;

	}
	else if (decksToMix == 1) { //just shuffle chance
		for (i = 0; i < 15; i++)
		{
			index = rand() % 16;
			temp = gm.chanceCards[i];
			gm.chanceCards[i] = gm.chanceCards[index];
			gm.chanceCards[index] = temp;
		}
		gm.chanceDeckLocation = 0;
	}
	else if (decksToMix == 2) //just shuffles chest
	{
		for (i = 0; i < 16; i++)
		{
			index = rand() % 17;
			temp = gm.communityChestCards[i];
			gm.communityChestCards[i] = gm.communityChestCards[index];
			gm.communityChestCards[index] = temp;

		}
		gm.communityChestDeckLocation = 0;

	}
}

void sortDecks() { //creates decks at the beginning of the game
	
	int i;

	for (i = 0; i < 16; i++) //loops through and creates cards
	{
		gm.chanceCards[i] = i;
		gm.communityChestCards[i] = i;
					if (i == 15) //communinty chest has an extra card so need to be done an extra time
		{
			i = 16;
			gm.communityChestCards[i] = i;
		}
	}
}

void calculateRail() //calculates how much each player needs to pay on all railroads
{
	int i;
	int numOfRailroadOwned[4] = {0,0,0,0};


	if (gm.PropertieOwner[5] != -1) //checks owner of railroad, and increments number of railroads owned by them by one
	{
			numOfRailroadOwned[gm.PropertieOwner[5]]++;
	}
	if (gm.PropertieOwner[15] != -1)
	{
			numOfRailroadOwned[gm.PropertieOwner[15]]++;
	}
	if (gm.PropertieOwner[25] != -1)
	{
			numOfRailroadOwned[gm.PropertieOwner[25]]++;
	}
	if (gm.PropertieOwner[35] != -1)
	{
			numOfRailroadOwned[gm.PropertieOwner[35]]++;
	}
	
		if (gm.PropertieOwner[5] != -1) //goes through and calcs price for each railroad
		{
			gm.squarePrices[5][1] = 25 * pow((float)2, (numOfRailroadOwned[gm.PropertieOwner[5]] - 1));
		}
		if (gm.PropertieOwner[15] != -1)
		{
			gm.squarePrices[15][1] = 25 * pow((float)2, (numOfRailroadOwned[gm.PropertieOwner[15]] - 1));
		}
		if (gm.PropertieOwner[25] != -1)
		{
			gm.squarePrices[25][1] = 25 * pow((float)2, (numOfRailroadOwned[gm.PropertieOwner[25]] - 1));
		}
		if (gm.PropertieOwner[35] != -1)
		{
			gm.squarePrices[35][1] = 25 * pow((float)2, (numOfRailroadOwned[gm.PropertieOwner[35]] - 1));
		}


	}

void textColour(int fore,int back) { //changes text fore and background colour

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int mixed;

	fore *= 16; //multiplies foreground colour by 16

	mixed = fore + back; //adds them to get the code to call the colour combo

	SetConsoleTextAttribute(hConsole,mixed);	// sets colours in the console

}

void ownerColour(int squareNumer) //changes text colour based on property owner
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int colour[5] = { 112,159,160,192,224 }; //declares colours for unowned and each player

	SetConsoleTextAttribute(hConsole, colour[(gm.PropertieOwner[squareNumer]) + 1]); // sets colours in the console
}

int checkIfBankrupt(int allPlayers, int bankrupter) //checks if the player is bankrupt
{
	int i, numOfBankruptPlayers = 0, quit = 0;

	if (allPlayers == 1) //checks all players
	{
		for (i = 0; i < gm.playerAmount; i++) //loops through all players
		{
			if (gm.playerMoney[i] < 0) { quit = bankrupt(i, bankrupter); } //checks if they are in the red, and calls bankrupt function if they are
			if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
		}
	}
	else //checks only the active player
	{
		if (gm.playerMoney[gm.activePlayer] < 0) { quit = bankrupt(gm.activePlayer, bankrupter); } //checks if they are in the red, and calls bankrupt function if they are
		if (quit == 10 || quit == 11) { return quit; } //returns quit if the user wants to quit
	}

	for (i = 0; i < gm.playerAmount; i++) //adds each bankrupted player to the number of bankrupt players tally
	{
		if (gm.bankrupt[i] == 1) { numOfBankruptPlayers++; }
	}

	if (numOfBankruptPlayers == (gm.playerAmount - 1))
	{ 
		for (i = 0; i < gm.playerAmount; i++)
		{
			if (gm.bankrupt[i] == 0) { gm.winner = i; }
		}
	} //if all but one player is bankrupt, call the win function

}

int win() //saves highscore and ends the game when someone wins
{
	char selection = ' ';

	HWND console = GetConsoleWindow();
	RECT r;

	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 700, 850, TRUE);    // sets console size and pos

	saveHighscores(); //saves the users score if it is a high score
	system("cls");

	printf("\n\n /$$     /$$                                      /$$           /$$\n|  $$   /$$/                                     |__/          | $$\n \\  $$ /$$//$$$$$$  /$$   /$$       /$$  /$$  /$$ /$$ /$$$$$$$ | $$\n  \\  $$$$//$$__  $$| $$  | $$      | $$ | $$ | $$| $$| $$__  $$| $$\n   \\  $$/| $$  \\ $$| $$  | $$      | $$ | $$ | $$| $$| $$  \\ $$|__/ \n    | $$ | $$  | $$| $$  | $$      | $$ | $$ | $$| $$| $$  | $$    \n    | $$ |  $$$$$$/|  $$$$$$/      |  $$$$$/$$$$/| $$| $$  | $$ /$$\n    |__/  \\______/  \\______/        \\_____/\\___/ |__/|__/  |__/|__/\n");

	printf("\n\nCongratulations %s!\nYour score has been saved if it is a highscore.\n\n1: Return to welcome screen\n2: Quit to Windows", gm.playerNames[gm.winner]); //asks what the user wants to do
	
	do
	{
		selection = getch();

		fflush(stdin);

	} while (!(selection == '1' || selection == '2' ));

	switch (selection) //does selected action
	{
	case '1':
		return 11;
	case '2':
		return 10;
	}
}

int bankrupt(int player, int bankrupter) //gets player in the red to sell houses and mortgage properties, or declare bankruptcy if they cant recover
{
	char command[100], log[100], *token[3];
	int i, returnValue,currentPlayer, quit;

	currentPlayer = gm.activePlayer;
	gm.activePlayer = player;

	if (gm.activePlayer != currentPlayer)
	{
		strcpy(log, "*** IT IS NOW "); //lets players know that its now the bankrupt player's turn
		strcat(log, gm.playerNames[gm.activePlayer]);
		strcat(log, "'S TURN FOR THEY ARE IN THE RED ***");
		saveLog(log);
		saveLog("You need to sell buildings and mortgage properties");
		saveLog("Press ENTER to continue...");
		while (getchar() != '\n'); //waits for enter
	}
	else
	{
		strcpy(log, gm.playerNames[gm.activePlayer]); //tells player they are in the red if its already their turn
		strcat(log, ", you are in the red");
		saveLog(log);
		saveLog("You need to sell buildings and mortgage properties");
	}

	do {
		strcpy(log, gm.playerNames[player]);
		strcat(log, ", enter command (type help for help): ");
		saveLog(log);

		fflush(stdin);
		printf("> ");
		gets(command);
		strcpy(log, "> ");
		strcat(log, command);
		strupr(command); //converts command to upper case
		saveLog(log);


		token[0] = strtok(command, " "); //gets the first token (first word user enters)
		i = 1;
		do
		{
			token[i] = strtok(NULL, " "); //gets subsiqant tokens (words)
			i++;

		} while (!(token[i - 1] == NULL || i == 3)); //ends loop then there are three words (max arguments) or there are no more words

		if (token[0] == NULL) { token[0] = " "; } //sets null tokens to spaces to avoid crashes
		if (token[1] == NULL) { token[1] = " "; }
		if (token[2] == NULL) { token[2] = " "; }

		if (!((strcmp(token[0], "HELP") == 0) || (strcmp(token[0], "BANKRUPT") == 0) || (strcmp(token[0], "BUILDINGS") == 0) || (strcmp(token[0], "MORTGAGE") == 0) || (strcmp(token[0], "QUIT") == 0) || (strcmp(token[0], "UNMORTGAGE") == 0))) //error traps the users input
		{
			strcpy(log, "Error unknown command ");
			strcat(log, token[0]);
			saveLog(log);
		}
		if (strcmp(token[0], "HELP") == 0) //displays help menu
		{
			saveLog("HELP - shows help, END - end your turn");
			saveLog("BUILDINGS - add or remove houses/hotels, type 'BUILDINGS' for sytax help");
			saveLog("MORTGAGE/UNMORTGAGE - mortgage/unmortgage a property");
			saveLog("BANKRUPT - declare bankruptcy, must mortgage everything first");
			saveLog("QUIT - quit the game");
		}
		else if (strcmp(token[0], "BUILDINGS") == 0) //checks if they said buildings
		{
			returnValue = buySellHouses(token[1], token[2]); //calls building function and passes through arguments the player entered

			if (returnValue == 0) //if the function detects an error, print error message
			{
				strcpy(log, "Error, invalid argument ");
				strcat(log, token[1]);
				saveLog(log);
				saveLog("BUILDINGS [BUY/SELL/PRICE/VIEW] [PROPERTY NAME (eg: OA)]"); //displays syntax
				saveLog("When using view you do not need a property name");
				saveLog("Used to buy or sell houses/hotels on a property you own");
				saveLog("Buying a house on a property with four houses will build a hotel");
			}
		}
		else if (strcmp(token[0], "MORTGAGE") == 0) //checks if they said mortgage
		{
			morgage(token[1], 0); //mortgages property user defines
		}
		else if (strcmp(token[0], "UNMORTGAGE") == 0) //checks if they said unmortgage
		{
			morgage(token[1], 1); //unmortgages property user defines
		}
		else if (strcmp(token[0], "QUIT") == 0) //checks if they said quit
		{
			quit = exitScreen(1); //calls exitscreen and returns choice
			if (quit == 10 || quit == 11) { return quit; } //checks if they want to quit and returns the value
		}
		else if (strcmp(token[0], "BANKRUPT") == 0)
		{
			for (i = 0; i < 40; i++)	//checks if the player has mortgaged all there proprties
			{
				if (gm.PropertieOwner[i] == gm.activePlayer)
					if (gm.sqaureStatus[i] != 7) 
					{
						token[0] = " "; //if they didnt sets the command they entered to a space, not letting them go bankrupt
						saveLog("You must mortgage all you proprties before you can declare bankruptcy");
					}
			}
		}
	} while (!(strcmp(token[0], "BANKRUPT") == 0 || gm.playerMoney[player] >= 0)); //ends the users turn

	if (strcmp(token[0], "BANKRUPT") == 0) //checks if they declare bankruptcy
	{
		gm.bankrupt[player] = 1; //sets their status to bankrupt
		strcpy(log, gm.playerNames[player]); //prints that they are bankrupt
		strcat(log, ", you are bankrupt!");
		saveLog(log);
		
		strcpy(log, "All of your assets have gone to "); //displays message saying who your money goes to
		if (bankrupter == -1)
		{
			strcat(log, "the BANK");
			for (i = 0; i < 40; i++) //gives all properties to the bank
			{
				if (gm.PropertieOwner[i] == player)
				{
					gm.PropertieOwner[i] = -1;
					gm.sqaureStatus[i] = 0;
				}
			}
			if (gm.chanceGOOJF[player] == 1)
			{
				gm.chanceGOOJFUsed = 0;
				gm.chanceGOOJF[player] = 0;
			}
		}
		else
		{
			strcat(log, gm.playerNames[bankrupter]);
			for (i = 0; i < 40; i++) //gives all properties to the player who bankrupted you
			{
				if (gm.PropertieOwner[i] == player) { gm.PropertieOwner[i] = bankrupter; }
			}
			if (gm.chanceGOOJF[player] == 1)
			{
				gm.chanceGOOJF[player] = bankrupter;
			}
		}
		saveLog(log);

		gm.playerMoney[player] = 0; //sets bankrupted player's money to 0
		gm.playerLocation[player] = 44; //moves the player off the board

		saveLog("Press the ENTER key to continue...");
		while (getchar() != '\n');
	}
	else
	{
		strcpy(log, gm.playerNames[player]); //they are back in the black and play continues
		strcat(log, ", you did not go bankrupt. Play will continue as normal.");
		saveLog(log);

	}

	gm.activePlayer = currentPlayer; //sets it back to the current users turn
}

int morgage(char *propertyName,int unMortgage) //used when the player would like to mortgage/unmortgage a property
{
	char log[100];
	char temp[5]; //for converting ints to strings
	struct Bar properties;
	int propertyNumber;

	propertyNumber = getPropertNumber(propertyName);
	if (propertyNumber == -1) //if it equals negative one the street name was invalid
	{

		strcpy(log, gm.playerNames[gm.activePlayer]);
		strcat(log, ", unknown property ");
		strcat(log, propertyName); //tells the player it was invalid
		saveLog(log);

		saveLog("MORTGAGE/UNMORTGAGE [PROPERTY NAME (eg: OA)]"); //tells the player how to use the command

		return 1; //stops the function

	}
	if (unMortgage == 1) //true if they want to unmortgage
	{
		if (!(propertyNumber == 0 || propertyNumber == 2 || propertyNumber == 4 || propertyNumber == 7 || propertyNumber == 10 || propertyNumber == 17 || propertyNumber == 20 || propertyNumber == 22 || propertyNumber == 30 || propertyNumber == 33 || propertyNumber == 36 || propertyNumber == 38 || propertyNumber == 40 || propertyNumber == 41 || propertyNumber == 42 || propertyNumber == 43 || propertyNumber == 44))
		{
			if (gm.PropertieOwner[propertyNumber] == gm.activePlayer) //makes sure they own the property
			{
				if (gm.sqaureStatus[propertyNumber] == 7) //checks if its mortgaged
				{
					if (gm.playerMoney[gm.activePlayer] < ((gm.squarePrices[propertyNumber][0] / 2) + (gm.squarePrices[propertyNumber][0] / 10))) //checks that they player has enough money
					{
						saveLog("Not enough money"); //displays error msg
					}
					else
					{
						gm.playerMoney[gm.activePlayer] -= ((gm.squarePrices[propertyNumber][0] / 2) + (gm.squarePrices[propertyNumber][0] / 10)); //charges the player
						gm.sqaureStatus[propertyNumber] = 1; //7 means it is morgateged
						strcpy(log, gm.playerNames[gm.activePlayer]); //displays success msg
						strcat(log, ", you successfully unmortgaged ");
						strcat(log, gm.squareNames[propertyNumber]);
						strcat(log, " for $");
						strcat(log, itoa(((gm.squarePrices[propertyNumber][0] / 2) + (gm.squarePrices[propertyNumber][0] / 10)), temp, 10));
						saveLog(log);
					}
				}
				else
				{
					saveLog("This property is not mortgaged"); //displays error msg
				}
			}
			else
			{
				saveLog("You must own the property"); //displays error msg
			}
		}
		else
		{
			saveLog("You can not unmortgage this property");
		}

	}
	else
	//runs if they want to mortgage the property
	{
		if (!(propertyNumber == 0 || propertyNumber == 2 || propertyNumber == 4 || propertyNumber == 7 || propertyNumber == 10 || propertyNumber == 17 || propertyNumber == 20 || propertyNumber == 22 || propertyNumber == 30 || propertyNumber == 33 || propertyNumber == 36 || propertyNumber == 38 || propertyNumber == 40 || propertyNumber == 41 || propertyNumber == 42 || propertyNumber == 43 || propertyNumber == 44))
		{
			if (gm.PropertieOwner[propertyNumber] == gm.activePlayer) //checks if they are the property owner
			{
				if (propertyNumber == 12 || propertyNumber == 15 || propertyNumber == 25 || propertyNumber == 28 || propertyNumber == 35) //checks if the proprty is a RR or utill
				{
					gm.sqaureStatus[propertyNumber] = 7; //7 means it is morgateged
					gm.playerMoney[gm.activePlayer] += (gm.squarePrices[propertyNumber][0] / 2); //gives them the money
					strcpy(log, gm.playerNames[gm.activePlayer]); //displays success msg
					strcat(log, ", you successfully mortgaged ");
					strcat(log, gm.squareNames[propertyNumber]);
					strcat(log, " for $");
					strcat(log, itoa(gm.squarePrices[propertyNumber][0] / 2, temp, 10));
					saveLog(log);
				}
				else
				{
					if (gm.sqaureStatus[propertyNumber] == 7) //checks if its already mortgages
					{
						saveLog("It is already mortaged");
					}
					else
					{
						if (propertyNumber == 5 || propertyNumber == 15 || propertyNumber == 25 || propertyNumber == 35 || propertyNumber == 12 || propertyNumber == 28)
						{
							properties.x = propertyNumber;
							properties.y = 0;
							properties.z = 0;
						}
						else
						{
							properties = getPropertiesOnStreet(propertyNumber); //returns the value of the other properties on the strret
						}
						//checks that there are no houses on the street
						if ((gm.sqaureStatus[properties.x] == 1 || gm.sqaureStatus[properties.x] == 7 ||gm.sqaureStatus[properties.x] == 0) && (gm.sqaureStatus[properties.y] == 1 || gm.sqaureStatus[properties.y] == 7 || gm.sqaureStatus[properties.y] == 0 || properties.y == 0) && (gm.sqaureStatus[properties.z] == 1 || gm.sqaureStatus[properties.z] == 7 || gm.sqaureStatus[properties.z] == 0 || properties.z == 0))
						{
							gm.sqaureStatus[propertyNumber] = 7; //7 means it is morgateged
							gm.playerMoney[gm.activePlayer] += (gm.squarePrices[propertyNumber][0] / 2); //adds money
							strcpy(log, gm.playerNames[gm.activePlayer]); //displays success msg
							strcat(log, ", you successfully mortgaged ");
							strcat(log, gm.squareNames[propertyNumber]);
							strcat(log, " for $");
							strcat(log, itoa(gm.squarePrices[propertyNumber][0] / 2, temp, 10));
							saveLog(log);
						}
						else
						{
							saveLog("All the buildings on this street must be sold"); //displays error msg
						}
					}
				}

			}
			else
			{
				saveLog("You must own a property to mortgage it"); //displays error msg
			}
		}
		else
		{
			saveLog("You can not mortgage this property");
		}	
	}

}

struct Bar getPropertiesOnStreet(int property) //returns the properties on a street (in a colour group)
{
	struct Bar result; //declares result as structure bar to save properties on the street in
	int streets[3] = {0,0,0}, temp;
	int j = 0;
	FILE *file;

	file = fopen("Monopoly_Data/streets.csv", "r"); //opens file

	while (1) //loops until a return
	{
		fscanf(file, "%d,", &temp); //reads value from .csv file
		if (temp == -1 || temp == -2)
		{
			j = 0;	
			if (streets[0] == property || streets[1] == property || streets[2] == property) //checks if it found the right street
			{
				result.x = streets[0]; //saves properties to the structure
				result.y = streets[1];
				result.z = streets[2];
				fclose(file);
				return result; //returns structure
			}
			else
			{
				streets[2] = NULL; //sets streets[2] to null in case the next set has only two properties 
			}
			if (temp == -2) //-2 means end of file
			{
				result.x = 0;  //sets properties to 0 to avoid error
				result.y = 0;
				result.z = 0;
				saveLog("!!! An error occured when reading streets.csv"); //prints error msg
				saveLog("!!! This will casue the game to not work as expected");
				saveLog("!!! Try re-gening the files in the properties menu");
				return result;
			}
		}
		else
		{
			streets[j] = temp; //reads next street from file
			j++;
		}
	}


}

int exitScreen(int ingame) //displays the exit screen
{
	char choice;
	int savedCorrectly;

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 900, 900, TRUE);	// sets console size and pos

	system("cls"); //clear screen

	//displays exit ascii art
	printf("\n                                         `:+++++/:-.`                                               \n                                         /ddddh//+ooyhso/-`                                         \n                                         +ddddy      `:ddhhs/-`                                     \n                                         odddd/      .yddddddhho-                                   \n                                        `hmmmd`      odddmmmmh+ds                                   \n                                        -NNNMo      `hmmNNNNm:hy`                                   \n                                `.`     ymdmmysso+/:+mNNNMMN/sd.                                    \n                               `ods:` `:NNNNo```..:/shdmNNMy:N:                                     \n                                `+hmhs+mmNNm`      `hdmhhdddhd`                                     \n                                  `/hNmmdhdo-.``   `hdmmNNN+Ny`                                     \n                                   `sh-`omssymdyhyyyhddhdmNdNdo+:.                                  \n                                  `sh. `o.  `.o.``.so:..-s+/dNNNNNd-                                \n                                 `+m.   ` `::`     .  -:.`` -Nmhhs/`                                \n                                +hyh-     .yh.       `yh/    hh``                                   \n                               .m+```      ``     `.  ```   `hdh:                                   \n                               `yy`           `..`/s        `..yy`                                  \n                                .yh``     `-+yy/oyh+ss+-`    `/Nhsso+-`                             \n                            ./shdmN/ys:::+so:.`` `` `.:os/-.-smNNMMMMNy-                            \n                          `+dmNNNNNmoms//:.     ``      .:/+/yNNNNNMMMMm:`                          \n                          sdmNNNNNNNmyyy/-...-/ohds:.`` ``.:omNNNNNMMMMMm.                          \n                         `ddmmNNNNNNNNdooooooo//ss/+sssoyhdmNNNNNNNMMMMMMo`                         \n                         `ddmmNNNNNNNMNNmy+/-.`````.:+oymNNNNNNNNNNMMMMMMd`                         \n                         `ddmmmNNNNNNNNNMMNmyyhhhhhhyNNNNNNNMNNNNNNMMMMMMm`                         \n                          odmmmNNNNNNNNNMMMNy:/dNo-+dNNNNNNNMMNNNMNMMMMMMN-`                        \n                         `odmmmmNNNNNNNNNNMMNy/dm/oNNNNNNMMNMNNNNNNMMMMMMM+.                        \n                        /ydmmNmmNNNNNNNNNNNNNNNNNmNNNNNNNNNNMNNNNNNMNMMMMMNy-                       \n                        hhmmNNNNNNNNNNNNNNNNNNNNNNNMMNNNNNNNNNNNNNNMNMMMMMMMh`                      \n                       .hdmmNNNNNNNNNNNNNNNNNNNNNNNMNMNNNNNMMNNNMNNMMMMMMMMMN:`                     \n                      .ydmmmNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNMMNssNMNNNNMMMMMMMd:`                    \n                     :ydmmNNNNNNNNNhyNNNNMNNNNNNNNNNNNNNNNNNMh--dNNNNNNMMMMMMMm/`                   \n                   `/hddmNNNNNMNNNN+-mNNNNNNNNNNNdNNNNNNNNNMMm+.:mNNNNNNMMMMMMMNo.                  \n                  -sdmmmNNNNNNNNNNd--dNNNNNMNNNms-+hmNNNNNNNNNNo-oNNNNNNMMMNMMMNN+`      ./         \n         `       `/sdmmNNNNNNNNNNN/-sNNNNNNMMNh:`  -ohmmNNNNNNNNs-sNNNNNMMMMNmy/odo.```:odN.        \n        :h+.    .+s..+hmNNNNMNNNMs.yNNNNMMNmy:`      `:sdmmNNNNNNs-sNNMMNNdy/.``:hdysso++dy`        \n        ss+so+++hdo.  `:ydNNNNNNd-+NNNNNNd+.            `-:shmNNNNs.odNyho.``.+yy/``.-:shs`         \n        .y+--::::./s+.  `-ohmNNd--mNNNms-`                  `-/sdNN+..mohy:+yyo-``-+ymy+.           \n         `-oso+:-.../so:`/y:/my-`sMNh+.                         `+NNy-odyyNd:`  .+o/-hs             \n            oh/++oo-  -ohy+:do``oNN+`                            `mNhds/..yN+`    :+ :m`            \n           :d-`:`     `-mooyo`-yNNd`                              sNy.sh: .hN/`   -d``m:            \n          .d/`/d.     :mo` `.odsmMo               -/              :Nm.`/m: .hm/`   o+ ys            \n          sh`/d-    `+d+`  -hy-.NM:              `yd`             .NN/  :d: `sNo.  `s.+d            \n         .m:.d:   `:yh:  `+d/` -MN-              -NN-             `mNs   +d` `+mh:` .--m`           \n         +h`y/  `.sd+`   /m-   :MN.              syds              hNd-``:m`   .sdo.  `o`           \n         +:`-`./sy/.     oh-.-/hMN`             `d+om`             yMNyyyho       ``                \n             .o+-`       `:oo+:+NN`             .m:.N/             yMN.```                          \n                               :MN.             /N. dy             yNN`                             \n                               .NN:             oN. yN.            dMd`                             \n		");
	
	if (ingame == 1) //if they are in game, and cannot quit and save
	{

		printf("\n\n\n                                       1: BACK TO GAME\n\n                                       2: EXIT TO WELCOME SCREEN\n\n                                       3: EXIT TO WINDOWS");

		do {
			choice = _getch();
		} while (!(choice == '1' || choice == '2' || choice == '3'));
		switch (choice) {
		case '1':
			GetWindowRect(console, &r); // stores the console's current dimensions
			MoveWindow(console, r.left, r.top, 565, 850, TRUE);	// sets console size and pos
			return 1;
		case '3':
			return 10;
		case '2':
			return 11;
		}
	}
	else if (ingame == 2) //if they are ingame and CAN quit and save
	{

		printf("\n\n\n                                       1: BACK TO GAME\n\n                                       2: EXIT TO WELCOME SCREEN\n\n                                       3: EXIT TO WINDOWS\n\n                                       4: SAVE AND EXIT TO WELCOME SCREEN\n\n                                       5: SAVE AND EXIT TO WINDOWS");

		do {
			choice = _getch();
		} while (!(choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == '5'));
		switch (choice) {
		case '1':
			GetWindowRect(console, &r); // stores the console's current dimensions
			MoveWindow(console, r.left, r.top, 565, 850, TRUE);	// sets console size and pos
			return 1;
		case '3':
			return 10;
		case '2':
			return 11;
		case '4':
			savedCorrectly = saveGame();
			if (savedCorrectly == 1)
			{
				GetWindowRect(console, &r); // stores the console's current dimensions
				MoveWindow(console, r.left, r.top, 565, 850, TRUE);	// sets console size and pos
				return 11;
			}
			return 1;
		case '5':
			savedCorrectly = saveGame();
			if (savedCorrectly == 1)
			{
				return 10;
			}
			return 1;
		}
	}
	else //they arent in game and cant save
	{

		printf("\n\n\n                                       1: BACK TO WELCOME SCREEN\n\n                                       2: EXIT TO WINDOWS");

		do {
			choice = _getch();
		} while (!(choice == '1' || choice == '2'));
		switch (choice) {
		case '2':
			return 10;
		case '1':
			return 11; 
		}

	}

}

void saveHighscores() { //sorts and saves the new highscore to the file

	char names[11][3], tempname[3];
	int score[11], money[11], sort = 0, i, tempscore, tempmoney;
	FILE *highscores;

	highscores = fopen("Monopoly_Data/highscores.sav", "r"); //open highscores file

	if (highscores == NULL)
	{
		printf("Error reading highscores\nPress ENTER to continue...");
		while (getchar() != '\n');
		return;
	}

	for (i = 0; i < 10; i++)
	{
		fscanf(highscores, "%[^~]~%d~%d~", names[i], &money[i], &score[i]); //scans for all saved highscores
	}

	fclose(highscores); //close file

	strcpy(names[10], gm.playerNames[gm.winner]); //copies the winner's info into a temp array
	money[10] = gm.playerMoney[gm.winner];
	if (gm.timesPassedGo[gm.winner] <= 0) { gm.timesPassedGo[gm.winner] = 1; }
	score[10] = ((gm.playerMoney[gm.winner] / gm.timesPassedGo[gm.winner]) * 1000); //calculates their score

	while (sort == 0) //sorts the info in the temp array
	{
		sort = 1;

		for (i = 0; i < 10; i++)
		{
			if (score[i] < score[i + 1])
			{
				strcpy(tempname, names[i]);
				strcpy(names[i],names[i+1]);
				strcpy(names[i+1],tempname);

				tempscore = score[i];
				score[i] = score[i+1];
				score[i+1] = tempscore;
				
				tempmoney = money[i];
				money[i] = money[i+1];
				money[i+1] = tempmoney;
				
				sort = 0;

			}

		}
	}

	highscores = fopen("Monopoly_Data/highscores.sav", "w"); //opens the highscore file

	for (i = 0; i < 10; i++) //saves back the top 10 highest scores
	{
		fprintf(highscores, "%s~%d~%d~", names[i], money[i], score[i]);
	}

	fclose(highscores);

}

int saveGame() { //encrypts and saves the game to a file to be loaded later

	int i, j, amountOfPlayers = gm.playerAmount,lengthOfLog = gm.logLength;
	unsigned int seed;
	char saveName[200], temp[100], *tildes;
	FILE *gameFile;

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 1100, 900, TRUE);    // sets console size and pos
	

	system("cls");

	do
	{
		//prints ascii art
		printf("  ______       _                                            __                                                              \n |  ____|     | |                                          / _|                                                             \n | |__   _ __ | |_ ___ _ __   _ __   __ _ _ __ ___   ___  | |_ ___  _ __   ___  __ ___   _____    __ _  __ _ _ __ ___   ___ \n |  __| | '_ \\| __/ _ \\ '__| | '_ \\ / _` | '_ ` _ \\ / _ \\ |  _/ _ \\| '__| / __|/ _` \\ \\ / / _ \\  / _` |/ _` | '_ ` _ \\ / _ \\\n | |____| | | | ||  __/ |    | | | | (_| | | | | | |  __/ | || (_) | |    \\__ \\ (_| |\\ ' /  __/ | (_| | (_| | | | | | |  __/\n |______|_| |_|\\__\\___|_|    |_| |_|\\__,_|_| |_| |_|\\___| |_| \\___/|_|    |___/\\__,_| \\_/ \\___|  \\__, |\\__,_|_| |_| |_|\\___|\n                                                                                                  __/ |                     \n                                                                                                 |___/                      \n");
		printf("\t\t\t\t\t\t\t _.........._ \n\t\t\t\t\t\t\t| |Monopoly| |\n\t\t\t\t\t\t\t| |        | |\n\t\t\t\t\t\t\t| |        | |\n\t\t\t\t\t\t\t| |________| |\n\t\t\t\t\t\t\t|   ______   |\n\t\t\t\t\t\t\t|  |    | |  |\n\t\t\t\t\t\t\t|__|____|_|__|\n\n");

		gets(temp); //gets save name
		if (strcmp(temp, "") == 0) { printf("\nError, that's not a valid filename!\n\n"); }
	} while (strcmp(temp, "") == 0);

	_mkdir("Monopoly_Data"); //makes directories for files incase theyve been deleted
	_mkdir("Monopoly_Data/saves");

	strcpy(saveName, "Monopoly_Data/saves/"); //creates save name
	strcat(saveName, temp);
	strcat(saveName, ".sav");

	gameFile = fopen(saveName, "w"); //opens file

	if (gameFile == NULL) //error traps
	{
		system("cls");
		printf("!!! The game could not be saved, press ENTER to continue...");

		while (getchar() != '\n');

		return 0;
	}

	seed = rand(); //generates seed for random number encryption

	if (gm.rollagain == 0) //ends players turn if they did not roll doubles
	{
		gm.activePlayer++;
		if (gm.activePlayer == gm.playerAmount) { gm.activePlayer = 0; }
	}

	fprintf(gameFile, "%d~", seed); //prints seed to file

	//initializes random num generator
	srand(seed);

	//encrypts
	gm.playerAmount += rand();
	gm.activePlayer += rand();
	gm.housesLeft += rand();
	gm.hotelsLeft += rand();
	gm.communityChestDeckLocation += rand();
	gm.chanceDeckLocation += rand();
	gm.communityGOOJFUsed += rand();
	gm.chanceGOOJFUsed  += rand();
	gm.logLength += rand();
	gm.numOfDoubles += rand();
	gm.rollagain += rand();
	
	//prints to file
	fprintf(gameFile, "%d~%d~%d~%d~%d~%d~%d~%d~%d~%d~%d~", gm.playerAmount,gm.activePlayer,gm.housesLeft,gm.hotelsLeft,gm.communityChestDeckLocation,gm.chanceDeckLocation,gm.communityGOOJFUsed,gm.chanceGOOJFUsed, gm.logLength, gm.numOfDoubles, gm.rollagain);

	for (i = 0; i < amountOfPlayers; i++) //encrypts and prints to file
	{
		gm.playerLocation[i] += rand();
		gm.playerMoney[i] += rand();
		gm.jailed[i] += rand();
		gm.communityGOOJF[i] += rand();
		gm.chanceGOOJF[i] += rand();
		gm.logPosition[i] += rand();
		gm.timesPassedGo[i] += rand();
		gm.turnsInJail[i] += rand();
		gm.bankrupt[i] += rand();

		fprintf(gameFile, "%d~%d~%d~%d~%d~%s~%d~%d~%d~%d~", gm.playerLocation[i], gm.playerMoney[i], gm.jailed[i], gm.communityGOOJF[i], gm.chanceGOOJF[i], gm.playerNames[i], gm.logPosition[i], gm.timesPassedGo[i], gm.turnsInJail[i], gm.bankrupt[i]);
		
		for (j = 0; j < lengthOfLog; j++) //saves log to file
		{
			while (strstr(gm.consoleLog[i][j], "~") != NULL)
			{
				tildes = strstr(gm.consoleLog[i][j], "~");
				strncpy(tildes,"-", 1);
			}

			if (strcmp(gm.consoleLog[i][j], "") == 0)
			{
				fprintf(gameFile, " ~");
			}
			else
			{
				fprintf(gameFile, "%s~", gm.consoleLog[i][j]);
			}
			
		}
	}

	for (i = 0; i < 40; i++) //encrypts and prints to file
	{
		gm.PropertieOwner[i] += rand();
		gm.sqaureStatus[i] += rand();
		fprintf(gameFile, "%d~%d~", gm.PropertieOwner[i], gm.sqaureStatus[i]);
	}

	for (i = 0; i < 16; i++) //encrypts and prints to file
	{
		gm.communityChestCards[i] += rand();
		fprintf(gameFile, "%d~", gm.communityChestCards[i]);
	}

	for (i = 0; i < 17; i++) //encrypts and prints to file
	{
		gm.chanceCards[i] += rand();
		fprintf(gameFile, "%d~", gm.chanceCards[i]);
	}

	fclose(gameFile); //closes file

	srand(seed); //reinitializes random num generator

	//decrypts everything to return to game

	gm.playerAmount -= rand();
	gm.activePlayer -= rand();
	gm.housesLeft -= rand();
	gm.hotelsLeft -= rand();
	gm.communityChestDeckLocation -= rand();
	gm.chanceDeckLocation -= rand();
	gm.communityGOOJFUsed -= rand();
	gm.chanceGOOJFUsed -= rand();
	gm.logLength -= rand();
	gm.numOfDoubles -= rand();
	gm.rollagain -= rand();

	for (i = 0; i < gm.playerAmount; i++)
	{
		gm.playerLocation[i] -= rand();
		gm.playerMoney[i] -= rand();
		gm.jailed[i] -= rand();
		gm.communityGOOJF[i] -= rand();
		gm.chanceGOOJF[i] -= rand();
		gm.logPosition[i] -= rand();
		gm.timesPassedGo[i] -= rand();
		gm.turnsInJail[i] -= rand();
		gm.bankrupt[i] -= rand();

	}

	for (i = 0; i < 40; i++)
	{
		gm.PropertieOwner[i] -= rand();
		gm.sqaureStatus[i] -= rand();
	}

	for (i = 0; i < 16; i++)
	{
		gm.communityChestCards[i] -= rand();
	}

	for (i = 0; i < 17; i++)
	{
		gm.chanceCards[i] -= rand();
	}

	return 1;

}

int loadGame() { //loads and unencrypts the save file defined by the player

	HWND console = GetConsoleWindow();
	RECT r;

	unsigned int seed;
	int i, j;
	char loadName[200], choice, temp[100];
	FILE *gameFile;

	//ascii art
	printf("          _____                    _____            _____                _____          \n         /\\    \\                  /\\    \\          /\\    \\              |\\    \\         \n        /::\\    \\                /::\\____\\        /::\\    \\             |:\\____\\        \n       /::::\\    \\              /:::/    /       /::::\\    \\            |::|   |        \n      /::::::\\    \\            /:::/    /       /::::::\\    \\           |::|   |        \n     /:::/\\:::\\    \\          /:::/    /       /:::/\\:::\\    \\          |::|   |        \n    /:::/__\\:::\\    \\        /:::/    /       /:::/__\\:::\\    \\         |::|   |        \n   /::::\\   \\:::\\    \\      /:::/    /       /::::\\   \\:::\\    \\        |::|   |        \n  /::::::\\   \\:::\\    \\    /:::/    /       /::::::\\   \\:::\\    \\       |::|___|______  \n /:::/\\:::\\   \\:::\\____\\  /:::/    /       /:::/\\:::\\   \\:::\\    \\      /::::::::\\    \\ \n/:::/  \\:::\\   \\:::|    |/:::/____/       /:::/  \\:::\\   \\:::\\____\\    /::::::::::\\____\\\n\\::/    \\:::\\  /:::|____|\\:::\\    \\       \\::/    \\:::\\  /:::/    /   /:::/~~~~/~~      \n \\/_____/\\:::\\/:::/    /  \\:::\\    \\       \\/____/ \\:::\\/:::/    /   /:::/    /         \n          \\::::::/    /    \\:::\\    \\               \\::::::/    /   /:::/    /          \n           \\::::/    /      \\:::\\    \\               \\::::/    /   /:::/    /           \n            \\::/____/        \\:::\\    \\              /:::/    /    \\::/    /            \n             ~~               \\:::\\    \\            /:::/    /      \\/____/             \n                               \\:::\\    \\          /:::/    /                           \n                                \\:::\\____\\        /:::/    /                            \n                                 \\::/    /        \\::/    /                             \n                                  \\/____/          \\/____/                              \n\n\n");

	printf("1: Start new game\n2: Load existing save"); //asks if they want to load a game

	do {

		fflush(stdin);

		choice = getch();

	} while (!(choice == '1' || choice == '2')); //waits for selection

	switch (choice)
	{
	default:
	case '1':
		system("cls");
		return 0; //starts game if they dont want to load one
	case '2':
		break;
	}

	system("cls");
	printf("  ______       _                                                  __                                _          _                 _ \n |  ____|     | |                                                / _|                              | |        | |               | |\n | |__   _ __ | |_ ___ _ __   _ __   __ _ _ __ ___   ___    ___ | |_    __ _  __ _ _ __ ___   ___  | |_ ___   | | ___   __ _  __| |\n |  __| | '_ \\| __/ _ \\ '__| | '_ \\ / _` | '_ ` _ \\ / _ \\  / _ \\|  _|  / _` |/ _` | '_ ` _ \\ / _ \\ | __/ _ \\  | |/ _ \\ / _` |/ _` |\n | |____| | | | ||  __/ |    | | | | (_| | | | | | |  __/ | (_) | |   | (_| | (_| | | | | | |  __/ | || (_) | | | (_) | (_| | (_| |\n |______|_| |_|\\__\\___|_|    |_| |_|\\__,_|_| |_| |_|\\___|  \\___/|_|    \\__, |\\__,_|_| |_| |_|\\___|  \\__\\___/  |_|\\___/ \\__,_|\\__,_|\n                                                                        __/ |                                                      \n                                                                       |___/                                                       \n");
	printf("\t\t\t\t\t\t\t _.........._ \n\t\t\t\t\t\t\t| |Monopoly| |\n\t\t\t\t\t\t\t| |        | |\n\t\t\t\t\t\t\t| |        | |\n\t\t\t\t\t\t\t| |________| |\n\t\t\t\t\t\t\t|   ______   |\n\t\t\t\t\t\t\t|  |    | |  |\n\t\t\t\t\t\t\t|__|____|_|__|\n\n");
	gets(temp); //gets name of game to load

	strcpy(loadName, "Monopoly_Data/saves/"); //creates file path
	strcat(loadName, temp);
	strcat(loadName, ".sav");

	gameFile = fopen(loadName, "r"); //opens file for writing
	
	if (gameFile == NULL) //error traps
	{
		printf("\nCould not find that save file.\nMake sure you spell the name correctly, and that the file is in the \"saves\" directory.\n\n"); //displays error
		printf("Press ENTER to continue...");
		while (getchar() != '\n');
		system("cls");
		loadGame(); //reruns function
		return;
	}

	startGame(); //initilizes varables

	fscanf(gameFile, "%d~", &seed); //finds the seed to use to decrypt game

	srand(seed); //initializes random num gen

	//reads variables from file
	fscanf(gameFile, "%d~%d~%d~%d~%d~%d~%d~%d~%d~%d~%d~", &gm.playerAmount, &gm.activePlayer, &gm.housesLeft, &gm.hotelsLeft, &gm.communityChestDeckLocation, &gm.chanceDeckLocation, &gm.communityGOOJFUsed, &gm.chanceGOOJFUsed, &gm.logLength, &gm.numOfDoubles, &gm.rollagain);

	//decrypts variables
	gm.playerAmount -= rand();
	gm.activePlayer -= rand();
	gm.housesLeft -= rand();
	gm.hotelsLeft -= rand();
	gm.communityChestDeckLocation -= rand();
	gm.chanceDeckLocation -= rand();
	gm.communityGOOJFUsed -= rand();
	gm.chanceGOOJFUsed  -= rand();
	gm.logLength -= rand();
	gm.numOfDoubles -= rand();
	gm.rollagain -= rand();

	for (i = 0; i < gm.playerAmount; i++) //reads variables from file and decrypts them
	{
		fscanf(gameFile, "%d~%d~%d~%d~%d~%[^~]~%d~%d~%d~%d~", &gm.playerLocation[i], &gm.playerMoney[i], &gm.jailed[i], &gm.communityGOOJF[i], &gm.chanceGOOJF[i], gm.playerNames[i], &gm.logPosition[i], &gm.timesPassedGo[i], &gm.turnsInJail[i], &gm.bankrupt[i]);

		gm.playerLocation[i] -= rand();
		gm.playerMoney[i] -= rand();
		gm.jailed[i] -= rand();
		gm.communityGOOJF[i] -= rand();
		gm.chanceGOOJF[i] -= rand();
		gm.logPosition[i] -= rand();
		gm.timesPassedGo[i] -= rand();
		gm.turnsInJail[i] -= rand();
		gm.bankrupt[i] -= rand();

		for (j = 0; j < gm.logLength; j++) //reads console log (not encrypted)
		{
			fscanf(gameFile, "%[^~]~", gm.consoleLog[i][j]);
		}
	}

	for (i = 0; i < 40; i++) //reads variables from file and decrypts them
	{
		fscanf(gameFile, "%d~%d~", &gm.PropertieOwner[i], &gm.sqaureStatus[i]);
		gm.PropertieOwner[i] -= rand();
		gm.sqaureStatus[i] -= rand();
	}

	for (i = 0; i < 17; i++) //reads variables from file and decrypts them
	{
		fscanf(gameFile, "%d~", &gm.communityChestCards[i]);
		gm.communityChestCards[i] -= rand();
	}

	for (i = 0; i < 16; i++) //reads variables from file and decrypts them
	{
		fscanf(gameFile, "%d~", &gm.chanceCards[i]);
		gm.chanceCards[i] -= rand();
	}

	fclose(gameFile); //closes file

	calculateRail(); //calls calc rail

	system("cls"); //clears screen

	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 600, 900, TRUE);	// sets console size and pos

	SetConsoleTitle(_T("Monopoly - Mark and Ben")); //sets the console title

	textColour(7, 0); //resets colour

	return 1; //returns a success

}

void drawBoard() { //draws the board in it's current state of play when called
	

	int i, numOfGetOutOfJailFree[4] = {0,0,0,0};
	char drawPlayersOnBoard[44][3];
	char drawPrices[40][5];
	char tempString[5];
	int playersOnSqaures[44],temp;
	HWND console = GetConsoleWindow();
	RECT r;

	GetWindowRect(console, &r); // stores the console's current dimensions

	MoveWindow(console, r.left, r.top, 565, 850, TRUE);    // sets console size and pos


	for (i = 0; i < gm.playerAmount; i++) //calcs how many get out of jail free cards each player has
	{
		if (gm.chanceGOOJF[i] == 1) {numOfGetOutOfJailFree[i]++;}
		if (gm.communityGOOJF[i] == 1) {numOfGetOutOfJailFree[i]++;}
	}

	for (i = 0; i < 44; i++) {

		strcpy(drawPlayersOnBoard[i], "  "); //removes player icons from board
		playersOnSqaures[i] = 0;

	}

	for (i = 0; i < 40; i++) { //calc what price to show on each sqaure

		if (i == 12) //special case for first utill
		{
			if (gm.sqaureStatus[12] == 7)
			{
				strcpy(drawPrices[i], "Mrtg");
			}
			else
			{
				if (gm.PropertieOwner[12] == -1) //if unowned shows price to buy
				{
					itoa(gm.squarePrices[i][gm.sqaureStatus[i]], tempString, 10);
					strcpy(drawPrices[i], "$");
					strcat(drawPrices[i], tempString);
				}
				else if (gm.PropertieOwner[12] == gm.PropertieOwner[28]) //checks if owner own both (ih they own both it shows X10 else it shown x4)
				{
					strcpy(drawPrices[i], "x10");
				}
				else
				{
					strcpy(drawPrices[i], "x4 ");
				}
			}
		}
		else if(i == 28) //special case for second utill
		{	
			if (gm.sqaureStatus[28] == 7)
			{
				strcpy(drawPrices[i], "Mrtg");
			}
			else
			{
				if (gm.PropertieOwner[28] == -1) //if unowned shows price to buy
				{
					itoa(gm.squarePrices[i][gm.sqaureStatus[i]], tempString, 10);
					strcpy(drawPrices[i], "$");
					strcat(drawPrices[i], tempString);
				}
				else if (gm.PropertieOwner[12] == gm.PropertieOwner[28]) //checks if owner own both (ih they own both it shows X10 else it shown x4)
				{
					strcpy(drawPrices[i], "x10");
				}
				else
				{
					strcpy(drawPrices[i], "x4 ");
				}
			}
		}
		else
		{
			if (gm.sqaureStatus[i] == 7) //checks if the square it mortgaged, shows mrtg if it is
			{
				strcpy(drawPrices[i], "Mrtg");
			}
			else
			{
				if (gm.showBuildingAmount == 1) //checks if plyaer want to see the amount of buildings
				{
					if (gm.sqaureStatus[i] < 2) //if there are no buildings it shows nothing
					{
						strcpy(drawPrices[i], "");
					}
					else if (gm.sqaureStatus[i] == 6) //shows hotl if there is a hotel
					{
						strcpy(drawPrices[i], "HOTL");
					}
					else
					{
						strcpy(drawPrices[i], "Hx"); //shows Hx(number of houses) if there are houses on the street
						itoa(gm.sqaureStatus[i] - 1, tempString, 10);
						strcat(drawPrices[i], tempString);
					}
				}
				else if (gm.squarePrices[i][gm.sqaureStatus[i]] < 1000) //if the price to land on the sqaure is < 999 it displays the price normaly
				{
					itoa(gm.squarePrices[i][gm.sqaureStatus[i]], tempString, 10);
					strcpy(drawPrices[i], "$");
					strcat(drawPrices[i], tempString);
				}
				else // if the price to land on the sqaure is > 999 it displays the price in hundreds
				{
					itoa((gm.squarePrices[i][gm.sqaureStatus[i]] / 100), tempString, 10);
					strcpy(drawPrices[i], "$");
					strcat(drawPrices[i], tempString);
					strcat(drawPrices[i], "C");
				}
			}
		}
	}

	for (i = 0; i < gm.playerAmount; i++) //figures out where to display the players on the board
	{
		if (gm.bankrupt[i] == 0) //checks is the player is still in the game
		{
			if (playersOnSqaures[gm.playerLocation[i]] == 0) //if they are the only player it shows the player name
			{
				strcpy(drawPlayersOnBoard[gm.playerLocation[i]], gm.playerNames[i]); //adds player icons to board
				playersOnSqaures[gm.playerLocation[i]] = 1;
			}
			else if (playersOnSqaures[gm.playerLocation[i]] == 1) //if there are two players there it shows X2
			{
				strcpy(drawPlayersOnBoard[gm.playerLocation[i]], "x2");
				playersOnSqaures[gm.playerLocation[i]] = 2;
			}
			else if (playersOnSqaures[gm.playerLocation[i]] == 2) //if there are two players there it shows X3
			{
				strcpy(drawPlayersOnBoard[gm.playerLocation[i]], "x3");
				playersOnSqaures[gm.playerLocation[i]] = 3;
			}
			else if (playersOnSqaures[gm.playerLocation[i]] == 3) //if there are two players there it shows X4
			{
				strcpy(drawPlayersOnBoard[gm.playerLocation[i]], "x4");
				playersOnSqaures[gm.playerLocation[i]] = 4;
			}
		}

	}

	system("cls");

	// line

	printf(" %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196);

	// first row
	textColour(15, 0);
	printf("|FREE|");						// free parking
	textColour(12, 0);
	printf("|-KA-|");						// kentucky ave
	textColour(15, 0);
	printf("|-C?-|");						// chance
	textColour(12, 0);
	printf("|-IN-|");						// indiana ave
	printf("|-IL-|");						// illinois ave
	textColour(8, 0);
	printf("|-RR-|");						// B&O railroad
	textColour(14, 0);
	printf("|-AA-|");						// atlantic ave
	printf("|-VA-|");						// ventnor ave
	textColour(1, 15);
	printf("|-WW-|");						// water works
	textColour(14, 0);
	printf("|-MG-|");						// marvin gardens
	textColour(9, 15);
	printf("|-TO-|\n");						// go to jail

	textColour(15, 0);
	printf("|PARK|");						// free parking
	ownerColour(21);
	printf("|%4s|", drawPrices[21]); 						// kentucky ave
	textColour(7, 0);
	printf("|    |"); 						// chance
	ownerColour(23);
	printf("|%4s|", drawPrices[23]); 						// indiana ave
	ownerColour(24);
	printf("|%4s|", drawPrices[24]); 						// illinois ave
	ownerColour(25);
	printf("|%4s|", drawPrices[25]); 						// B&O railroad
	ownerColour(26);
	printf("|%4s|", drawPrices[26]); 						// atlantic ave
	ownerColour(27);
	printf("|%4s|", drawPrices[27]); 						// ventnor ave
	ownerColour(28);
	printf("|%4s|", drawPrices[28]); 						// water works
	ownerColour(29);
	printf("|%4s|", drawPrices[29]); 						// marvin gardens
	textColour(9, 15);
	printf("|JAIL|\n");						// go to jail
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[20]);						// free parking
	printf("| %s |", drawPlayersOnBoard[21]); 						// kentucky ave
	printf("| %s |", drawPlayersOnBoard[22]); 						// chance
	printf("| %s |", drawPlayersOnBoard[23]); 						// indiana ave
	printf("| %s |", drawPlayersOnBoard[24]); 						// illinois ave
	printf("| %s |", drawPlayersOnBoard[25]); 						// B&O railroad
	printf("| %s |", drawPlayersOnBoard[26]); 						// atlantic ave
	printf("| %s |", drawPlayersOnBoard[27]); 						// ventnor ave
	printf("| %s |", drawPlayersOnBoard[28]); 						// water works
	printf("| %s |", drawPlayersOnBoard[29]); 						// marvin gardens
	printf("| %s |\n", drawPlayersOnBoard[30]);						// go to jail;

	// second row
	textColour(5, 15);
	printf("|-NY-|");						// new york
	textColour(7, 0);
	printf("------------------------------------------------------");
	textColour(2, 0);
	printf("|-PC-|\n");						// pacific ave

	ownerColour(19);
	printf("|%4s|", drawPrices[19]);						// new york
	textColour(7, 0);
	printf("\t\t\t\t\t\t\t    ");
	ownerColour(31);
	printf("|%4s|\n", drawPrices[31]);						// pacific ave
	
	textColour(7, 0);
	printf("| %s |", drawPlayersOnBoard[19]);						// new york
	printf("      .-----.");
	printf("\t\t\t\t\t    ");
	printf("| %s |\n", drawPlayersOnBoard[31]);						// pacific ave


	// third row
	textColour(5, 15);
	printf("|-TA-|");						// tennessee ave
	textColour(7, 0);
	printf("      |CHEST|");
	printf("\t\t\t\t\t    ");
	textColour(2, 0);
	printf("|-NC-|\n");						// north carolina avenue

	ownerColour(18);
	printf("|%4s|", drawPrices[18]);						// tennessee ave
	textColour(7, 0);
	printf("      \'-----\'");
	printf("\t\t\t\t\t    ");
	ownerColour(32);
	printf("|%4s|\n", drawPrices[32]);						// north carolina avenue
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[18]);						// tennessee ave
	printf("\t\t\t\t\t\t\t    ");
	printf("| %s |\n", drawPlayersOnBoard[32]);						// north carolina avenue


	// fourth row
	textColour(15, 0);
	printf("|-CC-|");						// left community chest
	textColour(7, 0);
	printf("\t\t\t\t\t\t\t    ");
	textColour(15, 0);
	printf("|-CC-|\n");						// right community chest

	textColour(7, 0);
	printf("|    |");						// left community chest
	printf("        ");
	textColour(7, 12);
	printf("  __  __   ____   _   _   ____        ");
	textColour(7, 0);
	printf("\t    ");
	printf("|    |\n");						// right community chest

	printf("| %s |", drawPlayersOnBoard[17]);						// left community chest
	printf("\t      ");
	textColour(7, 12);
	printf(" |  \\/  | / __ \\ | \\ | | / __ \\       ");
	textColour(7, 0);
	printf("\t    ");
	textColour(7, 0);
	printf("| %s |\n", drawPlayersOnBoard[33]);						// right community chest


	// fifth row
	textColour(5, 15);
	printf("|-SJ-|");						// st james place
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf(" | \\  / || |  | ||  \\| || |  | |      ");
	textColour(7,0);
	printf("\t    ");
	textColour(2, 0);
	printf("|-PA-|\n");						// pennsylvania avenue

	ownerColour(16);
	printf("|%4s|", drawPrices[16]);						// st james place
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf(" | |\\/| || |  | || . ` || |  | |      ");
	textColour(7,0);
	printf("\t    ");
	ownerColour(34);
	printf("|%4s|\n", drawPrices[34]);						// pennsylvania avenue
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[16]);						// st james place
	textColour(7,0);
	printf("\t      ");
	textColour(7, 12);
	printf(" | |  | || |__| || |\\  || |__| |      ");
	textColour(7, 0);
	printf("\t    ");
	printf("| %s |\n", drawPlayersOnBoard[34]);						// pennsylvania avenue


	// sixth row
	textColour(8, 0);
	printf("|-RR-|");						// pennsylvania rr
	textColour(7,0);
	printf("\t      ");
	textColour(7, 12);
	printf(" |_|  |_| \\____/ |_| \\_| \\____/       ");
	textColour(7, 0);
	printf("\t    ");
	textColour(8, 0);
	printf("|-RR-|\n");						// short rr

	ownerColour(15);
	printf("|%4s|", drawPrices[15]);						// pennsylvania rr
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf("        _____    ____   _   __     __ ");
	textColour(7 ,0);
	printf("\t    ");
	ownerColour(35);
	printf("|%4s|\n", drawPrices[35]);						// short rr
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[15]);						// pennsylvania rr
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf("       |  __ \\  / __ \\ | |  \\ \\   / / ");
	textColour(7, 0);
	printf("\t    ");
	printf("| %s |\n", drawPlayersOnBoard[35]);						// short rr


	// seventh row
	textColour(13, 0);
	printf("|-VI-|");						// virginia ave
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf("       | |__) || |  | || |   \\ \\_/ /  ");
	textColour(7, 0);
	printf("\t    ");
	textColour(15, 0);
	printf("|-C?-|\n");						// chance

	ownerColour(14);
	printf("|%4s|", drawPrices[14]);						// virginia ave
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf("       |  ___/ | |  | || |    \\   /   ");
	textColour(7, 0);
	printf("\t    ");
	printf("|    |\n");						// chance
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[14]);						// virginia ave
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf("       | |     | |__| || |____ | |    ");
	textColour(7, 0);
	printf("\t    ");
	printf("| %s |\n", drawPlayersOnBoard[36]);						// chance


	// eighth row
	textColour(13, 0);
	printf("|-SA-|");						// states avenue
	textColour(7, 0);
	printf("\t      ");
	textColour(7, 12);
	printf("       |_|      \\____/ |______||_|    ");
	textColour(7, 0);
	printf("\t    ");
	textColour(9, 15);
	printf("|-PP-|\n");						// park place

	ownerColour(13);
	printf("|%4s|", drawPrices[13]);						// states avenue
	textColour(7, 0);
	printf("\t\t\t\t\t\t\t    ");
	ownerColour(37);
	printf("|%4s|\n", drawPrices[37]);						// park place
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[13]);						// states avenue
	printf("\t\t\t\t\t\t\t    ");
	printf("| %s |\n", drawPlayersOnBoard[37]);						// park place


	// ninth row
	textColour(14,0);
	printf("|-EC-|");						// electric company
	textColour(9, 15);
	printf("|JAIL|");						// jail
	textColour(7, 0);
	printf("\t\t\t\t\t\t    ");
	textColour(4, 15);
	printf("|TAX!|\n");						// lux tax

	ownerColour(12);
	printf("|%4s|", drawPrices[12]);						// electric company
	textColour(7, 0);
	printf("| %s |", drawPlayersOnBoard[40]);						// jail
	printf("\t\t\t\t       .-----.      ");							// chance cards
	printf("|$75 |\n");						// lux tax

	printf("| %s |", drawPlayersOnBoard[12]);						// electric company
	printf("| %s |", drawPlayersOnBoard[41]);						// jail
	printf("\t\t\t\t       |  ?  |      ");							// chance cards
	printf("| %s |\n", drawPlayersOnBoard[38]);						// lux tax


	// tenth row
	textColour(13, 0);
	printf("|-SC-|");						// st charles place
	textColour(7, 0);
	printf("| %s |", drawPlayersOnBoard[42]);						// jail
	printf("\t\t\t\t       \'-----\'      ");							// chance cards
	textColour(9, 15);
	printf("|-BW-|\n");						// boardwalk

	ownerColour(11);
	printf("|%4s|", drawPrices[11]);						// st charles place
	textColour(7, 0);
	printf("| %s |", drawPlayersOnBoard[43]);						// jail
	printf("\t\t\t\t\t\t    ");
	ownerColour(39);
	printf("|%4s|\n", drawPrices[39]);						// boardwalk
	textColour(7, 0);

	printf("| %s |", drawPlayersOnBoard[11]);						// st charles place
	printf("------------------------------------------------------");
	printf("| %s |\n", drawPlayersOnBoard[39]);						// boardwalk


	// eleventh row

	textColour(15, 0);
	printf("|-JV-|");						// just visiting
	textColour(11, 0);
	printf("|-CA-|");						// conneticut ave
	printf("|-VT-|");						// vermont ave
	textColour(15, 0);
	printf("|-C?-|");						// chance
	textColour(11, 0);
	printf("|-OA-|");						// oriental ave
	textColour(8, 0);
	printf("|-RR-|");						// reading rr
	textColour(4, 15);
	printf("|TAX!|");						// income tax
	textColour(10, 0);
	printf("|-BA-|");						// baltic ave
	textColour(15, 0);
	printf("|-CC-|");						// bottom community chest
	textColour(10, 0);
	printf("|-MA-|");						// mediterranean ave
	textColour(2, 15);
	printf("|-GO-|\n");						// go

	textColour(7, 0);
	printf("|    |");						// just visiting
	ownerColour(9);
	printf("|%4s|", drawPrices[9]); 						// conneticut ave
	textColour(7, 0);
	ownerColour(8);
	printf("|%4s|", drawPrices[8]); 						// vermont ave
	textColour(7, 0);
	printf("|    |"); 						// chance
	ownerColour(6);
	printf("|%4s|", drawPrices[6]); 						// oriental ave
	textColour(7, 0);
	ownerColour(5);
	printf("|%4s|", drawPrices[5]); 						// reading rr
	textColour(7, 0);
	printf("| 10%%|"); 						// income tax
	ownerColour(3);
	printf("|%4s|", drawPrices[3]); 						// baltic ave
	textColour(7, 0);
	printf("|    |"); 						// bottom community chest
	ownerColour(1);
	printf("|%4s|", drawPrices[1]); 						// mediterranean ave
	textColour(7, 0);
	printf("|$200|\n");						// go

	printf("| %s |", drawPlayersOnBoard[10]);						// just visiting
	printf("| %s |", drawPlayersOnBoard[9]); 						// conneticut ave
	printf("| %s |", drawPlayersOnBoard[8]); 						// vermont ave
	printf("| %s |", drawPlayersOnBoard[7]); 						// chance
	printf("| %s |", drawPlayersOnBoard[6]); 						// oriental ave
	printf("| %s |", drawPlayersOnBoard[5]); 						// reading rr
	printf("| %s |", drawPlayersOnBoard[4]); 						// income tax
	printf("| %s |", drawPlayersOnBoard[3]); 						// baltic ave
	printf("| %s |", drawPlayersOnBoard[2]); 						// bottom community chest
	printf("| %s |", drawPlayersOnBoard[1]); 						// mediterranean ave
	printf("| %s |", drawPlayersOnBoard[0]); 						// go

	// bottom of board
	printf("\n %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196);
	
	// player stats
	switch (gm.playerAmount)
	{
	case 2: //2 players

		printf("|"); //prints player names with corresponding colours
		textColour(9,15);
		printf("       %2s       ", gm.playerNames[0]);
		textColour(10,0);
		printf("       %2s       ", gm.playerNames[1]);
		textColour(7,0);
		printf("\t\t\t\t |\n"); //prints player money
		printf("|     $%d\t      $%d\t\t\t\t\t |\n", gm.playerMoney[0], gm.playerMoney[1]);
		//prints the square the player is currently on (blank if bankrupt)
		printf("|       %s\t\t%s\t\t\t\t\t |\n", gm.squareNames[gm.playerLocation[0]], gm.squareNames[gm.playerLocation[1]]);
		if (gm.bankrupt[0] == 1) //if they are bankrupt, print that. otherwise, print how many get out of jail free cards they have
		{
			printf("|    BANKRUPT ");
		}
		else
		{
			printf("|    GOOJFC: %1d", numOfGetOutOfJailFree[0]);
		}
		if (gm.bankrupt[1] == 1)
		{
			printf("\t     BANKRUPT\t\t\t\t\t |\n");
		}
		else
		{
			printf("\t     GOOJFC: %1d\t\t\t\t\t |\n",numOfGetOutOfJailFree[1]);
		}

		break;
	case 3: //same as above, but for 3 players
		printf("|"); //prints player names with corresponding colours
		textColour(9,15); 
		printf("       %2s       ", gm.playerNames[0]);
		textColour(10,0);
		printf("       %2s       ", gm.playerNames[1]);
		textColour(12,0);
		printf("       %2s       ", gm.playerNames[2]);
		textColour(7,0);
		printf("\t\t |\n"); //prints player money
		printf("|     $%d\t      $%d\t      $%d\t\t\t |\n", gm.playerMoney[0], gm.playerMoney[1], gm.playerMoney[2]);
		//prints the square the player is currently on (blank if bankrupt)
		printf("|       %s\t\t%s\t\t%s\t\t\t |\n", gm.squareNames[gm.playerLocation[0]], gm.squareNames[gm.playerLocation[1]], gm.squareNames[gm.playerLocation[2]]);
		if (gm.bankrupt[0] == 1) //if they are bankrupt, print that. otherwise, print how many get out of jail free cards they have
		{
			printf("|    BANKRUPT ");
		}
		else
		{
			printf("|    GOOJFC: %1d", numOfGetOutOfJailFree[0]);
		}
		if (gm.bankrupt[1] == 1)
		{
			printf("\t      BANKRUPT \t");
		}
		else
		{
			printf("\t     GOOJFC: %1d\t", numOfGetOutOfJailFree[1]);
		}
		if (gm.bankrupt[2] == 1)
		{
			printf("      BANKRUPT \t\t\t |\n");
		}
		else
		{
			printf("     GOOJFC: %1d\t\t\t |\n", numOfGetOutOfJailFree[2]);
		}

		break;
	default:
		printf("|"); //prints player names with corresponding colours
		textColour(9,15);
		printf("       %2s       ", gm.playerNames[0]);
		textColour(10,0);
		printf("       %2s       ", gm.playerNames[1]);
		textColour(12,0);
		printf("       %2s       ", gm.playerNames[2]);
		textColour(14,0);
		printf("       %2s       ", gm.playerNames[3]);
		textColour(7,0);
		printf("|\n"); //prints player money
		printf("|     $%d\t      $%d\t      $%d\t      $%d\t |\n", gm.playerMoney[0], gm.playerMoney[1], gm.playerMoney[2], gm.playerMoney[3]);
		//prints the square the player is currently on (blank if bankrupt)
		printf("|       %s\t\t%s\t\t%s\t\t%s\t |\n", gm.squareNames[gm.playerLocation[0]], gm.squareNames[gm.playerLocation[1]], gm.squareNames[gm.playerLocation[2]], gm.squareNames[gm.playerLocation[3]]);
		if (gm.bankrupt[0] == 1) //if they are bankrupt, print that. otherwise, print how many get out of jail free cards they have
		{
			printf("|    BANKRUPT ");
		}
		else
		{
			printf("|    GOOJFC: %1d", numOfGetOutOfJailFree[0]);
		}
		if (gm.bankrupt[1] == 1)
		{
			printf("\t     BANKRUPT \t");
		}
		else
		{
			printf("\t     GOOJFC: %1d\t", numOfGetOutOfJailFree[1]);
		}
		if (gm.bankrupt[2] == 1)
		{
			printf("     BANKRUPT\t");
		}
		else
		{
			printf("     GOOJFC: %1d\t", numOfGetOutOfJailFree[2]);
		}
		if (gm.bankrupt[3] == 1)
		{
			printf("     BANKRUPT \t |\n");
		}
		else
		{
			printf("     GOOJFC: %1d\t |\n",numOfGetOutOfJailFree[3]);
		}
		break;
	}

	// bottom of player stats
	printf(" %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n\n", 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196);

}