﻿//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c � group assignment
//Author: Pascale Vacher
//Last updated: 24 February 2017
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
using namespace std;

//include our own libraries
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(16);    	//horizontal dimension
const int  SIZEY(11);		//vertical dimension
//defining symbols used for display of the grid and content
const char BEAR('@');   	//bear
const char PROTECTEDBEAR('£');
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char BOMB('0');		//bomb
const char PILL('P');		//pill
const char DETONATOR('T');	//detonator
const char LOCK('&');
const char KEY('F');
const char ROCK('Q');
const char EXIT('X');
//defining the command letters to move the bear on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char CHEAT('C');		//disable bombs

struct Item {
	int x, y;
	char symbol;
	bool isProtected;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main() {
	//function declarations (prototypes)
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, vector<Item>& rocks, int level);
	void paintGame(const char g[][SIZEX], string mess, string name, int scoreMove, int rescued, int previousScore, int level, const vector<Item>& bears);
	bool wantsToQuit(const int key);
	bool enableCheatMode(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void entryScreen(string, string&);
	void levelSelection(string, string, string&, int&, int&, bool&);
	void sortBears(vector<Item>& bears, const int key, const int rescued);
	void updateGameData(char g[][SIZEX], char maze[][SIZEX], int, vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, vector<Item>& rocks, const int key, string& mess, bool& finishGame, bool& levelCompleted, bool cheatMode, int& rescued, int level);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Item> bears, const vector<Item> bombs, const vector<Item> pills, const vector<Item> locks);
	void readScoreTxt(string, string, int&);
	void recordPlayerTxt(string, string, int, int, int);
	void resetStats(vector<Item>& bombs, vector<Item>& locks, int& scoreMove, int& rescued);
	void endLevel(string levelString);
	void endProgram(bool finishGame, vector<Item>& bears);

	//Colors in game
	HANDLE  hconsole;
	hconsole = GetStdHandle(STD_OUTPUT_HANDLE);

	//Reset the random function
	srand(time(NULL));

	//local variable declarations
	bool finishGame = false;
	bool levelCompleted = false;
	bool cheatMode = false;
	bool unlockMode = false;
	int level(1), scoreMove(0), rescued(0), moves(0), previousScore(500), levelRecord(level);
	char grid[SIZEY][SIZEX];	//grid for display
	char maze[SIZEY][SIZEX];	//structure of the maze
	string message;	//current message to player
	string playerName;
	string levelString = to_string(level);
	vector<Item> bears;
	vector<Item> bombs;
	vector<Item> pills;
	vector<Item> locks;
	vector<Item> rocks;

	//action...
	entryScreen(message, playerName);
	levelSelection(message, playerName, levelString, level, levelRecord, unlockMode);
	readScoreTxt(playerName, levelString, previousScore);
	initialiseGame(grid, maze, bears, bombs, pills, locks, rocks, level);	//initialise grid (incl. walls & bear)
	paintGame(grid, message, playerName, scoreMove, rescued, previousScore, level, bears);			//display game info, modified grid & messages
	showMessage(clBlack, clWhite, 40, 8, "LET'S START!          ");
	int key(getKeyPress()); 			//read in  selected key: arrow or letter command
	while ((!wantsToQuit(key)) && (finishGame == false)) {
		//while user does not want to quit
		if (isArrowKey(key))
		{
			sortBears(bears, key, rescued);
			scoreMove++;
			moves++;
			updateGameData(grid, maze, moves, bears, bombs, pills, locks, rocks, key, message, finishGame, levelCompleted, cheatMode, rescued, level);		//move bear in that direction
			//updateGrid(grid, maze, bears, bombs);			//update grid information - disabled for testing, updateGrid exist in updateGameData
		}
		else{
			if (enableCheatMode(key)) {
				if (cheatMode == false) {
					cheatMode = true;
					scoreMove = 500;
					showMessage(clBlack, clWhite, 40, 7, "CHEAT MODE ACTIVATED!");
					showMessage(clBlack, clWhite, 40, 8, "Bombs Deactivated!   ");

					Beep(523, 200);				// Beep sounds when cheatmode activated.
					Beep(523, 200);				// Beep sounds when cheatmode activated.
					Beep(523, 200);				// Beep sounds when cheatmode activated.
				}
				else {
					cheatMode = false;
					showMessage(clBlack, clBlack, 40, 7, "                     ");
					showMessage(clBlack, clBlack, 40, 8, "                     ");
				}
			}
			else {
				message = "INVALID KEY!         ";	//set 'Invalid key' message
			}
			message = "                     ";
		}
		paintGame(grid, message, playerName, scoreMove, rescued, previousScore, level, bears);	//display game info, modified grid & messages
		key = getKeyPress(); 		// display menu & read in next option

		// initialise the next level
		if (levelCompleted == true) {
			levelCompleted = false;	// reset the state of a completed level in the next one
			cheatMode = false;		// reset cheat mode in the next level
			showMessage(clBlack, clBlack, 40, 7, "                     ");
			showMessage(clBlack, clBlack, 40, 8, "                     ");

			// record the player's highest level completion
			if ((scoreMove <= previousScore) && (unlockMode == false) && (bears.empty())) {
				recordPlayerTxt(playerName, levelString, scoreMove, level + 1, levelRecord);
			}

			endLevel(levelString);	// display level completion message
			resetStats(bombs, locks, scoreMove, rescued);	// reset the scores to 0
			level++;	// increment the level count
			levelString = to_string(level);	// create a string version of the level count for the filename
			readScoreTxt(playerName, levelString, previousScore);	// read the player's highest score of the next level
			initialiseGame(grid, maze, bears, bombs, pills, locks, rocks, level);	// reinitialise the game with the next level
			paintGame(grid, message, playerName, scoreMove, rescued, previousScore, level, bears);	// update the next level's descriptions
		}
	}

