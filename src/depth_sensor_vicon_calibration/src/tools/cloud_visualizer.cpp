#include "cloud_visualizer.hpp"

#include "pcl/registration/registration.h"
#include "pcl/filters/voxel_grid.h"


#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"

namespace vis
{

using namespace std;
using namespace Eigen;
using namespace pcl;


CloudVisualizer::CloudVisualizer():pcl_visualizer_("cloudy")
{
	origin_ = Eigen::Matrix4f::Identity();
}
CloudVisualizer::~CloudVisualizer(){}



void CloudVisualizer::add_cloud(const pcl::PointCloud<pcl::PointXYZRGB> &point_cloud, const Eigen::Matrix3f R, const Eigen::Vector3f t)
{
	Eigen::Matrix4f H = Eigen::Matrix4f::Identity();

	H.topLeftCorner(3,3) = R;
	H.topRightCorner(3,1) = t;
	point_clouds_.push_back(point_cloud);
	poses_.push_back(H);

	if(point_clouds_.size() == 1)
		set_origin_to_mean();
}
void CloudVisualizer::add_cloud(const PointCloud<PointXYZ> &point_cloud, const Eigen::Matrix3f R, const Eigen::Vector3f t)
{
	pcl::PointCloud<pcl::PointXYZRGB> color_cloud;

	// Fill in the color_cloud data
	color_cloud.width    = point_cloud.points.size();
	color_cloud.height   = 1;
	color_cloud.is_dense = false;
	color_cloud.points.resize (color_cloud.width * color_cloud.height);


	for (size_t i = 0; i < color_cloud.points.size (); ++i)
	{
		color_cloud.points[i].x = point_cloud.points[i].x;
		color_cloud.points[i].y = point_cloud.points[i].y;
		color_cloud.points[i].z = point_cloud.points[i].z;

		color_cloud.points[i].r = 100;
		color_cloud.points[i].g = 50;
		color_cloud.points[i].b = 200;
	}

	add_cloud(color_cloud, R, t);
}


void CloudVisualizer::add_cloud(const std::vector<Eigen::Vector3d>  &points, const Eigen::Matrix3f R, const Eigen::Vector3f t)
{
	pcl::PointCloud<pcl::PointXYZRGB> point_cloud;


	// Fill in the point_cloud data
	point_cloud.width    = points.size();
	point_cloud.height   = 1;
	point_cloud.is_dense = false;
	point_cloud.points.resize (point_cloud.width * point_cloud.height);

	for (size_t i = 0; i < point_cloud.points.size (); ++i)
	{
		point_cloud.points[i].x = points[i](0);
		point_cloud.points[i].y = points[i](1);
		point_cloud.points[i].z = points[i](2);

		point_cloud.points[i].r = 100;
		point_cloud.points[i].g = 50;
		point_cloud.points[i].b = 200;
	}

	add_cloud(point_cloud, R, t);
}


void CloudVisualizer::add_cloud(const std::vector<Eigen::Vector3f>  &points, const Eigen::Matrix3f R, const Eigen::Vector3f t)
{
	pcl::PointCloud<pcl::PointXYZRGB> point_cloud;


	// Fill in the point_cloud data
	point_cloud.width    = points.size();
	point_cloud.height   = 1;
	point_cloud.is_dense = false;
	point_cloud.points.resize (point_cloud.width * point_cloud.height);

	for (size_t i = 0; i < point_cloud.points.size (); ++i)
	{
		point_cloud.points[i].x = points[i](0);
		point_cloud.points[i].y = points[i](1);
		point_cloud.points[i].z = points[i](2);

		point_cloud.points[i].r = 100;
		point_cloud.points[i].g = 50;
		point_cloud.points[i].b = 200;
	}

	add_cloud(point_cloud, R, t);
}
void CloudVisualizer::add_cloud(const std::vector<Eigen::Vector3i> &points, const Eigen::Matrix3f R, const Eigen::Vector3f t)
{
	pcl::PointCloud<pcl::PointXYZRGB> point_cloud;

	// Fill in the point_cloud data
	point_cloud.width    = points.size();
	point_cloud.height   = 1;
	point_cloud.is_dense = false;
	point_cloud.points.resize (point_cloud.width * point_cloud.height);

	for (size_t i = 0; i < point_cloud.points.size (); ++i)
	{
		point_cloud.points[i].x = points[i](0);
		point_cloud.points[i].y = points[i](1);
		point_cloud.points[i].z = points[i](2);

		point_cloud.points[i].r = 100;
		point_cloud.points[i].g = 50;
		point_cloud.points[i].b = 200;
	}

	add_cloud(point_cloud, R, t);
}





void CloudVisualizer::add_line(Eigen::Vector3f from, Eigen::Vector3f to)
{
	lines_.push_back(from);
	lines_.push_back(to);
}

//
//void CloudVisualizer::add_point(Eigen::Vector3f point, Eigen::Matrix4f transform_)
//{
//	points_.push_back(transform_.topLeftCorner(3,3)*point+transform_.topRightCorner(3,1));
//}


void CloudVisualizer::add_point(Eigen::Vector3d point, Eigen::Matrix4d transform_)
{
	points_.push_back((transform_.topLeftCorner(3,3)*point+transform_.topRightCorner(3,1)).cast<float>());
}

void CloudVisualizer::set_origin_to_mean()
{
	Eigen::Vector3f mean = Eigen::Vector3f::Zero();

	if(point_clouds_.size())
	{
		int n=0;

		for(unsigned int i = 0; i < point_clouds_[0].points.size(); i++)
		{
			float norm = point_clouds_[0].points[i].getVector3fMap().norm();
			if(norm == norm)
			{
				n++;
				mean += poses_[0].topLeftCorner(3,3) * point_clouds_[0].points[i].getVector3fMap() + poses_[0].topRightCorner(3,1);
			}
		}


		mean /= (float)n;
		origin_ = Eigen::Matrix4f::Identity();
		origin_.topRightCorner(3,1) = -mean;
	}

}


void CloudVisualizer::show(bool loop)
{
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

	// add point clouds ==========================================================================================
	for(unsigned int i = 0; i < point_clouds_.size(); i++)
	{
		pcl::transformPointCloud (point_clouds_[i], *transformed_cloud, origin_ * poses_[i]);
		// set color -------------------------------------------
		int r, g, b;
		if(i == 0) {r = 200; g = 50; b = 50;}
		else if (i == 1) {r = 50; g = 50; b = 200;}
		else {r = rand()%256; g = rand()%256; b = rand()%256;}

		// view point clouds ------------------------------------------------------------------------------------------------------------
		pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> single_color(transformed_cloud, r, g,b );
		pcl_visualizer_.addPointCloud(transformed_cloud, single_color, "point_cloud"+boost::lexical_cast<std::string>(i));

		pcl_visualizer_.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 6, "point_cloud"+boost::lexical_cast<std::string>(i));
	}
	pcl_visualizer_.setBackgroundColor (1, 1, 1);

