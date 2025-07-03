
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/HStackView.hpp"

namespace Tesses::Framework::SDL2::Views
{
        void HStackView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
        {
            int numberOfCells = 0;
            int freeWidth = r.w;
            std::vector<int> sizes;
            sizes.resize(this->items.size());

            freeWidth -= (this->items.size() - 1) * this->spacing;

            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(this->items[i].first > 0)
                {
                    //static size
                    sizes[i] = this->items[i].first;
                    freeWidth-= this->items[i].first;
                }
                else if(this->items[i].first == 0)
                {
                    auto prefered = this->items[i].second.first->PreferedMinSize();
                    if(prefered.first > 0)
                    {
                        sizes[i] = prefered.first;
                        freeWidth -= prefered.first;
                    }
                    else {
                        sizes[i] = 0;
                    }
                }
                else {
                    numberOfCells -= this->items[i].first;
                }
            }
            

            int cellSize = numberOfCells == 0 ? 0 : freeWidth / numberOfCells;


            for(int i = 0; i < this->items.size();i++)
            {
                if(this->items[i].first < 0)
                {
                    int myWidth =((-(this->items[i].first)) * cellSize);

                    auto minSz = this->items[i].second.first->PreferedMinSize();
                    if(minSz.first > myWidth) { 
                        sizes[i] = minSz.first;
                        freeWidth-= minSz.first;
                        numberOfCells -= -(this->items[i].first);
                    }
                }
            }   
             cellSize = numberOfCells == 0 ? 0 : freeWidth/ numberOfCells;
            int x = 0;

            
            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(i > 0) x += spacing;

                if(sizes[i] == 0)
                {
                    int myWidth =((-(this->items[i].first)) * cellSize);
                    

                    SDL_Rect theirBounds = {
                        .x =x,
                        .y=0,
                        .w=myWidth,
                        .h=r.h
                    };


                    theirBounds.x += r.x;
                    theirBounds.y += r.y;

                    
                    
                    CallOnDraw(this->items[i].second.first,renderer, theirBounds);
                    x+=myWidth;
                }
                else {

                    SDL_Rect theirBounds = {
                        .x =x,
                        .y=0,
                        .w=sizes[i],
                        .h=r.h
                    };


                    theirBounds.x += r.x;
                    theirBounds.y += r.y;

                    
                    
                    CallOnDraw(this->items[i].second.first,renderer, theirBounds);
                    x+=sizes[i];
                }
                
            }
        }
        bool HStackView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
        {
            int numberOfCells = 0;
            int freeWidth = myBounds.w;
            std::vector<int> sizes;
            sizes.resize(this->items.size());


            freeWidth -= (this->items.size() - 1) * this->spacing;
            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(this->items[i].first > 0)
                {
                    //static size
                    sizes[i] = this->items[i].first;
                    freeWidth-= this->items[i].first;
                }
                else if(this->items[i].first == 0)
                {
                    auto prefered = this->items[i].second.first->PreferedMinSize();
                    if(prefered.first > 0)
                    {
                        sizes[i] = prefered.first;
                        freeWidth -= prefered.first;
                    }
                    else {
                        sizes[i] = 0;
                    }
                }
                else {
                    numberOfCells -= this->items[i].first;
                }
            }
            

            int cellSize = numberOfCells == 0 ? 0 : freeWidth/ numberOfCells;

            for(int i = 0; i < this->items.size();i++)
            {
                if(this->items[i].first < 0)
                {
                    int myWidth =((-(this->items[i].first)) * cellSize);

                    auto minSz = this->items[i].second.first->PreferedMinSize();
                    if(minSz.first > myWidth) { 
                        sizes[i] = minSz.first;
                        myWidth -= minSz.first;
                        numberOfCells -= -(this->items[i].first);
                    }
                }
            }   


            numberOfCells == 0 ? 0 : freeWidth/ numberOfCells;
            int x = 0;

            
            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(i > 0) x += spacing;
                if(sizes[i] == 0)
                {
                    int myWidth =((-(this->items[i].first)) * cellSize);

                    auto minSz = this->items[i].second.first->PreferedMinSize();
                    if(minSz.first > myWidth) myWidth = minSz.first;

                    SDL_Rect theirBounds = {
                        .x = x,
                        .y=0,
                        .w=myWidth,
                        .h=myBounds.h
                    };

                    SDL_Rect theirVisibleBounds = theirBounds;
                    theirVisibleBounds.x += visibleBounds.x;
                    theirVisibleBounds.y += visibleBounds.y;

                    theirBounds.x += myBounds.x;
                    theirBounds.y += myBounds.y;
                    Clipper::ClipRect(theirVisibleBounds, visibleBounds);

                    
                    
                    CallOnEvent(this->items[i].second.first,event,theirBounds,theirVisibleBounds);
                    
                    x+=myWidth;
                }
                else {

                    SDL_Rect theirBounds = {
                        .x = x,
                        .y=0,
                        .w=sizes[i],
                        .h=myBounds.h
                    };

                    SDL_Rect theirVisibleBounds = theirBounds;
                    theirVisibleBounds.x += visibleBounds.x;
                    theirVisibleBounds.y += visibleBounds.y;

                    theirBounds.x += myBounds.x;
                    theirBounds.y += myBounds.y;
                    Clipper::ClipRect(theirVisibleBounds, visibleBounds);

                    
                    
                    CallOnEvent(this->items[i].second.first,event,theirBounds,theirVisibleBounds);
                    
                    x+=sizes[i];
                }
                
            }
            return false;
        }
        HStackView::HStackView() : ContainerView()
        {

        }

        void HStackView::Add(int sz, View* view, bool owns)
        {
            this->items.push_back(std::pair<int,std::pair<View*,bool>>(sz,std::pair<View*,bool>(view,owns)));
            this->AssignChildParentToThis(view);
        }
        void HStackView::Remove(View* view)
        {
            for(auto index = this->items.begin(); index < this->items.end(); index++)
            {
                if(index->second.first == view)
                {

                    if(index->second.first != nullptr && index->second.second)
                        delete view;
                    this->items.erase(index);
                    return;
                }
            }
        }   
        void HStackView::Clear()
        {
            for(auto& item : this->items)
                if(item.second.second && item.second.first != nullptr) delete item.second.first;
            items = {};
        }
            
        size_t HStackView::ViewCount()
        {
            return this->items.size();
        }
        View* HStackView::GetViewAt(size_t index)
        {
            return this->items.at(index).second.first;
        }

        HStackView::~HStackView()
        {
            for(auto& item : this->items)
                if(item.second.second && item.second.first != nullptr) delete item.second.first; 
        }


    
}
#endif