	// record the player's highest record after game completion
	if ((scoreMove <= previousScore) && (finishGame == true) && (unlockMode == false) && (bears.empty())) {
		recordPlayerTxt(playerName, levelString, scoreMove, level, levelRecord);
	}

	endProgram(finishGame, bears);	//display final message
	return 0;
}

void entryScreen(string message, string& playerName) {
	//Show the first info screen to the player with score

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	//display game title and bear image
	showMessage(clDarkGrey, clGreen, 15, 2, "	   _      _                                 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  ");
	showMessage(clDarkGrey, clGreen, 15, 3, "   : `.--.' ;              _....,_          /              MENU                /| ");
	showMessage(clDarkGrey, clGreen, 15, 4, "   .'      `.      _..--'''       `-._     /_ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _ _/ | ");
	showMessage(clDarkGrey, clGreen, 15, 5, "  :          :_.-''                  .`.   |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 6, "  :  6    6  :                     :  '.;  |     THREE  BEARS  GAME - FOP    |  | ");
	showMessage(clDarkGrey, clGreen, 15, 7, "  :          :                      `..';  |            GROUP RR             |  | ");
	showMessage(clDarkGrey, clGreen, 15, 8, "  `: .----. :'                          ;  |       Greg, Andy and Irek       |  | ");
	showMessage(clDarkGrey, clGreen, 15, 9, "    `._Y _.'               '           ;   |      Computer Science - 2017    | /  ");
	showMessage(clDarkGrey, clGreen, 15, 10, "       'U'      .'          `.         ;   |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|/   ");
	showMessage(clDarkGrey, clGreen, 15, 11, "          `:   ;`-..___       `.     .'`.                    ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 12, "           :   :  :    ```'''''``.    `.  `.                 ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 13, "        .'     ;..'            .'       `.'`                 ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 14, "       `.......'              `........-'`                   ||                   ");

	showMessage(clBlack, clWhite, 55, 18, "______________________");

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 47, 16, message);	//display current message

	showMessage(clBlack, clWhite, 40, 18, "Enter your name: ");
	getline(cin, playerName);								// Reads in the name of the player and stores it in "String name".
	
	// ensure that string length does not exceed 20 characters
	if (playerName.length() > 20) {
		playerName.resize(20);
	}
}

void levelSelection(string message, string playerName, string& levelString, int& level, int& levelRecord, bool& unlockMode) {

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void readPlayerRecord(string, int&);
	void hiddenLevels(int);

	// read the player's highest reached level
	readPlayerRecord(playerName, levelRecord);

	// display available levels and bear image
	showMessage(clDarkGrey, clGreen, 15, 2, "	   _      _                                 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  ");
	showMessage(clDarkGrey, clGreen, 15, 3, "   : `.--.' ;              _....,_          /              MENU                /| ");
	showMessage(clDarkGrey, clGreen, 15, 4, "   .'      `.      _..--'''       `-._     /_ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _ _/ | ");
	showMessage(clDarkGrey, clGreen, 15, 5, "  :          :_.-''                  .`.   |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 6, "  :  6    6  :                     :  '.;  |       1. Maze with detonator    |  | ");
	showMessage(clDarkGrey, clGreen, 15, 7, "  :          :                      `..';  |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 8, "  `: .----. :'                          ;  |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 9, "    `._Y _.'               '           ;   |                                 | /  ");
	showMessage(clDarkGrey, clGreen, 15, 10, "       'U'      .'          `.         ;   |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|/   ");
	showMessage(clDarkGrey, clGreen, 15, 11, "          `:   ;`-..___       `.     .'`.                    ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 12, "           :   :  :    ```'''''``.    `.  `.                 ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 13, "        .'     ;..'            .'       `.'`                 ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 14, "       `.......'              `........-'`                   ||                   ");

	// display the level options if unlocked
	hiddenLevels(levelRecord);

	showMessage(clBlack, clWhite, 40, 18, "Please select the desired level: ");
	getline(cin, levelString);	// Read the level option

	// unlock all levels if the "u" button has been pressed
	if (levelString == "u") {
		unlockMode = true;	// prevent file recording
		levelRecord = 3;	// unlock all levels in unlock mode
		hiddenLevels(levelRecord);	// display all levels in unlock mode
		showMessage(clBlack, clWhite, 40, 19, "All levels unlocked. Choose a level.");
		Beep(523, 200);		// Beep sounds when cheatmode activated
		Beep(523, 200);		// Beep sounds when cheatmode activated
		Beep(523, 200);		// Beep sounds when cheatmode activated
		showMessage(clBlack, clWhite, 40, 18, "Please select the desired level: ");
		getline(cin, levelString);	// Re-read a new level
	}

	// convert the string to int to declare the newly selected level as the current one
	level = atoi(levelString.c_str());	

	// set level to 1 if the selection does not represent any levels given
	if (!((level >= 1) && (level <= levelRecord))) {
		level = 1;
		levelString = to_string(level);
	}

	// Clear the level selection screen
	Clrscr(); 
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, vector<Item>& rocks, int level) {
	//initialise grid & place bear in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setMazeStructureLevel2(char maze[][SIZEX]);
	void setMazeStructureLevel3(char maze[][SIZEX]);
	void setInitialDataFromMaze(char maze[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& locks, vector<Item>& rocks);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Item> bears, const vector<Item> bombs, const vector<Item> pills, const vector<Item> locks, const vector<Item> rocks);

	// draw the maze dependent on the level count
	switch (level)
	{
	case 1:
		setInitialMazeStructure(maze);		//initialise maze
		break;
	case 2:
		setMazeStructureLevel2(maze);		//Level 2
		break;
	case 3:
		setMazeStructureLevel3(maze);		//Level 3
		break;
	default:
		break;
	}
	setInitialDataFromMaze(maze, bears, bombs, locks, rocks);	//initialise bear's position
	updateGrid(grid, maze, bears, bombs, pills, locks, rocks);		//prepare grid
}

void setInitialMazeStructure(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
	= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 3, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1 },
		{ 1, 2, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 2, 1, 0, 1, 5, 0, 0, 0, 3, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 3, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (initialMaze[row][col])
			{
				case 0: maze[row][col] = TUNNEL; break;
				case 1: maze[row][col] = WALL; break;
				case 2: maze[row][col] = BEAR; break;
				case 3: maze[row][col] = BOMB; break;
				case 4:	maze[row][col] = DETONATOR; break;
				case 5: maze[row][col] = EXIT; break;
			}
		}
	}
}

void setMazeStructureLevel2(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int maze2[SIZEY][SIZEX] 	//local array to store the maze structure
	= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 3, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1 },
		{ 1, 2, 1, 0, 1, 1, 1, 1, 3, 1, 4, 1, 0, 1, 0, 1 },
		{ 1, 2, 1, 0, 1, 6, 0, 0, 0, 3, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze2[row][col])
			{
			case 0: maze[row][col] = TUNNEL; break;
			case 1: maze[row][col] = WALL; break;
			case 2: maze[row][col] = BEAR; break;
			case 3: maze[row][col] = BOMB; break;
			case 4: maze[row][col] = LOCK; break;
			case 5:	maze[row][col] = KEY; break;
			case 6: maze[row][col] = EXIT; break;
			}
		}
	}
}

