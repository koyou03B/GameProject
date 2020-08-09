#include "ChunkyTriMesh.h"
#include "Vector.h"

namespace Source
{
	namespace TriMesh
	{
		struct BoundsItem
		{
			float bmin[2];
			float bmax[2];
			int num;
		};

		static int CompareItemX(const void* va, const void* vb)
		{
			const BoundsItem* a = (const BoundsItem*)va;
			const BoundsItem* b = (const BoundsItem*)vb;
			if (a->bmin[0] < b->bmin[0])
				return -1;
			if (a->bmin[0] > b->bmin[0])
				return 1;
			return 0;
		}

		static int CompareItemY(const void* va, const void* vb)
		{
			const BoundsItem* a = (const BoundsItem*)va;
			const BoundsItem* b = (const BoundsItem*)vb;
			if (a->bmin[1] < b->bmin[1])
				return -1;
			if (a->bmin[1] > b->bmin[1])
				return 1;
			return 0;
		}

		inline int LongestAxis(float x, float y)
		{
			return y > x ? 1 : 0;
		}

		static void CalcExtends(const BoundsItem* items,
			const int min, const int max,
			float* bmin, float* bmax)
		{
			bmin[0] = items[min].bmin[0];
			bmin[1] = items[min].bmin[1];

			bmax[0] = items[min].bmax[0];
			bmax[1] = items[min].bmax[1];

			for (int i = min + 1; i < max; ++i)
			{
				const BoundsItem& it = items[i];
				if (it.bmin[0] < bmin[0])
					bmin[0] = it.bmin[0];
				if (it.bmin[1] < bmin[1])
					bmin[1] = it.bmin[1];

				if (it.bmax[0] > bmax[0])
					bmax[0] = it.bmax[0];
				if (it.bmax[1] > bmax[1])
					bmax[1] = it.bmax[1];
			}
		}

		static void Subdivide(BoundsItem* items, int itemCount, int min, int max, int trisPerChunk,
			int& currentNode, ChunkyTriMeshNode* nodes, const int maxNodes,
			int& currentTri, int* outTris, const int* inTris)
		{
			int num = max - min;
			int current = currentNode;

			if (currentNode > maxNodes)
				return;

			ChunkyTriMeshNode& node = nodes[currentNode++];

			if (num <= trisPerChunk)
			{
				CalcExtends(items, min, max, node.bmin, node.bmax);

				node.num = currentTri;
				node.count = num;

				for (int i = min; i < max; ++i)
				{
					const int* src = &inTris[items[i].num * 3];
					int* dst = &outTris[currentTri * 3];
					++currentTri;
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];

				}
			}
			else
			{
				CalcExtends(items, min, max, node.bmin, node.bmax);
	
				int	axis = LongestAxis(node.bmax[0] - node.bmin[0],
					node.bmax[1] - node.bmin[1]);

				if (axis == 0)
				{
					::qsort(items + min, static_cast<size_t>(num), sizeof(BoundsItem), CompareItemX);

				}
				else if (axis == 1)
				{
					::qsort(items + min, static_cast<size_t>(num), sizeof(BoundsItem), CompareItemY);
				}

				int split = min + num / 2;

				// Left
				Subdivide(items, itemCount, min, split, trisPerChunk, currentNode, nodes, maxNodes, currentTri, outTris, inTris);
				// Right
				Subdivide(items, itemCount, split, max, trisPerChunk, currentNode, nodes, maxNodes, currentTri, outTris, inTris);

				int iescape = currentNode - current;
				// Negative index means escape.
				node.num = -iescape;

			}
		}

		bool ChunkyTriMesh::CreateChunkyTriMesh(const float* vertex, const u_int stride, 
			const u_int* indices, const size_t indicesCount, int trisPerChunk)
		{
			const size_t triangleCount = indicesCount / 3;

			int chunkCount = (static_cast<int>(triangleCount) + trisPerChunk - 1) / trisPerChunk;

			this->nodes = new ChunkyTriMeshNode[chunkCount * 4];
			this->triangles = new int[indicesCount];
			this->triangleCount = static_cast<int>(triangleCount);

			// Build tree
			BoundsItem* items = new BoundsItem[triangleCount];
			const uint8_t* p = reinterpret_cast<const uint8_t*>(vertex);

			for (size_t indexOfTriangle = 0; indexOfTriangle < triangleCount; ++indexOfTriangle)
			{
				const int c0 = 0;
				VECTOR2F p0 = VECTOR2F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[2]);

				BoundsItem& it = items[indexOfTriangle];
				it.num = static_cast<int>(indexOfTriangle);
				it.bmin[0] = it.bmax[0] = p0.x;
				it.bmin[1] = it.bmax[1] = p0.y;

				for (int j = 1; j < 3; ++j)
				{
					VECTOR2F pj = VECTOR2F(
						(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + j] * stride))[0],
						(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + j] * stride))[2]);

					if (pj.x < it.bmin[0])
						it.bmin[0] = pj.x;
					if (pj.y < it.bmin[1])
						it.bmin[1] = pj.y;

					if (pj.x > it.bmax[0])
						it.bmax[0] = pj.x;
					if (pj.y > it.bmax[1])
						it.bmax[1] = pj.y;
				}
			}
		

			int currentTri = 0;
			int currentNode = 0;

			int iTriangleCount = static_cast<int>(triangleCount);
			Subdivide(items, iTriangleCount, 0, iTriangleCount, trisPerChunk,
				currentNode, this->nodes, chunkCount * 4, currentTri,
				this->triangles, (reinterpret_cast<const int*>(indices)));


			this->nodeCount = currentNode;

			this->maxTrisPerChunk = 0;
			for (int i = 0; i < this->nodeCount; ++i)
			{
				ChunkyTriMeshNode& node = this->nodes[i];
				const bool isLeaf = node.num >= 0;
				if (!isLeaf) continue;
				if (node.count > this->maxTrisPerChunk)
					this->maxTrisPerChunk = node.count;
			}

			return true;
		}
	}
}