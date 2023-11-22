#ifndef MESHGENERATION_HPP
#define MESHGENERATION_HPP

#include "Types.hpp"
#include "Quadtree.hpp"
#include "PointCloud.hpp"
#include <queue>
#include <memory>

namespace sim
{
    template <typename uT, typename cT>
    Mesh generateMesh(sim::Quadtree<uT, cT>* quadtree)
    {
        // Get all leafs of the quadtree
        std::queue<sim::Quadtree<uT, cT>*> leafsQueue;
        quadtree->getLeafs(&leafsQueue);
        // Generate mesh
        sim::Mesh mesh;
        while (!leafsQueue.empty())
        {
            sim::Quadtree<uT, cT>* leaf = leafsQueue.front();
            leafsQueue.pop();

            // Create shared_ptr for the four corners of the leaf
            auto topLeft = std::make_shared<sim::Point>(leaf->getBoundary().topLeft.x, leaf->getBoundary().topLeft.y);
            auto topRight = std::make_shared<sim::Point>(leaf->getBoundary().bottomRight.x, leaf->getBoundary().topLeft.y);
            auto bottomLeft = std::make_shared<sim::Point>(leaf->getBoundary().topLeft.x, leaf->getBoundary().bottomRight.y);
            auto bottomRight = std::make_shared<sim::Point>(leaf->getBoundary().bottomRight.x, leaf->getBoundary().bottomRight.y);

            // Add the four corners to the mesh
            mesh.addPoint(topLeft);
            mesh.addPoint(topRight);
            mesh.addPoint(bottomLeft);
            mesh.addPoint(bottomRight);

            // Add the point in the leaf to the mesh if leaf has it
            if (leaf->getPoints().size() > 0)
            {
                auto leafPoint = std::make_shared<sim::Point>(leaf->getPoints()[0]);
                mesh.addPoint(leafPoint);
            }

            // Add the links between the four corners
            mesh.addLink(topLeft, topRight);
            mesh.addLink(topRight, bottomRight);
            mesh.addLink(bottomRight, bottomLeft);
            mesh.addLink(bottomLeft, topLeft);

            // Add the links between the point in the leaf and the four corners
            if (leaf->getPoints().size() > 0)
            {
                auto leafPoint = std::make_shared<sim::Point>(leaf->getPoints()[0]);
                mesh.addLink(topLeft, leafPoint);
                mesh.addLink(topRight, leafPoint);
                mesh.addLink(bottomRight, leafPoint);
                mesh.addLink(bottomLeft, leafPoint);
            }
            else // Add the links between the point in the middle of the leaf and the four corners
            {
				auto leafPoint = std::make_shared<sim::Point>((leaf->getBoundary().topLeft.x + leaf->getBoundary().bottomRight.x) / 2, (leaf->getBoundary().topLeft.y + leaf->getBoundary().bottomRight.y) / 2);
				mesh.addLink(topLeft, leafPoint);
				//mesh.addLink(topRight, leafPoint);
				mesh.addLink(bottomRight, leafPoint);
				//mesh.addLink(bottomLeft, leafPoint);
                mesh.addPoint(leafPoint);
            }
        }
        return mesh;
    }

    // Another test for mesh generation
    template <typename uT, typename cT>
    Mesh generateMesh2(Quadtree<uT, cT>* quadtree)
    {
        // Get all leafs of the quadtree
        std::queue<Quadtree<uT, cT>*> leafsQueue;
        quadtree->getLeafs(&leafsQueue);
        // Generate mesh
        Mesh mesh;
        // Find all points that "lies" in leaf node bounding-box
        while (!leafsQueue.empty())
        {
            Quadtree<uT, cT>* leaf = leafsQueue.front();
            leafsQueue.pop();

            std::vector<Point*> pointsInLeaf = quadtree->queryRange(leaf->getBoundary());
            // Connect points in leaf based on distance
            for (int i = 0; i < pointsInLeaf.size(); i++)
            {
                for (int j = i + 1; j < pointsInLeaf.size(); j++)
                {
                    if (pointsInLeaf[i]->distance(*pointsInLeaf[j]) < 0.5)
                    {
                        // make shared ptrs to points
                        auto point1 = std::make_shared<Point>(*pointsInLeaf[i]);
                        auto point2 = std::make_shared<Point>(*pointsInLeaf[j]);
						mesh.addLink(point1, point2);
					}
				}
			}
        }

        return mesh;
    }
}

#endif // MESHGENERATION_HPP