void setMazeStructureLevel3(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int maze3[SIZEY][SIZEX] 	//local array to store the maze structure
	= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 0, 1 },
		{ 1, 2, 4, 0, 0, 0, 0, 0, 4, 4, 0, 4, 0, 4, 0, 1 },
		{ 1, 2, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 0, 4, 0, 1 },
		{ 1, 0, 4, 4, 4, 4, 0, 0, 0, 4, 0, 4, 0, 4, 0, 1 },
		{ 1, 0, 4, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 1 },
		{ 1, 0, 4, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 1 },
		{ 1, 0, 4, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 1 },
		{ 1, 0, 4, 4, 0, 4, 0, 4, 0, 0, 0, 4, 4, 4, 3, 1 },
		{ 1, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 4, 4, 4, 5, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze3[row][col])
			{
			case 0: maze[row][col] = TUNNEL; break;
			case 1: maze[row][col] = WALL; break;
			case 2: maze[row][col] = BEAR; break;
			case 3: maze[row][col] = BOMB; break;
			case 4: maze[row][col] = ROCK; break;
			case 5:	maze[row][col] = EXIT; break;
			}
		}
	}
}

void setInitialDataFromMaze(char maze[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& locks, vector<Item>& rocks) {
	//extract bear's coordinates from initial maze info
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze[row][col])
			{
				case BEAR:
				{
					Item bear = { col, row, BEAR, false };
					bears.push_back(bear);
					maze[row][col] = TUNNEL;
					break;
				}
				case BOMB:
				{
					const Item bomb = { col, row, BOMB };
					bombs.push_back(bomb);
					maze[row][col] = TUNNEL;
					break;
				}
				case LOCK:
				{
					const Item lock = { col, row, LOCK };
					locks.push_back(lock);
					maze[row][col] = TUNNEL;
					break;
				}
				case ROCK:
				{
					const Item rock = { col, row, ROCK };
					rocks.push_back(rock);
					maze[row][col] = TUNNEL;
					break;
				}
				default:
					break;
					//will work for other bombs too
			}
		}
	}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const vector<Item> bears, const vector<Item> bombs, const vector<Item> pills, const vector<Item> locks, const vector<Item> rocks) {
	//update grid configuration after each move
	void setMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeBombs(char g[][SIZEX], const vector<Item>& bombs, const vector<Item>& pills);
	void placeLocks(char g[][SIZEX], const vector<Item>& locks);
	void placeRocks(char g[][SIZEX], const vector<Item>& rocks);
	void placeBears(char g[][SIZEX], const vector<Item>& bears);

	setMaze(grid, maze);	//reset the empty maze configuration into grid
	placeBombs(grid, bombs, pills);	// set bombs in grid - remove them if they're deleted
	placeLocks(grid, locks);	// set lock in grid - remove it if deleted
	placeRocks(grid, rocks);	// set rocks in grid
	placeBears(grid, bears);	// set bears in grid
}

