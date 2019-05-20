#pragma once
#include "BaseComponent.h"
#include <SDL.h>

namespace dae
{
	class Texture2D;

	class TextureComponent final : public BaseComponent
	{
	public:
		TextureComponent(std::string&& fileName, bool isCentered = true, const SDL_Rect &sourceRect = SDL_Rect());

		TextureComponent(const TextureComponent &) = delete;
		TextureComponent(TextureComponent &&) = delete;
		TextureComponent & operator= (const TextureComponent &) = delete;
		TextureComponent & operator= (const TextureComponent &&) = delete;

	protected:
		void Initialize(const SceneContext &sceneContext) override;
		void Update(const SceneContext &) override {}
		void LateUpdate(const SceneContext &) override {}

		virtual ~TextureComponent();

	private:
		Texture2D* m_pTexture;
		std::string m_FileName;
		SDL_Rect m_SourceRect;
		bool m_IsCentered;
	};
}