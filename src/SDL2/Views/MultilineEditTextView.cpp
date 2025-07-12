#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/MultilineEditTextView.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"

namespace Tesses::Framework::SDL2::Views
{
    static int numberWidth(size_t n)
    {
        if(n == 0) return 1;
        size_t digits = 0;
        while(n != 0)
        {
            n /= 10;
            digits++;
        }
        return digits;
    }

    void MultilineEditTextView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        //1   |
        
        // 9   |
        //10   |

        auto win = this->GetWindow();
        auto w = win->monospaced_font->MaxWidth();
        auto h = win->monospaced_font->MaxHeight();
        auto digitsTotal = numberWidth(this->lines.size());
        
        auto numLines = (r.h - (win->palette.borderSize*4)) / h;

        if(numLines+this->topLeft.y > this->lines.size()) numLines = this->lines.size() - this->topLeft.y;

        for(size_t i = 0; i < numLines; i++)
        {
            win->monospaced_font->Render(renderer, r.x+(win->palette.borderSize*2),r.y+(h*i), Http::HttpUtils::LeftPad(std::to_string(i+this->topLeft.y+1),digitsTotal,' '),win->palette.accent);
            win->monospaced_font->Render(renderer,r.x+(win->palette.borderSize*2)+(w*(digitsTotal+5)),r.y+(h*i),this->lines[i+this->topLeft.y],win->palette.accent,this->topLeft.x);
        }

    }
    bool MultilineEditTextView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
        if(event.type == SDL_TEXTINPUT)
        {
            TypeText(event.text.text);
        }
        else if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_RETURN:
                    TypeText("\n");
                    break;
            }
        }
        return false;
    }
    MultilineEditTextView::MultilineEditTextView() :MultilineEditTextView(std::string()) 
    {
       
    }
    MultilineEditTextView::MultilineEditTextView(std::string hint) : View()
    {
        this->hint = hint;
    }
    std::string MultilineEditTextView::GetHint()
    {
        return this->hint;
    }
    void MultilineEditTextView::SetHint(std::string hint)
    {
        this->hint = hint;
    }
    std::string MultilineEditTextView::GetText()
    {
        if(this->lines.empty()) return {};
        std::string text = this->lines.front();
        for(size_t i = 1; i < this->lines.size(); i++)
        {
            text.push_back('\n');
            text.append(this->lines[i]);
        }
        return text;
    }
    void MultilineEditTextView::SetText(std::string text)
    {
        lines.clear();
        std::string line = "";
        for(auto c : text)
        {
            if(c == '\n') {
                lines.push_back(line);
                line = "";
            }
            else {
                line += c;
            }
        }
        if(!line.empty()) lines.push_back(line);

        this->cursorPos.x=0;
        this->cursorPos.y=0;
        this->cursorEnd.x=-1;
        this->cursorEnd.y=-1;
    }
    void MultilineEditTextView::TypeText(std::string text)
    {

        SDL_Point cursorBegin = this->cursorPos;
        SDL_Point cursorEnd = this->cursorEnd;
        if((cursorBegin.y > cursorEnd.y || ((cursorBegin.y == cursorEnd.y) && (cursorBegin.x > cursorEnd.x))) && this->cursorEnd.x != -1 && this->cursorEnd.y != -1)
        {
            cursorBegin.y ^= cursorEnd.y;
            cursorEnd.y ^= cursorBegin.y;
            cursorBegin.y ^= cursorEnd.y;

            cursorBegin.x ^= cursorEnd.x;
            cursorEnd.x ^= cursorBegin.x;
            cursorBegin.x ^= cursorEnd.x;
        }
        /*

        if(cursorEnd != std::string::npos && cursorEnd <= this->text.size())
        {
            this->text.erase(cursorBegin,cursorEnd-cursorBegin);
            this->text.insert(cursorBegin,text);
        }
        else if(cursorBegin <= this->text.size()) {
            this->text.insert(cursorBegin,text);
            
        }

        this->cursorPos = cursorBegin+text.size();
        this->cursorEnd = std::string::npos;*/

        if(this->cursorEnd.y != -1 && this->cursorEnd.x != -1)
        {
            int line = cursorBegin.y;
            for(int y = cursorBegin.y; y <= cursorEnd.y && y < lines.size(); y++)
            {
                if(y == cursorBegin.y && y == cursorEnd.y && cursorBegin.x == 0 && cursorEnd.x == this->lines[y].size()-1)
                {
                    this->lines.erase(this->lines.begin()+line);
                }
                else if(y == cursorBegin.y && y == cursorEnd.y)
                {
                    this->lines[line]=this->lines[line].substr(0,cursorBegin.x) + this->lines[line].substr(cursorEnd.x);
                    line++;
                }
                else if(y == cursorBegin.y && cursorBegin.x == 0)
                {
                    this->lines.erase(this->lines.begin()+line);
                }
                else if(y == cursorBegin.y)
                {
                    this->lines[line]=this->lines[line].substr(0,cursorBegin.x);
                    line++;
                }
                else if(y > cursorBegin.y && y < cursorEnd.y)
                {
                    this->lines.erase(this->lines.begin()+line);
                }
                else if(y == cursorEnd.y && cursorEnd.x < this->lines[line].size()-1)
                {
                    this->lines[line]=this->lines[line].substr(cursorBegin.x);
                }
                else {
                    this->lines.erase(this->lines.begin()+line);
                }
            }
        }

        
        
        
        auto mylines = text == "\n" ? std::vector<std::string>({"",""}) : Http::HttpUtils::SplitString(text,"\n");

        if(!mylines.empty())
        {
            int setXTo = 0;
            if(mylines.size()==1)
            {
                setXTo=mylines[0].size()+cursorBegin.x;
            }
            else {
                setXTo=mylines.back().size();
            }
            if(cursorBegin.y < this->lines.size())
            {
                
                
                if(cursorBegin.x > 0)
                {
                    mylines[0] = this->lines[cursorBegin.y].substr(0,cursorBegin.x) + mylines[0];
                }
                if(cursorBegin.x < this->lines[cursorBegin.y].size())
                {
                    mylines.back() += this->lines[cursorBegin.y].substr(cursorBegin.x);
                }
                
                

                this->lines.erase(this->lines.begin()+cursorBegin.y);
            }
            

            bool first=true;
            for(auto& item : mylines)
            {
                if(!first)
                {
                    cursorBegin.y++;
                }
                this->lines.insert(this->lines.begin()+cursorBegin.y,{item});
                first=false;
            }
            cursorBegin.x = setXTo;
            

        }

        
        this->cursorPos = cursorBegin;
        this->cursorEnd = {.x=-1,.y=-1};
    }
    std::pair<int,int> MultilineEditTextView::PreferedMinSize()
    {
        return std::pair<int,int>(128,128);
    }
}
#endif