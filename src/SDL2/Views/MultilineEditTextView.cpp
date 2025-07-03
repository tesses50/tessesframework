#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/MultilineEditTextView.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"

namespace Tesses::Framework::SDL2::Views
{
    void MultilineEditTextView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        //1   |
        
        // 9   |
        //10   |

        

        
    }
    bool MultilineEditTextView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {

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
        if(cursorBegin.y > cursorEnd.y || ((cursorBegin.y == cursorEnd.y) && (cursorBegin.x > cursorEnd.x)))
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

        if(cursorEnd.y != -1 && cursorEnd.x != -1)
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

        
        
        
        auto mylines = Http::HttpUtils::SplitString(text,"\n");

        if(!mylines.empty())
        {
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

            

            for(auto& item : mylines)
            {
                this->lines.insert(this->lines.begin()+cursorBegin.y,{item});
            }

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