void setMaze(char grid[][SIZEX], const char maze[][SIZEX]) {
	//reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			grid[row][col] = maze[row][col];
		}
	}
}

void placeBears(char grid[][SIZEX], const vector<Item>& bears) {
	//place bear at its new position in grid
	for (size_t pos(0); pos < bears.size(); ++pos) {
		grid[bears.at(pos).y][bears.at(pos).x] = bears.at(pos).symbol;
	}
}

void placeBombs(char grid[][SIZEX], const vector<Item>& bombs, const vector<Item>& pills) {
	for (size_t pos(0); pos < bombs.size(); ++pos)
	{
		grid[bombs.at(pos).y][bombs.at(pos).x] = bombs.at(pos).symbol;
	}

	for (size_t pos(0); pos < pills.size(); ++pos)
	{
		grid[pills.at(pos).y][pills.at(pos).x] = pills.at(pos).symbol;

	}
}

void placeLocks(char grid[][SIZEX], const vector<Item>& locks) {
	for (size_t pos(0); pos < locks.size(); ++pos)
	{
		grid[locks.at(pos).y][locks.at(pos).x] = locks.at(pos).symbol;
	}
}

void placeRocks(char grid[][SIZEX], const vector<Item>& rocks) {
	for (size_t pos(0); pos < rocks.size(); ++pos)
	{
		grid[rocks.at(pos).y][rocks.at(pos).x] = rocks.at(pos).symbol;
	}
}

