#include "stdafx.h"
#include "Database.h"
#include <iostream>
#include <algorithm>

using namespace std;

vector<Player> Database::playerList;
vector<Game> Database::gameList;
vector<Achievement> Database::achievementList;

Player Database::newPlayer(int id, string name)
{
	Player newPlayer = Player(id, name);
	playerList.push_back(newPlayer);
	return newPlayer;
}

Game Database::newGame(int id, string name)
{
	Game newGame = Game(id, name);
	gameList.push_back(newGame);
	return newGame;
}

Achievement Database::newAchievement(int gameID, int achID, string achName, int pts)
{
	Achievement newAchievement = Achievement(gameID, achID, achName, pts);
	achievementList.push_back(newAchievement);
	getGameByID(gameID)->addToAchievementNumber();	//add to total number of game achievements
	return newAchievement;
}

//two parts so that both the game and player have reference:
//add ign and game id to player
//add ign and player id to game
Game* Database::playerPlays(int playerID, int gameID, string ign)
{
	//add ign and game id to player
	Player* thisPlayer = getPlayerByID(playerID);
	try{
		thisPlayer->addPlays(gameID, ign);
	}
	catch (...) //if player doesn't exist
	{
		cout << "Player at id " + to_string(playerID) + " doesn't exist.\n";
	}	

	//add ign and player id to game
	Game* thisGame = getGameByID(gameID);
	try{
		thisGame->addPlays(playerID, ign);
	}
	catch (...) //if game doesn't exist
	{
		cout << "Game at id " + to_string(gameID) + " doesn't exist.\n";
	}

	return thisGame;
}

void Database::makeFriends(int firstID, int secondID)
{
	//add ign and game id to player
	Player* firstPlayer = getPlayerByID(firstID);
	Player* secondPlayer = getPlayerByID(secondID);

	firstPlayer->addFriend(secondID);
	secondPlayer->addFriend(firstID);

	cout << "Friend list for " + firstPlayer->getName() + ":\n-----------\n";
	firstPlayer->printFriends();
	cout << "\nFriend list for " + secondPlayer->getName() + ":\n-----------\n";
	secondPlayer->printFriends();
}

void Database::achieved(int playerID, int gameID, int achievementID)
{
	//add id of achievement to player list
	Player* thisPlayer = getPlayerByID(playerID);
	/*Game* thisGame = getGameByID(gameID);
	Achievement* thisAchievement*/

	thisPlayer->addAchievement(gameID, achievementID);
	thisPlayer->printAchievements();
}

void Database::printFriendsWhoPlay(int playerID, int gameID)
{
	Player* thisPlayer = getPlayerByID(playerID);
	Game* thisGame = getGameByID(gameID);
	vector<int> friendList = thisPlayer->getFriendList();
	vector<pair<string, int>> gameIGNList = thisGame->getIGNList();
	cout << "Friends of " + thisPlayer->getName() + " who play " + thisGame->getName() + "\n--------------\n";

	//go through friend list
	for (int i = 0; i < friendList.size(); i++)
	{
		int currentFriendID = friendList.at(i);
		//go through the game's playerlist to see if the current friend plays it
		for (int j = 0; j < gameIGNList.size(); j++)
		{
			pair<string, int> thisIGN = gameIGNList.at(j);
			if (currentFriendID == thisIGN.second)
				cout << "Player " + getPlayerByID(currentFriendID)->getName() + " plays this game with IGN: " + thisIGN.first + "\n";
		}
	}
}

//for the game given by gameID, print achievement # and achievement pts for
//that game for each player
void Database::comparePlayers(int playerID1, int playerID2, int gameID)
{
	//number of achievements
	int totalAchievementNum = getGameByID(gameID)->getAchievementNumber();
	int player1NumCount = 0;
	int player2NumCount = 0;
	//points from achievements
	int player1PtCount = 0;
	int player2PtCount = 0;
	//players
	Player* firstPlayer = getPlayerByID(playerID1);
	Player* secondPlayer = getPlayerByID(playerID2);
	vector<pair<int, int>> p1Achievements = firstPlayer->getAchievementList();
	vector<pair<int, int>> p2Achievements = secondPlayer->getAchievementList();

	//tally first player
	for (int i = 0; i < p1Achievements.size(); i++)
	{
		//if achievement is for this game
		if (p1Achievements.at(i).first == gameID)
		{
			player1NumCount++;
			player1PtCount += getAchievementByID(p1Achievements.at(i).second)->getPoints();
		}
	}

	//tally second player
	for (int i = 0; i < p2Achievements.size(); i++)
	{
		//if achievement is for this game
		if (p2Achievements.at(i).first == gameID)
		{
			player2NumCount++;
			player2PtCount += getAchievementByID(p2Achievements.at(i).second)->getPoints();
		}
	}

	//output
	string player1Fraction = to_string(player1NumCount) + "/" + to_string(totalAchievementNum);
	string player2Fraction = to_string(player2NumCount) + "/" + to_string(totalAchievementNum);
	cout << "For the game " + getGameByID(gameID)->getName() + ":\n-------------------------------------------\n";
	cout << firstPlayer->getName() + ":\tAchievements: " + player1Fraction + "\tPoints (for this game): " + to_string(player1PtCount) + "\n";
	cout << secondPlayer->getName() + ":\tAchievements: " + player2Fraction + "\tPoints (for this game): " + to_string(player2PtCount) + "\n";
}

