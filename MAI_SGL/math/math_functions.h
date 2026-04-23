#include "../base.h"

namespace mai
{
	static float lerp(const float& v1, const float& v2, const float& weight)
	{
		return v2 * weight + (1.0f - weight) * v1;
	}

	static float lerp(const float& v1, const float& v2, const float& v3, const float& weight1, const float& weight2, const float& weight3)
	{
		return v1 * weight1 + v2 * weight2 + v3 * weight3;
	}

	static RGBA lerp(const RGBA& v1, const RGBA& v2, const float& weight)
	{
		RGBA result;
		result._R = static_cast<byte>(static_cast<float>(v2._R) * weight + (1.0f - weight) * static_cast<float>(v1._R));
		result._G = static_cast<byte>(static_cast<float>(v2._G) * weight + (1.0f - weight) * static_cast<float>(v1._G));
		result._B = static_cast<byte>(static_cast<float>(v2._B) * weight + (1.0f - weight) * static_cast<float>(v1._B));
		result._A = static_cast<byte>(static_cast<float>(v2._A) * weight + (1.0f - weight) * static_cast<float>(v1._A));

		return result;
	}
}