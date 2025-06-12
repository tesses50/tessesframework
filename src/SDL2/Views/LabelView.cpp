#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/LabelView.hpp"
namespace Tesses::Framework::SDL2::Views
{
    void LabelView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        auto win =this->GetWindow();
        win->normal_font->Render(renderer,r.x+4,r.y+4,text,win->palette.accent);
    }

    LabelView::LabelView() : View()
    {
        this->flags &= ~VIEWFLAG_TABSTOP;
    }
    LabelView::LabelView(std::string text) : View(text)
    {
        this->flags &= ~VIEWFLAG_TABSTOP;
    }
}
#endif