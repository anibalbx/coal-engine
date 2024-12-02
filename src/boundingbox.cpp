#include "boundingbox.h"

coalengine::BoundingBox::BoundingBox()
{
	min_x = -0.5f;
	max_x = 0.5f;
	min_y = -0.5f;
	max_y = 0.5f;
	min_z = -0.5f;
	max_z = 0.5f;
}

coalengine::BoundingBox::~BoundingBox()
{
}

coalengine::BoundingBox::BoundingBox(const BoundingBox& o)
{
    min_x = o.min_x;
    max_x = o.max_x;
    min_y = o.min_y;
    max_y = o.max_y;
    min_z = o.min_z;
    max_z = o.max_z;
    corners = o.corners;
}

coalengine::BoundingBox::BoundingBox(float minx, float maxx, float miny, float maxy, float minz, float maxz)
{
  min_x = minx;
  max_x = maxx;
  min_y = miny;
  max_y = maxy;
  min_z = minz;
  max_z = maxz;
}

coalengine::BoundingBox& coalengine::BoundingBox::operator=(const BoundingBox& o)
{
    min_x = o.min_x;
    max_x = o.max_x;
    min_y = o.min_y;
    max_y = o.max_y;
    min_z = o.min_z;
    max_z = o.max_z;
    corners = o.corners;

    return *this;
}

coalengine::BoundingBox::BoundingBox(const BoundingBox&& o)
{
	min_x = o.min_x;
    max_x = o.max_x;
    min_y = o.min_y;
    max_y = o.max_y;
    min_z = o.min_z;
    max_z = o.max_z;
    corners = o.corners;
}

coalengine::BoundingBox& coalengine::BoundingBox::operator=(const BoundingBox&& o)
{
    min_x = o.min_x;
    max_x = o.max_x;
    min_y = o.min_y;
    max_y = o.max_y;
    min_z = o.min_z;
    max_z = o.max_z;
    corners = o.corners;

    return *this;
}
