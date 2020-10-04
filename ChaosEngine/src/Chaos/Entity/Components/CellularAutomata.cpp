#include "chaospch.h"
#include "CellularAutomata.h"
#include <random>
#include <time.h>

namespace Chaos
{
  //sizeX: number of nodes / cells in the x axis
  //sizeY: number of nodes / cells in the y axis
  //numOfPasses: how many times the function will itterate over the resurection / kill process
  //startingAliveChance: if random is above this value, start the node dead (100 = all alive) 
  //reviveThreshold: if the node has more than this number of neighbours alive, resurect it
  //deathThreshold: if the node has less than this number of neighbours alive, kill it
  std::vector<Node> CellularAutomata::Generate(int sizeX, int sizeY, int numOfPasses, int startingAliveChance, int reviveThreshold, int deathThreshold)
 {
	std::vector<Node> nodes;
	srand(time(NULL));	//seeding random with current time

	//create nodes
	for (int x = 0; x < sizeX; ++x)
	{
		for (int y = 0; y < sizeY; ++y)
		{
			Node node;
			node.X = x;
			node.Y = y;
			
			//if above the specified percentage value, start the node dead
			if ((rand() % 100) > startingAliveChance)
			{
				node.Alive = false; 
			}
			nodes.push_back(node);
		}
	}

	//link nodes
	for (auto& node : nodes)
	{
		for (auto& other : nodes)
		{
		  if(!(node.X == other.X && node.Y == other.Y))
			{
				if(std::abs(node.X - other.X) < 2 && std::abs(node.Y - other.Y) < 2)
				{
					node.Neighbours.push_back(&other);
				}
			}
		}		
	}

	for (int i = 0; i < numOfPasses; ++i)
	{
	//Check each node to see if it meets any critira for death / resurrection
		for (auto& node : nodes)
		{
			int numAlive = 0;
			for (auto* n : node.Neighbours)
			{
				if (n->Alive)
				{
					++numAlive;
				}
			}

			if (node.Alive && numAlive < deathThreshold)
			{
				node.Alive = false;
			}
			else if (!node.Alive && numAlive > reviveThreshold)
			{
				node.Alive = true;
			}
		}
	}
	return nodes;
 }
  
}
