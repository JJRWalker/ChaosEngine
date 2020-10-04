#include <vector>

namespace Chaos
{
	struct Node
	{
		int X = 0;
		int Y = 0;
		bool Alive = true;
		std::vector<Node*> Neighbours;
	};
	
	class CellularAutomata
	{
	public:
		CellularAutomata(){};
		std::vector<Node> Generate(int sizeX, int sizeY, int numOfPasses, int startingAliveChance, int reviveThreshold, int deathThreshold);

	};
}
