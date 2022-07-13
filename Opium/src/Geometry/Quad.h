#pragma once

#include <Renderer/VertexArray.h>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <Geometry/Mesh.h>

namespace OP
{
	// A general purpose 
	class Quad : public Mesh
	{
	public:
		Quad();
		static Ref<Quad> Create();
	private:
		virtual void BuildVertices() override;
	};
}