//---------------------------------------------------------------------------
//----- move the bear
//---------------------------------------------------------------------------
void updateGameData(char g[][SIZEX], char maze[][SIZEX], int moves, vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, vector<Item>& rocks, const int key, string& mess, bool& finishGame, bool& levelCompleted, bool cheatMode, int& rescued, int level) {
	//move bear in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void createThePill(char g[][SIZEX], const int, vector<Item>& pills);
	void paintGrid(const char g[][SIZEX], const vector<Item>& bears);
	assert(isArrowKey(key));

	//calculate direction of movement for given key
	int dx(0), dy(0);
	bool steppedOnBomb = false;
	setKeyDirection(key, dx, dy);

	if (moves % 10 == 0){
		pills.clear();
		createThePill(g, moves, pills);
		moves = 0;
	}

	for (size_t pos(0); pos < bears.size(); ++pos) {
		//check new target position in grid and update game data (incl. bear coordinates) if move is possible
		switch (g[bears.at(pos).y + dy][bears.at(pos).x + dx])
		{			//...depending on what's on the target position in grid...
			case TUNNEL:		//can move
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go in that X direction
				mess = "                                     ";
				break;
			case WALL:  		//hit a wall and stay there
				cout << '\a';	//beep the alarm - Disabled as the alarm is annoying
				break;
			case BEAR:
				bears.at(pos).y += 0;	//go in that Y direction
				bears.at(pos).x += 0;	//go in that X direction
				break;
			case BOMB:			//hit a bomb and the bear disappears and the game ends
				if (!cheatMode && !bears.at(pos).isProtected) {
					cout << '\a';		//beep the alarm
					mess = "BEAR DIES!                           ";
					steppedOnBomb = true;
					bears.at(pos).symbol = ' ';
					finishGame = true;
					break;
				}
				else {
					bears.at(pos).y += dy;	//go in that Y direction
					bears.at(pos).x += dx;	//go in that X direction
					break;
				}
			case DETONATOR:
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go in that X direction	
				if (cheatMode == false)
				{
					bombs.clear();
					mess = "BOMBS DEACTIVATED!   ";	//set 'Invalid key' message
				}
				break;
			case KEY:
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go in that X direction	
				locks.clear();
				mess = "LOCK REMOVED!        ";	//set 'Invalid key' message
				break;
			case ROCK:
				//bears.at(pos).y += dy;	//go in that Y direction
				//bears.at(pos).x += dx;	//go in that X direction	
				rocks.at(pos).y += dy;	//go in that Y direction
				rocks.at(pos).x += dx;	//go in that X direction
				mess = "                                     ";
			case PILL:
				bears.at(pos).isProtected = true;
				bears.at(pos).y += dy;
				bears.at(pos).x += dx;
				bears.at(pos).symbol = PROTECTEDBEAR;
				pills.clear();
				break;
			case EXIT:
				cout << '\a';		//beep the alarm
				rescued++;
				mess = "BEAR RESCUED!                        ";
				bears.erase(bears.begin() + pos);
				if (rescued == 3){
					bears.clear();
				}
				if (bears.empty() == true) {
					if (level != 3) {
						levelCompleted = true;
					}
					else {
						finishGame = true;	// confirms the game's completion
					}
				}
		}
		updateGrid(g, maze, bears, bombs, pills, locks, rocks);
	}
}

void createThePill(char g[][SIZEX], const int moves, vector<Item>& pills) {

	int col = rand() % 16 + 1;
	int row = rand() % 11 + 1;

	while (pills.empty())
	{
		if (g[row][col] == TUNNEL){
			Item pill = { col, row, PILL };
			pills.push_back(pill);
			g[row][col] = PILL;
		}
		else{
			col = rand() % 16 + 1;
			row = rand() % 11 + 1;
		}
	}

}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy) {
	//calculate direction indicated by key
	bool isArrowKey(const int k);
	//assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP: 		//when UP arrow pressed...
		dx = 0;
		dy = -1;	//decrease the Y coordinate
		break;
	case DOWN: 		//when DOWN arrow pressed...
		dx = 0;
		dy = +1;	//increase the Y coordinate
		break;
	}
}

