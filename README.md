<p align="center">
  <img src="https://github.com/GiovanniLag/QuadTreeLib/blob/master/imgs/QuadTree_example_image.png" width="650">
</p>


# QuadTreeLib

A C++ Library for QuadTree with balancing function and with custom user-defined crowdedness criteria and data.
Also, mesh generation using QuadTree and Delaunay Triangulation. (working on it now)

Everything is in the "sim" namespace.

## QuadTree

### Creating a QuadTree
To create a QaudTree first define a BoundingBox. example:
```[c++]
sim::BoundingBox boundary = sim::BoundingBox(sim::Point(0, 0), sim::Point(1000, 1000));
```

Then you can create the QuadTree. You can define crowdedness criteria for the QuadTree by creating a function such as:
```[c++]
template <typename uT, typename cT>
bool isCrowded_simple(Quadtree<uT, cT>* quadtree, int capacity)
{
        return quadtree->getPoints().size() > capacity;
}
```
(this is the default crowdedness criteria by the way)
**NOTE**: if you don't need capacity in your crowdness criteria, you can just make it a "trash" variable, i.e. don't use it.


Then you can create the QuadTree:
```[c++]
sim::Quadtree<uT, cT> quadtree(BoundingBox boundary, int capacity, std::function<bool(Quadtree*, cT)> isCrowded, cT isCrowdedData, uT userData);
```
where:
- boundary: the boundary of the QuadTree
- capacity: the capacity of each node
- isCrowded: the crowdedness criteria
- isCrowdedData: the data that will be passed to the crowdedness criteria function
- userData: the data that will be passed to the user-defined function

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

**NOTE**: I should change things so that you can attach custom data to individual points, but for now I am more interested in using QuadTree for mesh generation.

### Balance the QuadTree
A QuadTree is said to be balanced if every leaf has neighbours (nodes "graphically" adjacent to it) that are not more than 2 levels in depth apart from the depth of the leaf itself.
To balance a QuadTree use the **balance** method.
```[c++]
quadtree.balance();
```

## Mesh Generation
**Now working on this**

Generate a mesh using QuadTree and Delaunay Triangulation.
Exploiting the fact that the QuadTree is a spatial data structure which already somewhat divides the points for us, we can use dividi et impera implementation of Delaunay triangulation.

We proceed by "projecting" the QuadTree, i.e. we go down the tree to the leafs while carrying the points with us. At the end, we are left basically with only the leafs and all the points placed in them.
We then check for points to close to the leafs vertex, and if so we replace such vertex with the point.
At this point, we can perform Delaunay Triangulation on each set of points (points in the leaf + vertexes of the leaf).
To finish we just need to merge the triangulations of each leaf.
