#pragma once
#include <string>

class Achievement
{
	//private
	int gameID, id, points;
	std::string name;
public:
	Achievement();
	~Achievement();
	Achievement(int gameID, int id, std::string name, int points);
	std::string toString();

	//public accessors
	int getID(){ return id; };
	int getPoints(){ return points; };
	int getGameID(){ return gameID; };
	std::string getName(){ return name; };
};