int getKeyPress() {
	//get key or command (in uppercase) selected by user
	//KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) { 	//ignore symbol following cursor key
		keyPressed = _getch();
	}
	return toupper(keyPressed);		//return it in uppercase 
}

bool isArrowKey(const int key) {
	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool wantsToQuit(const int key) {
	//check if the user wants to quit (when key is 'Q' or 'q')
	return toupper(key) == QUIT;
}

bool enableCheatMode(const int key) {
	// Checks if user has enabled cheat mode (disable bombs)
	return toupper(key) == CHEAT;
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(char x) {
	std::ostringstream os;
	os << x;
	return os.str();
}

void paintGame(const char g[][SIZEX], string mess, string playerName, int scoreMove, int rescued, int previousScore, int level, const vector<Item>& bears) {
	//display game title, messages, maze, bear and other bombs on screen
	string tostring(char x);
	string prevScore = to_string(previousScore);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX], const vector<Item>& bears);
	void reduceVisibility(const char g[][SIZEX], const vector<Item>& bears);


	// retrieve current time and date.
	auto t = time(nullptr);
	auto tm = *localtime(&t);

	ostringstream oss;
	oss << put_time(&tm, "%H:%M:%S %d/%m/%Y");
	auto str = oss.str();

	// clear level completion messages after transition
	showMessage(clBlack, clWhite, 40, 22, "                                   ");
	showMessage(clBlack, clWhite, 40, 23, "                                   ");

	// display game title
	showMessage(clYellow, clBlack, 0, 0, "THREE BEARS GAME - GROUP 1RR");

	// display number of rescued bears
	showMessage(clDarkGrey, clYellow, 0, 2, "RESCUED:        ");
	switch (rescued)
	{
		case 1: showMessage(clDarkGrey, clGreen, 9, 2, "@      ");
			break;
		case 2: showMessage(clDarkGrey, clGreen, 9, 2, "@@     ");
			break;
		case 3: showMessage(clDarkGrey, clGreen, 9, 2, "@@@    ");
			break;
		default:
			break;
	}

	// calculate length of current player line and then output the correct number of blank spaces
	string nameSpaces;
	for (int spaces = 37 - (playerName.length() + 17); spaces > 0; spaces--) {
		nameSpaces = nameSpaces + " ";
	}

	// calculate length of score line and then output the correct number of blank spaces
	string scoreSpaces;
	for (int spaces = 37 - (prevScore.length() + 17); spaces > 0; spaces--) {
		scoreSpaces = scoreSpaces + " ";
	}

	// display details
	showMessage(clDarkGrey, clYellow, 40, 0, " CURRENT PLAYER: " + playerName + nameSpaces);
	showMessage(clDarkGrey, clYellow, 40, 1, " PREVIOUS SCORE: " + prevScore + scoreSpaces);
	showMessage(clDarkGrey, clYellow, 40, 2, " DATE AND TIME : " + str + " ");

	// display moves and rescued bears
	showMessage(clBlack, clWhite, 40, 5, "MOVES SO FAR: ");
	cout << scoreMove;
	showMessage(clBlack, clWhite, 40, 6, "BEARS OUT: ");
	cout << rescued;

	// display level descriptions
	switch (level)
	{
	case 1:
		showMessage(clDarkGrey, clWhite, 40, 13, " GAME LEVEL 1 RULES                  ");
		showMessage(clDarkGrey, clWhite, 40, 16, " To disable bombs use detonator 'T'  ");
		break;
	case 2:
		showMessage(clDarkGrey, clWhite, 40, 13, " GAME LEVEL 2 RULES                  ");
		showMessage(clDarkGrey, clWhite, 40, 16, " To remove the lock use key 'F'      ");
		break;
	case 3:
		showMessage(clDarkGrey, clWhite, 40, 13, " GAME LEVEL 3 RULES                  ");
		showMessage(clDarkGrey, clWhite, 40, 16, " Move rock 'Q' to reach the exit     ");
		break;
	}
	showMessage(clDarkGrey, clWhite, 40, 14, " Rescue all the bears '@' through    ");
	showMessage(clDarkGrey, clWhite, 40, 15, " exit 'X' avoiding bombs 'O'         ");
	showMessage(clDarkGrey, clWhite, 40, 17, "                                     ");
	showMessage(clDarkGrey, clWhite, 40, 18, " TO MOVE USE KEYBOARD ARROWS         ");
	showMessage(clDarkGrey, clWhite, 40, 19, " TO QUIT ENTER 'Q'                   ");

	// print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message

	// display grid contents
	paintGrid(g, bears);

	//´symbol description
	showMessage(clDarkGrey, clWhite, 0, 16, "BEAR         ");
	showMessage(clDarkGrey, clGreen, 10, 16, "@     ");
	showMessage(clDarkGrey, clWhite, 0, 17, "BOMB          ");
	showMessage(clDarkGrey, clRed, 10, 17, "0     ");
	showMessage(clDarkGrey, clWhite, 0, 18, "DETONATOR      ");
	showMessage(clDarkGrey, clYellow, 10, 18, "T     ");
	showMessage(clDarkGrey, clWhite, 0, 19, "EXIT         ");
	showMessage(clDarkGrey, clBlack, 10, 19, "X     ");
	if (level == 2) {
		showMessage(clDarkGrey, clWhite, 0, 18, "KEY            ");
		showMessage(clDarkGrey, clCyan, 10, 18, "F     ");
		showMessage(clDarkGrey, clWhite, 0, 19, "LOCK         ");
		showMessage(clDarkGrey, clCyan, 10, 19, "&     ");
		showMessage(clDarkGrey, clWhite, 0, 20, "EXIT         ");
		showMessage(clDarkGrey, clBlack, 10, 20, "X     ");
	}
	else if (level == 3) {
		showMessage(clDarkGrey, clWhite, 0, 18, "ROCK           ");
		showMessage(clDarkGrey, clMagenta, 10, 18, "Q     ");
		showMessage(clDarkGrey, clWhite, 0, 19, "EXIT         ");
		showMessage(clDarkGrey, clBlack, 10, 19, "X     ");
		showMessage(clBlack, clBlack, 0, 20, "             ");
		showMessage(clBlack, clBlack, 10, 20, "      ");
	}
}

