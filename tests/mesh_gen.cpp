// QuadTreeTEST.cpp : Defines the entry point for the application.
//

#include "Quadtree.hpp"
#include "graphics.hpp"
#include "utility.hpp"
#include "Types.hpp"

#define cwidth 1000
#define cheight 1000


int main()
{
    // Test quadtree generation
    sim::BoundingBox boundary = sim::BoundingBox(sim::Point(0, 0), sim::Point(cwidth, cheight));
    // create a quadtree with capacity of 4 and simple isCrowded function
    int trash_data;
    sim::Quadtree<int, int> quadtree(boundary, 1);
    std::cout << "Quadtree generated" << std::endl;


    // 
    // Make quadtree image or live quadtree
    quadtreeLive(quadtree);
    return 0;
}
