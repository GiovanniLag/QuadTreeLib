#include "PointCloud.hpp"

// --- Single frame point cloud ---
void sim::readPointCloud(std::string path_to_file, sim::PointCloud* point_cloud)
{
    std::ifstream file(path_to_file, std::ios::binary);
    if (!file) 
    {
        throw std::runtime_error("Cannot open file");
    }
    // Read header
    int frames; // Expected to be one for static point cloud version
    file.read((char*)&frames, sizeof(int));
    if (frames != 1)
    {
        throw std::runtime_error("Expected 1 frame, found " + static_cast<char>(frames));
    }
    // Read number of points for each frame
    int nPoints;
    file.read((char*)&nPoints, sizeof(int));
    // Read points
    for (int i = 0; i < nPoints; i++)
    {
        double x, y;
        file.read((char*)&x, sizeof(double));
        file.read((char*)&y, sizeof(double));
        point_cloud->points.push_back(sim::Point(x, y));
    }

    // Close file
    file.close();
}

void sim::savePointCloud(std::string path_to_file, PointCloud point_cloud)
{
    // Open or create file
    std::ofstream file(path_to_file, std::ios::binary);
    if (!file)
    {
		throw std::runtime_error("Cannot open file");
	}
    // Write header
    int frames = 1; // Expected to be one for static point cloud version
    file.write((char*)&frames, sizeof(int));
    // Write number of points for each frame
    int nPoints = point_cloud.size();
    file.write((char*)&nPoints, sizeof(int));
    // Write points
    for (int i = 0; i < nPoints; i++)
    {
		double x = point_cloud[i].x;
		double y = point_cloud[i].y;
		file.write((char*)&x, sizeof(double));
		file.write((char*)&y, sizeof(double));
	}

    // Close file
    file.close();
}

// --- Temporal point cloud ---
void sim::readPointCloud(std::string path_to_file, sim::TemporalPointCloud* tpc)
{
    std::ifstream file(path_to_file, std::ios::binary);
    if (!file)
    {
		throw std::runtime_error("Cannot open file");
	}
    // Read header
    int N_of_frames;
    file.read((char*)&N_of_frames, sizeof(int));
    // Read frames
    int nPoints;
    sim::PointCloud frame_point_cloud;
    for (int i = 0; i < N_of_frames; i++)
    {
        // Read number of points for current frame
        file.read((char*)&nPoints, sizeof(int));
        // Read points
        for (int j = 0; j < nPoints; j++)
        {
			double x, y;
			file.read((char*)&x, sizeof(double));
			file.read((char*)&y, sizeof(double));
			frame_point_cloud.push_back(sim::Point(x, y));
		}
        tpc->push_back(frame_point_cloud);
    }
}

void sim::savePointCloud(std::string path_to_file, sim::TemporalPointCloud* tpc)
{
    	// Open or create file
	std::ofstream file(path_to_file, std::ios::binary);
    if (!file)
    {
		throw std::runtime_error("Cannot open file");
	}
	// Write header
	int N_of_frames = tpc->frameNumber();
	file.write((char*)&N_of_frames, sizeof(int));
	// Write frames
	int nPoints;
    for (int i = 0; i < N_of_frames; i++)
    {
		// Write number of points for current frame
		nPoints = tpc->at(i).size();
		file.write((char*)&nPoints, sizeof(int));
		// Write points
        for (int j = 0; j < nPoints; j++)
        {
			double x = tpc->at(i)[j].x;
			double y = tpc->at(i)[j].y;
			file.write((char*)&x, sizeof(double));
			file.write((char*)&y, sizeof(double));
		}
	}
	// Close file
	file.close();
}