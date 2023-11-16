namespace sim
{
    // Constructor for the Quadtree class with default isCrowded function
    template <typename uT, typename cT>
    Quadtree<uT, cT>::Quadtree(BoundingBox boundary, int capacity)
        : Quadtree(
            boundary,
            capacity,
            [](Quadtree<uT, cT>* quadtree, cT data) { return quadtree->points.size() >= quadtree->capacity; },
            cT(),
            nullptr,
            0)
    {
        // Delegate to the principal constructor
    }

    // Constructor for the Quadtree class with custom isCrowded function
    template <typename uT, typename cT>
    Quadtree<uT, cT>::Quadtree(BoundingBox boundary, int capacity, std::function<bool(Quadtree<uT, cT>*, cT)> isCrowded, cT isCrowdedData)
        : Quadtree(
            boundary,
            capacity,
            isCrowded,
            isCrowdedData,
            nullptr)
    {
		// Delegate to the principal constructor
	}

    // Principal constructor
    template <typename uT, typename cT>
    Quadtree<uT, cT>::Quadtree(BoundingBox boundary, int capacity, std::function<bool(Quadtree<uT, cT>*, cT)> isCrowded, cT isCrowdedData, Quadtree<uT, cT>* parent, int type) : boundary(boundary), capacity(capacity), divided(false), isCrowded(isCrowded), isCrowdedData(isCrowdedData), parent(parent), type(type)
    {
        points.reserve(capacity); // Reserve memory for the points vector
        northWest = nullptr;
        northEast = nullptr;
        southWest = nullptr;
        southEast = nullptr;
        // Set the depth of the Quadtree
        if (parent != nullptr)
        {
            depth = parent->depth + 1;
        }
        else
        {
			depth = 0;
        }
    }

    // Constructor with parent specified, used by subdivide function
    template <typename uT, typename cT>
    Quadtree<uT, cT>::Quadtree(BoundingBox boundary, Quadtree<uT, cT>* parent, int type)
        : Quadtree(
            boundary,
            parent->getCapacity(),
            parent->getIsCrowdedFun(),
            cT(),
            parent,
            type)
    {
        // Delegate to the "full" constructor
    }

    // Destructor for the Quadtree class
    template <typename uT, typename cT>
    Quadtree<uT, cT>::~Quadtree()
    {
        points.clear(); // Clear the points vector
        delete northWest;
        delete northEast;
        delete southWest;
        delete southEast;
        //delete parent;
    }

    // Subdivide the Quadtree into four smaller Quadtree objects
    template <typename uT, typename cT>
    void Quadtree<uT, cT>::subdivide()
    {
        // Check if the Quadtree is already divided, if so, return (should never happen, but better safe than sorry)
        if (divided)
        {
			return;
		}
        // Find the center of the boundary
        double xMid = (boundary.topLeft.x + boundary.bottomRight.x) / 2;
        double yMid = (boundary.topLeft.y + boundary.bottomRight.y) / 2;

        // Create the four smaller bounding boxes
        BoundingBox nw = BoundingBox(boundary.topLeft, Point(xMid, yMid));
        BoundingBox ne = BoundingBox(Point(xMid, boundary.topLeft.y), Point(boundary.bottomRight.x, yMid));
        BoundingBox sw = BoundingBox(Point(boundary.topLeft.x, yMid), Point(xMid, boundary.bottomRight.y));
        BoundingBox se = BoundingBox(Point(xMid, yMid), boundary.bottomRight);

        // Create the four smaller Quadtree objects
        northWest = new Quadtree(nw, this, NORTHWEST);
        northEast = new Quadtree(ne, this, NORTHEAST);
        southWest = new Quadtree(sw, this, SOUTHWEST);
        southEast = new Quadtree(se, this, SOUTHEAST);

        // Set the divided flag to true
        divided = true;
    }

    // Insert a point into the Quadtree (uses a recursive approach)
    template <typename uT, typename cT>
    bool Quadtree<uT, cT>::insert(Point pt)
    {
        // Ignore objects that do not belong in this quad tree
        if (!boundary.contains(pt))
        {
            return false;
        }

        // If is not crowded, add the point here
        if (!isCrowded(this, isCrowdedData))
        {
            points.push_back(pt);
            return true;
        }

        // Otherwise, subdivide and then add the point to whichever node will accept it
        if (!divided)
        {
            subdivide();
        }

        // We have to add the points contained in this quad array to the new quads if we want to keep them
        if (northWest->insert(pt))
        {
            return true;
        }
        if (northEast->insert(pt))
        {
            return true;
        }
        if (southWest->insert(pt))
        {
            return true;
        }
        if (southEast->insert(pt))
        {
            return true;
        }

        // Otherwise, the point cannot be inserted for some unknown reason (this should never happen)
        return false;
    }

    // Create list of leaf nodes
    template <typename uT, typename cT>
    void Quadtree<uT, cT>::getLeafs(std::queue<Quadtree*>* leafsQueue)
    {
        if (this->divided)
        {
            this->northWest->getLeafs(leafsQueue);
            this->northEast->getLeafs(leafsQueue);
            this->southWest->getLeafs(leafsQueue);
            this->southEast->getLeafs(leafsQueue);
        }
        else
        {
            leafsQueue->push(this);
        }
    }


    // Recursive functions to get node neighbours
    // Helper function to backtrace north
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::backTraceNorthNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence)
    {
        // Trace back the sequence of directions but with N -> S
        while (!sequence->empty() && currentNode->isDivided())  
        {
            int direction = sequence->top();
            sequence->pop();
            if (direction == NORTHWEST) { currentNode = currentNode->southWest; }
            else if (direction == NORTHEAST) { currentNode = currentNode->southEast; }
        }
        return currentNode;
	}
    // Helper function to backtrace south
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::backTraceSouthNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence)
    {
        // Trace back the sequence of directions but with S -> N
        while (!sequence->empty() && currentNode->isDivided())
        {
            int direction = sequence->top();
            sequence->pop();
            if (direction == SOUTHWEST) { currentNode = currentNode->northWest; }
			else if (direction == SOUTHEAST) { currentNode = currentNode->northEast; }
        }
        return currentNode;
    }
    // Helper function to backtrace east
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::backTraceEastNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence)
    {
		// Trace back the sequence of directions but with W -> E
        while (!sequence->empty() && currentNode->isDivided())
        {
			int direction = sequence->top();
			sequence->pop();
			if (direction == NORTHEAST) { currentNode = currentNode->northWest; }
			else if (direction == SOUTHEAST) { currentNode = currentNode->southWest; }
		}
		return currentNode;
	}
    // Helper function to backtrace west
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::backTraceWestNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence)
    {
		// Trace back the sequence of directions but with E -> W
        while (!sequence->empty() && currentNode->isDivided())
        {
			int direction = sequence->top();
			sequence->pop();
			if (direction == NORTHWEST) { currentNode = currentNode->northEast; }
			else if (direction == SOUTHWEST) { currentNode = currentNode->southEast; }
		}
		return currentNode;
	}
   
    // North
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getNorthNeighbour(std::stack<int>* sequence)
    {
        if (type == ROOT) { return nullptr; }
        else if (type == SOUTHWEST) { backTraceNorthNeighbour(parent->northWest, sequence); }
        else if (type == SOUTHEAST) { backTraceNorthNeighbour(parent->northEast, sequence); }
        else 
        {
            sequence->push(type);
            return parent->getNorthNeighbour(sequence); 
        }
	}
    // South
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getSouthNeighbour(std::stack<int>* sequence)
    {
        if (type == ROOT) { return nullptr; }
        else if (type == NORTHWEST) { backTraceSouthNeighbour(parent->southWest, sequence); }
		else if (type == NORTHEAST) { backTraceSouthNeighbour(parent->southEast, sequence); }
        else
        {
			sequence->push(type);
			return parent->getSouthNeighbour(sequence); 
		}
    }
    // West
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getWestNeighbour(std::stack<int>* sequence)
    {
        if (type == ROOT) { return nullptr; }
        else if (type == NORTHEAST) { backTraceWestNeighbour(parent->northWest, sequence); }
		else if (type == SOUTHEAST) { backTraceWestNeighbour(parent->southWest, sequence); }
		else 
        { 
            sequence->push(type);
            return parent->getWestNeighbour(sequence);
        }
    }
    // East
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getEastNeighbour(std::stack<int>* sequence)
    {
		if (type == ROOT) { return nullptr; }
		else if (type == NORTHWEST) { backTraceEastNeighbour(parent->northEast, sequence); }
		else if (type == SOUTHWEST) { backTraceEastNeighbour(parent->southEast, sequence); }
        else
        {
			sequence->push(type);
			return parent->getEastNeighbour(sequence);
		}
	}

    // Public get neighbours methods, delegate work to private methods
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getNorthNeighbour()
    {
		std::stack<int> sequence;
		return getNorthNeighbour(&sequence);
	}
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getSouthNeighbour()
    {
		std::stack<int> sequence;
		return getSouthNeighbour(&sequence);
	}
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getWestNeighbour()
    {
        std::stack<int> sequence;
        return getWestNeighbour(&sequence);
    }
    template <typename uT, typename cT>
    Quadtree<uT, cT>* Quadtree<uT, cT>::getEastNeighbour()
    {
        std::stack<int> sequence;
        return getEastNeighbour(&sequence);
    }

    // Balance function for a quadtree
    template <typename uT, typename cT>
    void Quadtree<uT, cT>::balance()
    {
        std::queue<Quadtree*> leafsToBalance;
        this->getLeafs(&leafsToBalance);

        while (!leafsToBalance.empty())
        {
            Quadtree* node = leafsToBalance.front();
            leafsToBalance.pop();

            // Get leaf's neighbours
            auto northNeighbour = node->getNorthNeighbour();
            auto southNeighbour = node->getSouthNeighbour();
            auto westNeighbour = node->getWestNeighbour();
            auto eastNeighbour = node->getEastNeighbour();

            auto checkAndSubdivide = [&](Quadtree* neighbour) {
                if (neighbour != nullptr && node->depth - neighbour->depth > 1)
                {
                    if (!neighbour->isDivided()) { neighbour->subdivide(); }
                    leafsToBalance.push(neighbour->northWest);
                    leafsToBalance.push(neighbour->northEast);
                    leafsToBalance.push(neighbour->southWest);
                    leafsToBalance.push(neighbour->southEast);
                }
            };

            checkAndSubdivide(northNeighbour);
            checkAndSubdivide(southNeighbour);
            checkAndSubdivide(westNeighbour);
            checkAndSubdivide(eastNeighbour);
        }
    }


    
}