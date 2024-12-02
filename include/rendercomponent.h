#ifndef __RENDER_COMPONENT_H__
#define __RENDER_COMPONENT_H__ 1

#include "boundingbox.h"
#include "geometry.h"


namespace coalengine 
{
	class RenderComponent 
	{
	public:
		RenderComponent();
		RenderComponent(const RenderComponent& o) = delete;
		RenderComponent(RenderComponent&& o);
		~RenderComponent();

		RenderComponent& operator= (const RenderComponent& o) = delete;
		RenderComponent& operator= (RenderComponent&& o);

		void SetMesh(uint32_t geometry);

		// Set the mesh to null.
    void QuitMesh();
		Geometry* GetMesh();

    bool b_throw_shadows;

		// Bounding Box that will be created based on the mesh shape
		BoundingBox box;

	private:
		Geometry* mesh_;
	};
}

#endif 

