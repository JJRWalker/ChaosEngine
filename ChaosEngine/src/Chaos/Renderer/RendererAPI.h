#pragma once
namespace Chaos
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, Vulkan = 1
		};

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}