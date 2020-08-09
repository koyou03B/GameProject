#pragma once
#include <vector>
#include <string>
#include "Vector.h"
#include ".\LibrarySource\ModelResource.h"

namespace Source
{
	namespace Blend
	{
		struct  HalfBodyLayer
		{
			float weight;
			std::vector<std::vector<float>> halfBodyBone;

			HalfBodyLayer() : weight(0) {};
			~HalfBodyLayer() = default;

		};
	}
}