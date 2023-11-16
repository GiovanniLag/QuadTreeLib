/*Functions to generate, read and save point clouds*/

/*PointCloud .pc file format structure (it's preatty useless but i wanted to try implement custom filetipe):
* 1. Header (int) - number of frames
* --- REPEATS FOR ALL FRAMES ---
* 2. Number of points (int) of frame
* -- Points (x, y, z) of i-th frame
*/

#ifndef POINTCLOUD_HPP
#define POINTCLOUD_HPP

#include "Types.hpp"
#include <string>
#include <fstream>
#include <iostream>

namespace sim
{
	void readPointCloud(std::string path_to_file, PointCloud* point_cloud); // Read point cloud from .pc file, not temporal PointCloud (only 1 frame)
	void savePointCloud(std::string path_to_file, PointCloud point_cloud); // Save point cloud to .pc file, not temporal PointCloud (only 1 frame)

	void readPointCloud(std::string path_to_file, TemporalPointCloud* temporal_point_cloud); // Read point cloud from .pc file, temporal PointCloud (multiple frames)
	void savePointCloud(std::string path_to_file, TemporalPointCloud* temporal_point_cloud); // Save point cloud to .pc file, temporal PointCloud (multiple frames)
}


#endif // POINTCLOUD_HPP