void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message) {
	Gotoxy(x, y);
	SelectBackColour(backColour);
	SelectTextColour(textColour);
	cout << message;
}

void paintGrid(const char g[][SIZEX], const vector<Item>& bears) {
	//display grid content on screen
	void setColor(int);
	void resetColor();

	Gotoxy(0, 4);

	int pos(0);

	// Paint the whole Grid, item by item
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			SelectBackColour(clBlack);	//	Sets the background colour to black
			SelectTextColour(clWhite);	//	Sets the walls colour to white

			switch (g[row][col])		//	Looks for special characters and paint them for chosen colours
			{
			case BEAR:
			{
				setColor(10);			// Paint the Bear green
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case PROTECTEDBEAR:
				setColor(2);
				cout << BEAR;
				resetColor();
				break;
			case BOMB:
			{
				setColor(12);			// Paint the Bomb red
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case DETONATOR:
			{
				setColor(14);			// Paint the Detonator yellow
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case PILL:
				setColor(11);
				cout << g[row][col];
				resetColor();
				break;
			case LOCK:
			{
				setColor(11);			// Paint the Detonator cyan
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case KEY:
			{
				setColor(11);			// Paint the Detonator blue
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case ROCK:
			{
				setColor(13);			// Paint the Rocks magenta
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case EXIT:
				SelectBackColour(clWhite);
				SelectTextColour(clBlack);			
				cout << g[row][col];
				break;

			default:
				cout << g[row][col];
				break;
			}
		}
		cout << endl;
	}
}

void reduceVisibility(const char g[][SIZEX], const vector<Item>& bears) {
	//display grid content on screen
	void setColor(int);
	void resetColor();
	bool visible(false);

	Gotoxy(0, 4);

	int pos(0);

	// Paint the whole Grid, item by item
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			SelectBackColour(clBlack);	//	Sets the background colour to black
			SelectTextColour(clWhite);	//	Sets the walls colour to white

			int pos(0);
			while ((pos < bears.size()) && (visible == false))
			{
				if ((row > bears.at(pos).y - 2 && row < bears.at(pos).y + 2) && (col > bears.at(pos).x - 2 && col < bears.at(pos).x + 2))
				{
					visible = true;
				}
				else
				{
					visible = false;
				}
				pos++;
			}

			if (visible == true)
			{
				switch (g[row][col])		//	Looks for special characters and paint them for chosen colours
				{
				case BEAR:
				{
					setColor(10);			// Paint the Bear green
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case BOMB:
				{
					setColor(12);			// Paint the Bomb red
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case DETONATOR:
				{
					setColor(14);			// Paint the Detonator yellow
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case EXIT:
					SelectBackColour(clWhite);
					SelectTextColour(clBlack);
					cout << g[row][col];
					break;
				default:
					cout << g[row][col];
					break;
				}
			}
			else
			{
				setColor(1);
				cout << g[row][col];
				resetColor();
			}
			visible = false;
		}
		cout << endl;
	}
}

void hiddenLevels(int levelRecord) {
	if (levelRecord > 1) {
		showMessage(clDarkGrey, clGreen, 15, 7, "  :          :                      `..';  |  2 - Maze with lock and key     |  | ");
		if (levelRecord > 2) {
			showMessage(clDarkGrey, clGreen, 15, 8, "  `: .----. :'                          ;  |  3 - Maze with slidable rocks   |  | ");
		}
	}
}

void readScoreTxt(string playerName, string level, int& previousScore) {
	ifstream fin;
	fin.open(playerName + "_LV" + level + ".txt", ios::in);
	fin >> previousScore;
	fin.close();
}

void readPlayerRecord(string playerName, int& levelRecord) {
	ifstream gameRecord;
	gameRecord.open(playerName + "_gameRecord.txt", ios::in);
	gameRecord >> levelRecord;
	gameRecord.close();
}

void recordPlayerTxt(string playerName, string levelString, int scoreMove, int level, int levelRecord) {
	ofstream fout(playerName + "_LV" + levelString + ".txt", ios::out);
	fout << scoreMove << endl;

	if (level >= levelRecord) {
		ofstream gameRecord(playerName + "_gameRecord.txt", ios::out);
		gameRecord << level << endl;
	}
}

void resetStats(vector<Item>& bombs, vector<Item>& locks, int& scoreMove, int& rescued) {
	bombs.clear();	// remove residual bombs available on previous level for the next one
	locks.clear();	// remove residual locks available on previous level for the next one
	showMessage(clBlack, clWhite, 40, 5, "                   ");	// reset score count
	scoreMove = 0;	// reset score count
	rescued = 0;	// reset number of bears rescued (vital for bears movement)
}

void endLevel(string level) {
	showMessage(clRed, clYellow, 40, 22, "LEVEL ");
	showMessage(clRed, clYellow, 46, 22, level);
	showMessage(clRed, clYellow, 47, 22, " COMPLETED!");
	showMessage(clBlack, clWhite, 40, 23, "");
	system("PAUSE");
}

void endProgram(bool finishGame, vector<Item>& bears) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	if (finishGame == false) {
		showMessage(clRed, clYellow, 40, 22, "END OF GAME: GAME STOPPED BY PLAYER");	//hold output screen until a keyboard key is hit
	}
	else {
		if (bears.empty() == true) {
			showMessage(clRed, clYellow, 40, 22, "END OF GAME: GAME WON!");
		}
		else {
			showMessage(clRed, clYellow, 40, 22, "END OF GAME: GAME OVER!");
		}
	}
	showMessage(clBlack, clWhite, 40, 23, "");
	system("pause");
}

// Changes the console colour to the value passed in
void setColor(int colour) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour);
}

// Resets the console colour to basic colour
void resetColor() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}


// This function rearranges the position of bears inside the vector
// Based on which key is pressed for the movement
void sortBears(vector<Item>& bears, const int key, const int rescued) {
	vector<Item> temp(1);
	int pos(0);

	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		if (rescued != 2) {
			if (bears[pos].x > bears[pos + 1].x) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].x > bears[pos + 2].x) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].x > bears[pos + 2].x) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		if (rescued != 2) {
			if (bears[pos].x < bears[pos + 1].x) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].x < bears[pos + 2].x) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].x < bears[pos + 2].x) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	case UP: 		//when UP arrow pressed...
		if (rescued != 2) {
			if (bears[pos].y > bears[pos + 1].y) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].y > bears[pos + 2].y) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].y > bears[pos + 2].y) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	case DOWN: 		//when DOWN arrow pressed...
		if (rescued != 2) {
			if (bears[pos].y < bears[pos + 1].y) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].y < bears[pos + 2].y) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].y < bears[pos + 2].y) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	}
}
