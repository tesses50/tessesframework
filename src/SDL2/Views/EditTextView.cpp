#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/EditTextView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    EditTextView::EditTextView() : EditTextView(std::string())
    {
       
    }
    EditTextView::EditTextView(std::string hint) : View()
    {
        this->hint = hint;
        this->cursorPos=0;
        this->cursorEnd=std::string::npos;
    }
    void EditTextView::TypeText(std::string text)
    {
        size_t cursorBegin = this->cursorPos;
        size_t cursorEnd = this->cursorEnd;
        if(cursorBegin > cursorEnd)
        {
            cursorBegin ^= cursorEnd;
            cursorEnd ^= cursorBegin;
            cursorBegin ^= cursorEnd;
        }

        if(cursorEnd != std::string::npos && cursorEnd <= this->text.size())
        {
            this->text.erase(cursorBegin,cursorEnd-cursorBegin);
            this->text.insert(cursorBegin,text);
        }
        else if(cursorBegin <= this->text.size()) {
            this->text.insert(cursorBegin,text);
            
        }

        this->cursorPos = cursorBegin+text.size();
        this->cursorEnd = std::string::npos;
        
    }
    void EditTextView::SetText(std::string text)
    {
        View::SetText(text);
        this->cursorPos=0;
        this->cursorEnd = std::string::npos;
        
    }
    void EditTextView::SetHint(std::string hint)
    {
        this->hint = hint;
    }
    std::string EditTextView::GetHint()
    {
        return this->hint;
    }
    std::pair<int,int> EditTextView::PreferedMinSize()
    {
        int x=-1;
        int y=-1;
        auto win = this->GetWindow();
        if(win != nullptr)
        {   
            x=-1; 
            y=win->monospaced_font->MaxHeight()+(win->palette.borderSize*4); 
        
        }

        return std::pair<int,int>(x,y);
    }
    
    bool EditTextView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& myVisibleBounds)
    {
        if(this->GetViewFlag(VIEWFLAG_ISACTIVE))
        {
            if(event.type == SDL_TEXTINPUT)
            {
                if(this->GetViewFlag(VIEWFLAG_TOUCHED))
                {
                    this->SetViewFlag(VIEWFLAG_TOUCHED,false);
                    
                    if(!this->text.empty())
                    {
                        this->cursorPos=0;
                        this->cursorEnd=this->text.size();
                    }
                }
                TypeText(event.text.text);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDL_KeyCode::SDLK_c:
                    {
                        if((event.key.keysym.mod & KMOD_CTRL) != 0)
                        {
                            size_t cursorBegin = cursorPos;
                            size_t cursorEnd = this->cursorEnd;
                            if(cursorBegin > cursorEnd)
                            {
                                cursorBegin ^= cursorEnd;
                                cursorEnd ^= cursorBegin;
                                cursorBegin ^= cursorEnd;
                            }
                            
                            if(cursorEnd != std::string::npos && cursorEnd <= this->text.size() && this->text.size() > 0)
                            {
                                std::string res = this->text.substr(cursorBegin,cursorEnd-cursorBegin);
                                //SDL_GetClipboardText()
                                 SDL_SetClipboardText(res.c_str());
                            
                            }
                        }
                    }
                    break;
                    case SDL_KeyCode::SDLK_v:
                    {
                        if((event.key.keysym.mod & KMOD_CTRL) != 0)
                        {
                            if(SDL_HasClipboardText())
                            {
                                char* text = SDL_GetClipboardText();
                                if(text != NULL)
                                {   
                                    TypeText(text);
                                }
                                SDL_free(text);
                            }
                        }
                    }
                    break;
                    case SDL_KeyCode::SDLK_x:
                    {
                        if((event.key.keysym.mod & KMOD_CTRL) != 0)
                        {
                            size_t cursorBegin = cursorPos;
                            size_t cursorEnd = this->cursorEnd;
                            if(cursorBegin > cursorEnd)
                            {
                                cursorBegin ^= cursorEnd;
                                cursorEnd ^= cursorBegin;
                                cursorBegin ^= cursorEnd;
                            }
                            
                            if(cursorEnd != std::string::npos && cursorEnd <= this->text.size() && this->text.size() > 0)
                            {
                                std::string res = this->text.substr(cursorBegin,cursorEnd-cursorBegin);
                                //SDL_GetClipboardText()
                                 SDL_SetClipboardText(res.c_str());

                                 TypeText("");
                            
                            }
                        }
                    }
                    break;
                    case SDL_KeyCode::SDLK_a:
                    {
                        if((event.key.keysym.mod & KMOD_CTRL) != 0)
                        {
                            this->cursorPos=0;
                            this->cursorEnd=this->text.size();
                        }
                    }
                    break;
                    case SDL_KeyCode::SDLK_BACKSPACE:
                    {
                        if(this->cursorEnd == std::string::npos)
                        {
                            if(this->cursorPos > 0)
                            {
                                this->cursorPos--;
                                this->text.erase(this->cursorPos,1);
                                
                            }
                        }
                        else {
                            TypeText("");
                        }
                    }
                    break;
                    case SDL_KeyCode::SDLK_DELETE:
                    {
                        if(this->cursorEnd == std::string::npos)
                        {
                            if(this->cursorPos + 1 <= this->text.size())
                            {
                                this->text.erase(this->cursorPos,1);
                                
                            }
                        }
                        else {
                            TypeText("");
                        }
                    }
                    break;
                    case SDL_KeyCode::SDLK_LEFT:
                    {
                        if((event.key.keysym.mod & KMOD_SHIFT) != 0)
                        {
                            if(this->cursorEnd > this->text.size())
                                this->cursorEnd = this->cursorPos;
                            
                            this->cursorEnd--;

                            
                            if(this->cursorEnd > this->text.size()) this->cursorEnd=0;
                            if(this->cursorEnd == this->cursorPos) this->cursorEnd = std::string::npos;

                        }
                        else 
                        {
                            if(cursorPos > cursorEnd)
                            {
                                cursorPos ^= cursorEnd;
                                cursorEnd ^= cursorPos;
                                cursorPos ^= cursorEnd;
                            }
                            this->cursorPos--;
                            this->cursorEnd=std::string::npos;
                            if(this->cursorPos > this->text.size()) this->cursorPos=0;
                        }
                    }
                        break;
                    case SDL_KeyCode::SDLK_RIGHT:
                    if((event.key.keysym.mod & KMOD_SHIFT) != 0)
                        {
                            if(this->cursorEnd == std::string::npos)
                                this->cursorEnd = this->cursorPos;
                            
                            this->cursorEnd++;

                            
                            if(this->cursorEnd > this->text.size()) this->cursorEnd=this->text.size();
                            if(this->cursorEnd == this->cursorPos) this->cursorEnd = std::string::npos;

                        }
                        else 
                        {
                            if(cursorPos > cursorEnd)
                            {
                                cursorPos ^= cursorEnd;
                                cursorEnd ^= cursorPos;
                                cursorPos ^= cursorEnd;
                            }
                            if(cursorEnd != std::string::npos)
                            {
                                this->cursorPos=cursorEnd+1;

                            }else {
                                this->cursorPos++;
                            }
                            this->cursorEnd=std::string::npos;
                            if(this->cursorPos > this->text.size()) this->cursorPos=this->text.size();
                        }
                        break;
                }
            }
        }
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            if(event.button.x >= myVisibleBounds.x && event.button.x < myVisibleBounds.x+myVisibleBounds.w && event.button.y >= myVisibleBounds.y && event.button.y < myVisibleBounds.y+myVisibleBounds.h)
            {
                this->GetWindow()->MakeActive(this); 
                this->SetViewFlag(VIEWFLAG_TOUCHED,false);
                
                SDL_StartTextInput();
            }
        }
        if(event.type == SDL_FINGERDOWN)
        {
            auto pos = this->GetCordFromEvent(event);
            if(pos.first >= myVisibleBounds.x && pos.first < myVisibleBounds.x+myVisibleBounds.w && pos.second >= myVisibleBounds.y && pos.second < myVisibleBounds.y+myVisibleBounds.h)
            {
                this->GetWindow()->MakeActive(this); 
                
                this->SetViewFlag(VIEWFLAG_TOUCHED,true);
                SDL_StartTextInput();
            }
        }
        return View::OnEvent(event,myBounds,myVisibleBounds);
    }
    void EditTextView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        size_t cursorBegin = this->cursorPos;
        size_t cursorEnd = this->cursorEnd;
        if(cursorBegin > cursorEnd)
        {
            cursorBegin ^= cursorEnd;
            cursorEnd ^= cursorBegin;
            cursorBegin ^= cursorEnd;
        }
        auto win = this->GetWindow();
        //SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
        //SDL_RenderFillRect(renderer,&r);

        int textW=win->monospaced_font->MaxWidth()+2;
        int textH=win->monospaced_font->MaxHeight();

        int noChars = (r.w-(win->palette.borderSize*4)) / textW;
        

        int x=0;
        int y=(r.h/2)-((textH+(win->palette.borderSize*4))/2);
        x+=r.x+(win->palette.borderSize*2);
        y+=r.y+(win->palette.borderSize*2);

        auto isHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto isMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);

        SDL_Color& color = win->palette.GetBorderColor(isHovering,isActive,isMouseDown);

        size_t tracking = this->cursorPos;
        if(cursorEnd != std::string::npos)
            tracking = this->cursorEnd;


        size_t off = noChars == 0 ? (size_t)0 : ((tracking / (size_t)noChars) * (size_t)noChars);


        std::string&  datext = this->text.empty() ? this->hint : this->text;

        //printf("%i %i %i\n",(int) tracking,(int)off,(int)noChars);

        for(size_t i = 0; i < (size_t)noChars; i++)
        {
            if(cursorEnd == std::string::npos && (i+off) == cursorBegin)
            {
                SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
                int x1=((int)i*textW)+x;
                    int y1 = y;
                    int y2 = y+textH;
                SDL_RenderDrawLine(renderer,x1,y1,x1,y2);
            }
            if(i + off < datext.size())
            {
                if(cursorEnd != std::string::npos && (i+off) >= cursorBegin &&  (i+off) < cursorEnd)
                {
                    SDL_Rect r0={.x=((int)i*textW)+x,.y=y,.w=textW,.h=textH+8};
                    SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
                    SDL_RenderFillRect(renderer,&r0);
                    
                    auto ch=win->monospaced_font->GetCharOfColor(datext[i+off],color);
                    
                    SDL_Rect src={.x=0,.y=0,.w=0,.h=0};
                    SDL_QueryTexture(ch,NULL,NULL,&src.w,&src.h);
                    SDL_Rect dest = {.x=((int)i*textW)+x+1,.y=y,.w=src.w,.h=src.h};
                    SDL_RenderCopy(renderer,ch,&src,&dest);
                    
                }
                else {
                    

                    

                    auto ch=win->monospaced_font->GetCharOfColor(datext[i+off],this->text.empty() ? color : win->palette.accent);
                    
                    SDL_Rect src={.x=0,.y=0,.w=0,.h=0};
                    SDL_QueryTexture(ch,NULL,NULL,&src.w,&src.h);
                    SDL_Rect dest = {.x=((int)i*textW)+x+1,.y=y,.w=src.w,.h=src.h};
                    SDL_RenderCopy(renderer,ch,&src,&dest);
                }
            }
        }


        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

        SDL_Rect r2={.x=r.x,.y=y-(win->palette.borderSize*2),.w=r.w,.h=textH+16};
        for(size_t i=0;i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawRect(renderer,&r2);
            r2.x++;
            r2.y++;
            r2.w-=2;
            r2.h-=2;
        }
        
    
    }
    
}
#endif