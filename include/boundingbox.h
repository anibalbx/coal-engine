
#ifndef __BOUNDING_BOX_H__
#define __BOUNDING_BOX_H__ 1
#include <vec3.hpp>
#include <vec4.hpp>
#include <vector>

namespace coalengine
{
	class BoundingBox
	{
	public:
		BoundingBox();
		virtual ~BoundingBox();

		BoundingBox(const BoundingBox& o);
		BoundingBox(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z);
		BoundingBox& operator=(const BoundingBox& o);

		BoundingBox(const BoundingBox&& o);
		BoundingBox& operator=(const BoundingBox&& o);


		// Max and minimum values of the planes that generates de bounding box
		float min_x;
		float max_x;
		float min_y;
		float max_y;
		float min_z;
		float max_z;

		// Corners of the Bounding Box
		std::vector<glm::vec4> corners;
	};
}


#endif

