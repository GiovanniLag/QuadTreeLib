# QuadTreeLib

A C++ Library for QuadTree with custom user defined crowdness criteria and data.
Also mesh generation using QuadTree and Delaunay Triangulation. (working on it now)

Evrything is in the "sim" namespace.

## QuadTree

### Creating a QuadTree
To create a QaudTree frist define a BoundingBox. example:
```[c++]
sim::BoundingBox boundary = sim::BoundingBox(sim::Point(0, 0), sim::Point(1000, 1000));
```

Then you can create the QuadTree. You can define a crowdness criteria for the QuadTree by creating a function such as:
```[c++]
template <typename uT, typename cT>
bool isCrowded_simple(Quadtree<uT, cT>* quadtree, int capacity)
{
        return quadtree->getPoints().size() > capacity;
}
```
(this is the default crowdness criteria by the way)
**NOTE**: if you don't need capacity in your crowdness criteria, you can just make it a "trash" variable, i.e. dont use it.


Then you can create the QuadTree:
```[c++]
sim::Quadtree<uT, cT> quadtree(BoundingBox boundary, int capacity, std::function<bool(Quadtree*, cT)> isCrowded, cT isCrowdedData, uT userData);
```
where:
- boundary: the boundary of the QuadTree
- capacity: the capacity of each node
- isCrowded: the crowdness criteria
- isCrowdedData: the data that will be passed to the crowdness criteria function
- userData: the data that will be passed to the user defined function

For other constructors see the **QuadTree.hpp** file.

### Inserting data
To insert data into the QuadTree use the **insert** function:
```[c++]
quadtree.insert(sim::Point(10, 10));
```
where the argument is the data you want to insert.

### Querying the QuadTree
To query the QuadTree use the **queryRange** function, passing a BoundigBox of the reagion to query:
```[c++]
sim::BoundingBox queryBox = sim::BoundingBox(sim::Point(0, 0), sim::Point(100, 100));
std::vector<sim::Point*> points = quadtree.queryRange(queryBox);
```
where the return value is a vector of pointers to the points in the QuadTree that are in the queryBox.

**NOTE**: I should change thinsg so that you can attach custom data to individual points, but for now i was more intrested in using QuadTree for mesh generation.

## Mesh Generation
**Now working on this**

Generate a mesh using QuadTree and Delaunay Triangulation.
Exploiting the fact that the QuadTree is a spatial data structure which already somewhat divides the points for us, we can use dividi et impera implementation of delaunay triangulation.

We procede by "projecting" the QuadTree, i.e. we go down the tree to the leafs while carrying the points with us. At the end we are left basically with only the leafs and all points placed in them.
We then check for points to close to the leafs vertex, and if so we replace such vertex with the point.
At this point we can perform Delaunay Triangulation on each set of points (points in the leaf + vertexes of the leaf).
To finish we just need to merge the triangulations of each leaf.