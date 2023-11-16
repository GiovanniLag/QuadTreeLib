#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "Types.hpp"

#define M_PI 3.14159265358979323846

namespace sim {
    template <typename uT, typename cT> class Quadtree;

    template <typename uT, typename cT>
    bool isCrowded_simple(Quadtree<uT, cT>* quadtree, int capacity)
    {
        return quadtree->getPoints().size() > capacity;
    }

    template <typename uT, typename cT>
    bool BEG_isCrowded(Quadtree<uT, cT>* quadtree, int trash) //Criteria from Provably good mesh generation by Bern, Eppstein and Gilbert, But C2 was removed and C1 was modified
    {
        if (quadtree->getPoints().size() >= quadtree->getCapacity()) //C1 - Box b contains two (extended to capacity) points of X. 
        {
			return true;
		}
        //C3 - Box b contains a point of X and one of the extended neighbors of b is split. 
        std::vector<Quadtree<uT, cT>*> extended_neighbors = quadtree->getExtendedNeighbour();
        for(auto neighbor : extended_neighbors)
        {
            if (neighbor->isDivided() && neighbor->getPoints().size() > neighbor->getCapacity())
            {
				return true;
			}
		}
        // If no conditions are satisified return false (do not subdivide)
        return false;
    }


    // --- POINT GENERATION TESTS ---
    
    // Draw a circle with center (x0, y0) and radius r of points
    PointCloud generateCircle(double x0, double y0, double r, int points)
    {
		PointCloud pointCloud;
        for (int i = 0; i < points; i++)
        {
			double angle = 2 * M_PI * i / points;
			double x = x0 + r * cos(angle);
			double y = y0 + r * sin(angle);
			pointCloud.push_back(Point(x, y));
		}
		return pointCloud;
	}


} // namespace sim

#endif // UTILITY_HPP
