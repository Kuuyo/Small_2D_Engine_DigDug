#pragma once
#include "Singleton.h"

struct SDL_Window;
struct SDL_Renderer;

namespace dae
{
	class Texture2D;
	class Renderer final
	{		
	public:
		Renderer(SDL_Window* window);		
		~Renderer();

		void Render(const std::vector<Texture2D*> &pTextures) const;

		SDL_Renderer* GetSDLRenderer() const { return m_pRenderer; }

	private:
		void RenderTexture(const Texture2D& texture) const;
		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;

		SDL_Renderer* m_pRenderer{ nullptr };
	};
}

