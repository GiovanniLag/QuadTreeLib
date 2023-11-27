#ifndef MESHGENERATION_HPP
#define MESHGENERATION_HPP

#include "Types.hpp"
#include "Quadtree.hpp"
#include "PointCloud.hpp"
#include <queue>
#include <memory>

#define MIN_L_RATIO 0.05

namespace sim
{
    // Divide et impera Delaunay triangulation
    std::vector<Triangle> DEIDelaunay(std::vector<Point> points)
    {
        std::vector<Triangle> triangles;
		return triangles;
	}

    template <typename uT, typename cT>
    void projectQuadtree(Quadtree<uT, cT>* qt, std::vector<Quadtree<uT, cT>>* qt_projection)
    {
        // recursively go down the quadtree and add the leafs to the projection, while carrying the points
        if (qt->isDivided())
        {
            // Check if there is a point in the current node (should be only one)
            if (qt->getPoints().size() > 0)
            {
				// If there is a point, add it to the correct child
                Point point = qt->getPoints()[0];
                if (qt->getNorthWest()->getBoundary().contains(point))
                {
					qt->getNorthWest()->forceInsert(point);
				}
                else if (qt->getNorthEast()->getBoundary().contains(point))
                {
					qt->getNorthEast()->forceInsert(point);
				}
                else if (qt->getSouthWest()->getBoundary().contains(point))
                {
					qt->getSouthWest()->forceInsert(point);
				}
                else if (qt->getSouthEast()->getBoundary().contains(point))
                {
					qt->getSouthEast()->forceInsert(point);
				}
                else
                {
                    // Should never happen
					std::cout << "Error: point not in boundary" << std::endl;
                    exit(1);
				}
				
			}
			// Recursively go down the quadtree
			projectQuadtree(qt->getNorthWest(), qt_projection);
			projectQuadtree(qt->getNorthEast(), qt_projection);
			projectQuadtree(qt->getSouthWest(), qt_projection);
			projectQuadtree(qt->getSouthEast(), qt_projection);
		}
        else
        {
			qt_projection->push_back(*qt);
		}
    }

    // DELAUNAY TRIANGULATION
    // Generate a delaunay triangulation from a quadtree
    // Returns a vector of triangles
    template <typename uT, typename cT>
    std::vector<Triangle> generateDelaunay(Quadtree<uT, cT>* qt)
    {

        // Generate the projection of the quadtree; i.e. go down to the leafs while carrying the points
        std::vector<Quadtree<uT, cT>> qtProjection;
        projectQuadtree(qt, &qtProjection);

        // For each node of projection apply Delaunay triangulation
        for (auto node : qtProjection)
        {
            auto nodePoints = node.getPoints();
            if (nodePoints.size() > 0)
            {
				// If a point is to close to a corner then make the point the corner
                int northWest_closestIndex = -1;
                double northWest_closestDistance = std::numeric_limits<double>::max();
                int northEast_closestIndex = -1;
                double northEast_closestDistance = std::numeric_limits<double>::max();
                int southWest_closestIndex = -1;
                double southWest_closestDistance = std::numeric_limits<double>::max();
                int southEast_closestIndex = -1;
                double southEast_closestDistance = std::numeric_limits<double>::max();
                for (Point point : nodePoints)
                {
                    // Check the distance from each leaf vertex
                    double northWest_distance = point.distance(node.getBoundary().topLeft);
                    if (northWest_distance <= northWest_closestDistance) { northWest_closestDistance = northWest_distance;  }
                    
                    double northEast_distance = point.distance(Point(node.getBoundary().bottomRight.x, node.getBoundary().topLeft.y));
                    if (northEast_distance <= northEast_closestDistance) { northEast_closestDistance = northEast_distance;  }

                    double southWest_distance = point.distance(Point(node.getBoundary().topLeft.x, node.getBoundary().bottomRight.y));
                    if (southWest_distance <= southWest_closestDistance) { southWest_closestDistance = southWest_distance;  }

                    double southEast_distance = point.distance(node.getBoundary().bottomRight);
                    if (southEast_distance <= southEast_closestDistance) { southEast_closestDistance = southEast_distance;  }
				}
                // If the point is too close to a corner, make it the corner
                if (northWest_closestDistance < MIN_L_RATIO * node.getBoundary().getWidth())
                {
					nodePoints[northWest_closestIndex] = (Point(node.getBoundary().topLeft.x, node.getBoundary().topLeft.y));
				}
                if (northEast_closestDistance < MIN_L_RATIO * node.getBoundary().getWidth())
                {
					nodePoints[northEast_closestIndex] = (Point(node.getBoundary().bottomRight.x, node.getBoundary().topLeft.y));
				}
                if (southWest_closestDistance < MIN_L_RATIO * node.getBoundary().getWidth())
                {
					nodePoints[southWest_closestIndex] = (Point(node.getBoundary().topLeft.x, node.getBoundary().bottomRight.y));
				}
                if (southEast_closestDistance < MIN_L_RATIO * node.getBoundary().getWidth())
                {
					nodePoints[southEast_closestIndex] =(Point(node.getBoundary().bottomRight.x, node.getBoundary().bottomRight.y));
				}
				// Generate the delaunay triangulation
				std::vector<Triangle> delaunay = DEIDelaunay(nodePoints);

			}
		}

        return std::vector<Triangle>();
    }

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
        std::vector<Triangle> triangles = generateDelaunay(quadtree);
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
