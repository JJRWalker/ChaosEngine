#pragma once

namespace Chaos
{
	class Image
	{

	public:
		Image() {};

		virtual ~Image() {};
		virtual void CreateImage(const char* filePath) = 0;
	};
}