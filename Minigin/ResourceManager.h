#pragma once

namespace dae
{
	class Texture2D;
	class Font;
	class Renderer;

	class ResourceManager final
	{		
	public:
		ResourceManager() = default;

		void Init(std::string&& data, Renderer* pRenderer);

		Texture2D* CreateTexture(const std::string &file, const glm::vec2 &pos);
		void CreateTextTexture(const SDL_Color &color, const Font* pFont, const std::string &text, Texture2D* &pTexture, const glm::vec2 &pos);
		Font* LoadFont(const std::string& file, unsigned int size);

	private:
		Renderer* m_pRenderer{ nullptr };
		std::string m_DataPath;
	};

}