	for(unsigned int i = 0; i < lines_.size(); i += 2)
	{
		pcl::PointXYZRGB a,b;
		a.x = lines_[i](0) + origin_(0,3);
		a.y = lines_[i](1) + origin_(1,3);
		a.z = lines_[i](2) + origin_(2,3);

		b.x = lines_[i+1](0) + origin_(0,3);
		b.y = lines_[i+1](1) + origin_(1,3);
		b.z = lines_[i+1](2) + origin_(2,3);

		pcl_visualizer_.addLine(a, b, "line" + boost::lexical_cast<std::string>(i));
	}

	for(unsigned int i = 0; i < points_.size(); i++)
	{
		pcl::PointXYZRGB a;
		a.x = points_[i](0) + origin_(0,3);
		a.y = points_[i](1) + origin_(1,3);
		a.z = points_[i](2) + origin_(2,3);
		pcl_visualizer_.addSphere (a, 0.005, 1, 0.5, 1, "point" + boost::lexical_cast<std::string>(i));
	}

	pcl_visualizer_.initCameraParameters ();
	if(loop)
		pcl_visualizer_.spin();
	else
		pcl_visualizer_.spinOnce();

}

void CloudVisualizer::reset()
{
	point_clouds_.clear();
	poses_.clear();
	lines_.clear();
	points_.clear();
	origin_ = Eigen::Matrix4f::Identity();
}
}

