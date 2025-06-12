#pragma once
#include "Component.h"
#include "Font.h"
#include "Texture2D.h"
#include <string>
#include <memory>

namespace dae
{
    class TextComponent : public Component
    {
    public:
        explicit TextComponent(GameObject* owner, std::shared_ptr<Font> font, const std::string& text);
        ~TextComponent() override;

        void SetText(const std::string& text);
        void Render() const override;

        // Add getters for debugging
        const std::string& GetText() const { return m_text; }
        bool HasValidTexture() const { return m_textTexture != nullptr; }

    private:
        void UpdateTexture();

        std::string m_text;
        std::shared_ptr<Font> m_font;
        std::shared_ptr<Texture2D> m_textTexture;
        mutable bool m_needsUpdate = true;
    };
}