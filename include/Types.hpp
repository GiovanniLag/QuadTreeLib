#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <memory>

namespace sim
{
    // --- FOR QUADTREE ---
    typedef struct Point
    {
        double x;
        double y;
        Point(double x, double y) : x(x), y(y) {}

        // Methods
        double squareDistance(Point other)
        {
            return pow(x - other.x, 2) + pow(y - other.y, 2);
        }
        double squareDistance(Point* other)
        {
			return pow(x - other->x, 2) + pow(y - other->y, 2);
		}
        double distance(Point other)
        {
			return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
		}
        double distance(Point* other)
        {
            return sqrt(pow(x - other->x, 2) + pow(y - other->y, 2));
        }

        // Operators
        bool operator==(const Point& other) const
        {
			return (x == other.x && y == other.y);
		}
    }Point;

    typedef struct BoundingBox
    {
        Point topLeft;
        Point bottomRight;
        BoundingBox(Point topLeft, Point bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {}
        // Methods
        // Check if a point is inside this bounding box
        bool contains(Point pt)
        {
            return (pt.x >= topLeft.x && pt.x <= bottomRight.x && pt.y >= topLeft.y && pt.y <= bottomRight.y);
        }
        // Check if two bounding boxes intersect
        bool intersects(BoundingBox other)
        {
			if (other.topLeft.x > bottomRight.x || other.bottomRight.x < topLeft.x)
				return false;
			if (other.topLeft.y > bottomRight.y || other.bottomRight.y < topLeft.y)
				return false;
			return true;
		}
        // Getters
        double getWidth() const { return bottomRight.x - topLeft.x; }
        double getHeight() const { return bottomRight.y - topLeft.y; }
    } Bounding;

    // --- FOR POINT CLOUD ---
    // Point cloud is just a simple collection (vector) of points
    typedef struct PointCloud
    {
        std::vector<Point> points;

        // Constructors
        PointCloud(std::vector<Point> points) : points(points) {}
        PointCloud() {} // Empty constructor
        // Getters
        int size() const { return points.size(); }
        Point operator[](int i) const { return points[i]; }
        Point at(int i) const { return points.at(i); }
        // Setters
        void push_back(Point pt) { points.push_back(pt); }

    } PointCloud;

    typedef struct TemporalPointCloud
    {
        std::vector<PointCloud> frames;

        // Constructors
        TemporalPointCloud(std::vector<PointCloud> frames) : frames(frames) {}
        TemporalPointCloud() {} // Empty constructor
        // Getters
        int frameNumber() const { return frames.size(); }
        PointCloud operator[](int i) const { return frames[i]; }
        PointCloud at(int i) const { return frames.at(i); }
        // Setters
        void push_back(PointCloud pc) { frames.push_back(pc); }
    };

    // --- FOR MESH ---
    typedef struct Link
    {
        std::shared_ptr<Point> p1;
        std::shared_ptr<Point> p2;

        Link(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2) : p1(p1), p2(p2) {}
    }Link;

    typedef struct Mesh
    {
        std::vector<std::shared_ptr<Point>> points;
        std::vector<Link> links;

        Mesh(std::vector<std::shared_ptr<Point>> points, std::vector<Link> links) : points(points), links(links) {}
        Mesh() {}
        int size() const { return points.size(); }
        void addPoint(std::shared_ptr<Point> pt) { points.push_back(pt); }
        void addLink(Link link) { links.push_back(link); }
        void addLink(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2) { links.push_back(Link(p1, p2)); }
    }Mesh;

    /* (p1, p2, p3) and (*a1, *a2, *a3) where *ai are pointers to adjeson triangles
     *ai would rappresent the triangle adjacent to the opposite side respect to vertex pi
     if there is no adjacent triangle, *ai then points to frist triangle encountered going counter-clockwise
     from the opposite side respect to vertex pi*/
    typedef struct Triangle
    {
		std::shared_ptr<Point> p1;
		std::shared_ptr<Point> p2;
		std::shared_ptr<Point> p3;

        std::shared_ptr<Triangle> a1;
        std::shared_ptr<Triangle> a2;
        std::shared_ptr<Triangle> a3;

        Triangle(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2, std::shared_ptr<Point> p3) : p1(p1), p2(p2), p3(p3) {}
        Triangle() {}

    }Triangle;

}

#endif // TYPES_HPP