void Database::summarizePlayer(int playerID)
{
	Player* player = getPlayerByID(playerID);
	string name = player->getName();
	string gamerScore = to_string(player->getTotalGamerscore());
	vector<pair<string, int>> playsList = player->getPlaysList(); //string is ign, int is game id
	vector<int> friendList = player->getFriendList();

	//first part of printing
	cout << "\nPlayer: " + name + "\tTotal Gamerscore: " + gamerScore + " pts\n\n" +
		"Game name\tAchievements\tGamerscore\tIGN\n---------------------------------------------------\n";
	//for each game, print its summary for this player
	for (int i = 0; i < playsList.size(); i++)
		printPlayerGameSummary(i + 1, playerID, playsList.at(i).second, playsList.at(i).first);

	cout << "\nFriend name\t\tGamerscore\n---------------------------------------------------\n";
	//for each friend, print their summary (name and gamerscore)
	for (int i = 0; i < friendList.size(); i++)
		printPlayerFriendSummary(friendList.at(i));

}

//prints a line for the game when summarizing player
void Database::printPlayerGameSummary(int gameNum, int playerID, int gameID, string ign)
{
	Game* game = getGameByID(gameID);
	//first int is game id, second is achievement id
	vector<pair<int, int>> playerAchievements = getPlayerByID(playerID)->getAchievementList();
	string gameName = game->getName();
	string totalAchNumber = to_string(getGameByID(gameID)->getAchievementNumber());
	int achievementNumCount = 0;
	int achievementScore = 0;

	//tally achievements for this game
	for (int i = 0; i < playerAchievements.size(); i++)
	{
		//if achievement is for this game
		if (playerAchievements.at(i).first == gameID)
		{
			achievementNumCount++;
			achievementScore += getAchievementByID(playerAchievements.at(i).second)->getPoints();
		}
	}
	string scoreString = to_string(achievementScore);
	if (achievementScore == 0)
		scoreString = "N/A";
	else
		scoreString += " pts";

	string fraction = to_string(achievementNumCount) + "/" + totalAchNumber;
	cout << to_string(gameNum) + ". " + gameName + "\t" + fraction + "\t\t" + scoreString + "\t\t" + ign + "\n";
}

void Database::printPlayerFriendSummary(int friendID)
{
	Player* playerFriend = getPlayerByID(friendID);
	string name = playerFriend->getName();
	string gamerscore = to_string(playerFriend->getTotalGamerscore()) + " pts";
	cout << name + "\t" + gamerscore + "\n";
}

void Database::summarizeGame(int gameID)
{
	////string is ign, int is player id
	//vector<pair<string, int>> ignList = getGameByID(gameID)->getIGNList();
	//cout << "Players who have played this game:\nName\tIGN\n------------------------------\n";
	////print players who've played game, as well as their igns
	//for (int i = 0; i < ignList.size(); i++)
	//	cout << getPlayerByID(ignList.at(i).second)->getName() + "\t" + ignList.at(i).first + "\n";

	//for (int i = 0; i < achievementList.size(); i++)
}

void Database::achievementRanking()
{
	cout << "\nAchievement Ranking:\n------------------\n";
	//vector<Player> inorderList;
	vector<pair<int, string>> playersInOrder;
	for (int i = 0; i < playerList.size(); i++)
		playersInOrder.push_back(make_pair(playerList.at(i).getTotalGamerscore(), playerList.at(i).getName()));
	sort(playersInOrder.begin(), playersInOrder.end(), comparison);
	for (int i = 0; i < playerList.size(); i++)
		cout << playersInOrder.at(i).second + "\t" + to_string(playersInOrder.at(i).first) + " pts\n";

}

//used for sorting
bool Database::comparison(pair<int, string> i, pair<int, string> j)
{
	return (i.first>j.first);
}

//accessors

Player* Database::getPlayerByID(int id)
{
	Player* thisPlayer = &Player();
	for (int i = 0; i < playerList.size(); i++)
	{
		//if player match
		if ((playerList.at(i).getID()) == id && (id != 0))
		{
			thisPlayer = &playerList.at(i);
			break;
		}
	}
	return thisPlayer;
}

Game* Database::getGameByID(int id)
{
	Game* thisGame = &Game();
	for (int i = 0; i < gameList.size(); i++)
	{
		//if player match
		if ((gameList.at(i).getID()) == id && (id != 0))
		{
			thisGame = &gameList.at(i);
			break;
		}
	}
	return thisGame;
}

Achievement* Database::getAchievementByID(int id)
{
	Achievement* thisAchievement = &Achievement();
	for (int i = 0; i < achievementList.size(); i++)
	{
		//if player match
		if ((achievementList.at(i).getID()) == id && (id != 0))
		{
			thisAchievement = &achievementList.at(i);
			break;
		}
	}
	return thisAchievement;
}

void Database::printPlayers()
{
	cout << "Current player list:\n----------------\n";
	for (int i = 0; i < playerList.size(); i++)
	{
		cout << playerList.at(i).toString();
	}
	cout << "----------------\n";
}

void Database::printGames()
{
	cout << "Current game list:\n----------------\n";
	for (int i = 0; i < gameList.size(); i++)
	{
		cout << gameList.at(i).toString();
	}
	cout << "----------------\n";
}

void Database::printAchievements()
{
	cout << "Current achievement list:\n----------------\n";
	for (int i = 0; i < achievementList.size(); i++)
	{
		cout << achievementList.at(i).toString();
	}
	cout << "----------------\n";
}
