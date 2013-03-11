#pragma once

#include "I4GeometryBuffer.h"

namespace i4core
{
	class I4Camera;
}
using namespace i4core;

namespace i4graphics
{
	class I4LineMesh;

	class I4LineBatch
	{
	public:
		I4LineBatch(void);
		~I4LineBatch(void);

		bool	create(int count);

		void draw();

		void	add(const I4Vector3& p0, const I4Vector3& p1, const I4Vector4& color)
		{
			I4Vertex_Pos_Col v;
			v.pos = p0;
			v.color = color;
			vecSceneVertex.push_back(v);

			v.pos = p1;
			v.color = color;
			vecSceneVertex.push_back(v);
		}

	private:
		I4LineMesh*					mesh;
		vector<I4Vertex_Pos_Col>	vecSceneVertex;
		vector<I4Vertex_Pos_Col>	vecCulledVertex;
	};

}