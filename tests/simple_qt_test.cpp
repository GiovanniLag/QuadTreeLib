// QuadTreeTEST.cpp : Defines the entry point for the application.
//

#include "QuadTreeTEST.h"
#include "Quadtree.hpp"
#include "graphics.hpp"

#define cwidth 1000
#define cheight 1000


int main()
{
    // Test quadtree generation
    sim::BoundingBox boundary = sim::BoundingBox(sim::Point(0, 0), sim::Point(cwidth, cheight));
    // create a quadtree with capacity of 4 and simple isCrowded function
    sim::Quadtree<int, int> quadtree(boundary, 4);
    std::cout << "Quadtree generated" << std::endl;

    // Test quadtree insertion
    int N = 1;
    for (int i = 0; i < N; i++)
    {
        sim::Point pt = sim::Point(rand() % cwidth, rand() % cheight);
        quadtree.insert(pt);
    }
    std::cout << "Quadtree populated" << std::endl;

    // Make quadtree image or live quadtree
    quadtreeLive(quadtree);
    return 0;
}
