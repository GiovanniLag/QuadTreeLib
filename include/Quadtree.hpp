#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#define ROOT 0
#define NORTHWEST 1
#define NORTHEAST 2
#define SOUTHWEST 3
#define SOUTHEAST 4

#include <vector>
#include <functional>
#include <queue>
#include <stack>
#include "Types.hpp"

namespace sim
{
    template <typename uT, typename cT> // uT is userData type while cT is the type of isCrowded function
    class Quadtree
    {
    private:
        BoundingBox boundary;
        int capacity;
        int depth;
        std::vector<Point> points;
        Quadtree *northWest;
        Quadtree *northEast;
        Quadtree *southWest;
        Quadtree *southEast;
        Quadtree *parent;
        int type; // 0 = root, 1 = northWest, 2 = northEast, 3 = southWest, 4 = southEast
        bool divided;
        std::function<bool(Quadtree *, cT)> isCrowded;
        cT isCrowdedData;
        uT userData;

        // Private methods
        Quadtree* backTraceNorthNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence);
        Quadtree* backTraceSouthNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence);
        Quadtree* backTraceEastNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence);
        Quadtree* backTraceWestNeighbour(Quadtree<uT, cT>* currentNode, std::stack<int>* sequence);
        Quadtree* getNorthNeighbour(std::stack<int>* sequence);
        Quadtree* getSouthNeighbour(std::stack<int>* sequence);
        Quadtree* getEastNeighbour(std::stack<int>* sequence);
        Quadtree* getWestNeighbour(std::stack<int>* sequence);

    public:
        Quadtree(BoundingBox boundary, int capacity); // Constructor that uses simple isCrowded function (see utility.hpp)
        Quadtree(BoundingBox boundary, int capacity, std::function<bool(Quadtree*, cT)> isCrowded, cT isCrowdedData);// Constructor that uses custom isCrowded function
        Quadtree(BoundingBox boundary, int capacity, std::function<bool(Quadtree *, cT)> isCrowded, cT isCrowdedData, Quadtree* parent, int type); // Principal constructor
        Quadtree(BoundingBox boundary, Quadtree* parent, int type); // Constructor used by subdivide function
        ~Quadtree();
        
        // Main methods
        void subdivide();
        bool insert(Point point);
        std::vector<Point> queryRange(BoundingBox range);
        void balance();
        void getLeafs(std::queue<Quadtree*>* leafsQueue); // Get all leafs of the quadtree, provide a queue to store them

        // Getters and setters
        BoundingBox getBoundary() const { return boundary; }
        std::vector<Point> getPoints() const { return points; }
        int getCapacity() const { return capacity; }
        Quadtree *getNorthWest() const { return northWest; }
        Quadtree *getNorthEast() const { return northEast; }
        Quadtree *getSouthWest() const { return southWest; }
        Quadtree *getSouthEast() const { return southEast; }
        Quadtree *getParent() const { return parent; }
        uT *getUserData() const { return userData; }
        std::function<bool(Quadtree *, cT)> getIsCrowdedFun() const { return isCrowded; }
        void setUserData(uT userData) { this->userData = userData; }
        bool isDivided() const { return divided; }
        int getDepth() const { return depth; }
        int getType() const { return type; }
        // Get neighbours, delegate work to private methods
        Quadtree* getNorthNeighbour();
        Quadtree* getSouthNeighbour();
        Quadtree* getEastNeighbour();
        Quadtree* getWestNeighbour();
        
    };
    
} // namespace sim

#include "Quadtree_impl.tpp"

#endif // QUADTREE_HPP
