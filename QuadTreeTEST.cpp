// QuadTreeTEST.cpp : Defines the entry point for the application.
//

#include "QuadTreeTEST.h"
#include "Quadtree.hpp"
#include "graphics.hpp"
#include "utility.hpp"
#include "PointCloud.hpp"
#include "MeshGeneration.hpp"

#define cwidth 1000
#define cheight 1000


int main()
{
    // PointCloud
    sim::PointCloud pc = sim::generateCircle(cwidth / 2, cheight / 2, 300, 400);

    // Test quadtree generation
    sim::BoundingBox boundary = sim::BoundingBox(sim::Point(0, 0), sim::Point(cwidth, cheight));
    // create a quadtree with capacity of 4 and simple isCrowded function
    sim::Quadtree<int, int> quadtree(boundary, 1);
    //sim::Quadtree<int, int> quadtree(boundary, 1);
    std::cout << "Quadtree generated" << std::endl;
    
    // Test quadtree insertion (inserts all points from point cloud)
    int N = pc.size();
    for (int i = 0; i < N; i++)
    {
        //sim::Point pt = sim::Point(rand() % cwidth, rand() % cheight);
        //pc.push_back(pt);
        quadtree.insert(pc[i]);
    }
    std::cout << "Quadtree populated" << std::endl;

    // balance the quadtree
    quadtree.balance();

    // Generate mesh
    sim::Mesh mesh;
    mesh = sim::generateMesh2(&quadtree);

    // Make quadtree image or live quadtree
    quadtreeLive(quadtree, mesh);
    return 0;
}
