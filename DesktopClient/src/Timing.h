#ifndef TIMING_H
#define TIMING_H

#include <vector>
#include <string>

class Timing {
	int hour = 12;
	int minute = 0;

	bool days[7] = {};
	bool active = false;

	int value = 0;

	inline static std::vector<std::string> nameMap = {};
public:
	std::string serializeDayState()
	{
		std::string result;

		for (int i = 0; i < 7; i++)
		{
			if (days[i])
			{
				result += 'T';
			}
			else
			{
				result += 'F';
			}		
		}

		return result;
	}

	void parseDayState(std::string stateString)
	{
		int length = stateString.length() < 7 ? stateString.length() : 7;

		for (int i = 0; i < length; i++)
		{
			if (stateString[i] == 'T')
			{
				days[i] = true;
			}
			else {
				days[i] = false;
			}
		}
	}

	inline static void setNameMap(std::vector<std::string> nameMap) { Timing::nameMap = nameMap; };

	friend class MainWindow;
};

#endif