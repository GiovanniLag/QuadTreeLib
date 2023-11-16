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
    }Point;

    typedef struct BoundingBox
    {
        Point topLeft;
        Point bottomRight;
        BoundingBox(Point topLeft, Point bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {}
        // Check if a point is inside this bounding box
        bool contains(Point pt)
        {
            return (pt.x >= topLeft.x && pt.x <= bottomRight.x && pt.y >= topLeft.y && pt.y <= bottomRight.y);
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

}

#endif // TYPES_HPP