#pragma once

#include "I4GeometryBuffer.h"

namespace i4core
{
	class Camera;
}
using namespace i4core;

namespace i4graphics
{
	class LineMesh;

	class LineBatch
	{
	public:
		LineBatch(void);
		~LineBatch(void);

		bool	create(int count);

		void draw();

		void	add(const Vector3& p0, const Vector3& p1, const Vector4& color)
		{
			Vertex_Pos_Col v;
			v.pos = p0;
			v.color = color;
			vecSceneVertex.push_back(v);

			v.pos = p1;
			v.color = color;
			vecSceneVertex.push_back(v);
		}

	private:
		LineMesh*					mesh;
		vector<Vertex_Pos_Col>	vecSceneVertex;
		vector<Vertex_Pos_Col>	vecCulledVertex;
	};

}