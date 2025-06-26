#ifndef BOUNDING_CUBE_HPP
# define BOUNDING_CUBE_HPP

# include <gmath.hpp>

struct BoundingCube
{
	gm::Vec3f	center;
	gm::Vec3f	extents;

	gm::Vec3f	points[8];

	void	computePoints(void)
	{
		gm::Vec3f	xAxis = gm::Vec3f(extents.x, 0, 0);
		gm::Vec3f	yAxis = gm::Vec3f(0, extents.y, 0);
		gm::Vec3f	zAxis = gm::Vec3f(0, 0, extents.z);

		points[0] = center + xAxis + yAxis + zAxis;
		points[1] = center - xAxis + yAxis + zAxis;
		points[2] = center + xAxis - yAxis + zAxis;
		points[3] = center - xAxis - yAxis + zAxis;
		points[4] = center + xAxis + yAxis - zAxis;
		points[5] = center - xAxis + yAxis - zAxis;
		points[6] = center + xAxis - yAxis - zAxis;
		points[7] = center - xAxis - yAxis - zAxis;
	}
};

#endif
