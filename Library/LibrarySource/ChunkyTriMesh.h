#pragma once
#include <winsock.h>

namespace Source
{
	namespace TriMesh
	{
		struct ChunkyTriMeshNode
		{
			float bmin[2];
			float bmax[2];
			int num;
			int count;
		};

		struct ChunkyTriMesh
		{
		private:
			inline ChunkyTriMesh() : nodes(0), nodeCount(0), triangles(0), triangleCount(0), maxTrisPerChunk(0) {};
			inline ~ChunkyTriMesh() { delete[] nodes; delete[] triangles; }

			ChunkyTriMeshNode* nodes;
			int nodeCount;
			int* triangles;
			int triangleCount;
			int maxTrisPerChunk;

			bool CreateChunkyTriMesh(const float* vertex, const u_int stride, const u_int* indices, const size_t indicesCount, int trisPerChunk = 256);
		};
	}
}
