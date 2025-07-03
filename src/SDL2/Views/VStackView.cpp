
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/VStackView.hpp"

namespace Tesses::Framework::SDL2::Views
{
        void VStackView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
        {
            int numberOfCells = 0;
            int freeHeight = r.h;
            std::vector<int> sizes;
            sizes.resize(this->items.size());

            freeHeight -= (this->items.size() - 1) * this->spacing;

            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(this->items[i].first > 0)
                {
                    //static size
                    sizes[i] = this->items[i].first;
                    freeHeight-= this->items[i].first;
                }
                else if(this->items[i].first == 0)
                {
                    auto prefered = this->items[i].second.first->PreferedMinSize();
                    if(prefered.second > 0)
                    {
                        sizes[i] = prefered.second;
                        freeHeight -= prefered.second;
                    }
                    else {
                        sizes[i] = 0;
                    }
                }
                else {
                    numberOfCells -= this->items[i].first;
                }
            }
            

            int cellSize = numberOfCells == 0 ? 0 : freeHeight / numberOfCells;


            for(int i = 0; i < this->items.size();i++)
            {
                if(this->items[i].first < 0)
                {
                    int myHeight =((-(this->items[i].first)) * cellSize);

                    auto minSz = this->items[i].second.first->PreferedMinSize();
                    if(minSz.second > myHeight) { 
                        sizes[i] = minSz.second;
                        freeHeight -= minSz.second;
                        numberOfCells -= -(this->items[i].first);
                    }
                }
            }   
            cellSize = numberOfCells == 0 ? 0 : freeHeight / numberOfCells;
            int y = 0;

            
            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(i > 0) y += spacing;

                if(sizes[i] == 0)
                {
                    int myHeight =((-(this->items[i].first)) * cellSize);
                    

                    SDL_Rect theirBounds = {
                        .x = 0,
                        .y=y,
                        .w=r.w,
                        .h=myHeight
                    };


                    theirBounds.x += r.x;
                    theirBounds.y += r.y;

                    
                    
                    CallOnDraw(this->items[i].second.first,renderer, theirBounds);
                    y+=myHeight;
                }
                else {

                    SDL_Rect theirBounds = {
                        .x = 0,
                        .y=y,
                        .w=r.w,
                        .h=sizes[i]
                    };


                    theirBounds.x += r.x;
                    theirBounds.y += r.y;

                    
                    
                    CallOnDraw(this->items[i].second.first,renderer, theirBounds);
                    y+=sizes[i];
                }
                
            }
        }
        bool VStackView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
        {
            int numberOfCells = 0;
            int freeHeight = myBounds.h;
            std::vector<int> sizes;
            sizes.resize(this->items.size());


            freeHeight -= (this->items.size() - 1) * this->spacing;
            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(this->items[i].first > 0)
                {
                    //static size
                    sizes[i] = this->items[i].first;
                    freeHeight-= this->items[i].first;
                }
                else if(this->items[i].first == 0)
                {
                    auto prefered = this->items[i].second.first->PreferedMinSize();
                    if(prefered.second > 0)
                    {
                        sizes[i] = prefered.second;
                        freeHeight -= prefered.second;
                    }
                    else {
                        sizes[i] = 0;
                    }
                }
                else {
                    numberOfCells -= this->items[i].first;
                }
            }
            

            int cellSize = numberOfCells == 0 ? 0 : freeHeight / numberOfCells;

            for(int i = 0; i < this->items.size();i++)
            {
                if(this->items[i].first < 0)
                {
                    int myHeight =((-(this->items[i].first)) * cellSize);

                    auto minSz = this->items[i].second.first->PreferedMinSize();
                    if(minSz.second > myHeight) { 
                        sizes[i] = minSz.second;
                        freeHeight -= minSz.second;
                        numberOfCells -= -(this->items[i].first);
                    }
                }
            }   


            cellSize = numberOfCells == 0 ? 0 : freeHeight / numberOfCells;
            int y = 0;

            
            for(size_t i = 0; i < this->items.size(); i++)
            {
                if(i > 0) y += spacing;
                if(sizes[i] == 0)
                {
                    int myHeight =((-(this->items[i].first)) * cellSize);

                    SDL_Rect theirBounds = {
                        .x = 0,
                        .y=y,
                        .w=myBounds.w,
                        .h=myHeight
                    };

                    SDL_Rect theirVisibleBounds = theirBounds;
                    theirVisibleBounds.x += visibleBounds.x;
                    theirVisibleBounds.y += visibleBounds.y;

                    theirBounds.x += myBounds.x;
                    theirBounds.y += myBounds.y;
                    Clipper::ClipRect(theirVisibleBounds, visibleBounds);

                    
                    
                    CallOnEvent(this->items[i].second.first,event,theirBounds,theirVisibleBounds);
                    
                    y+=myHeight;
                }
                else {

                    SDL_Rect theirBounds = {
                        .x = 0,
                        .y=y,
                        .w=myBounds.w,
                        .h=sizes[i]
                    };

                    SDL_Rect theirVisibleBounds = theirBounds;
                    theirVisibleBounds.x += visibleBounds.x;
                    theirVisibleBounds.y += visibleBounds.y;

                    theirBounds.x += myBounds.x;
                    theirBounds.y += myBounds.y;
                    Clipper::ClipRect(theirVisibleBounds, visibleBounds);

                    
                    
                    CallOnEvent(this->items[i].second.first,event,theirBounds,theirVisibleBounds);
                    
                    y+=sizes[i];
                }
                
            }
            return false;
        }
        VStackView::VStackView() : ContainerView()
        {

        }

        void VStackView::Add(int sz, View* view, bool owns)
        {
            this->items.push_back(std::pair<int,std::pair<View*,bool>>(sz,std::pair<View*,bool>(view,owns)));
            this->AssignChildParentToThis(view);
        }
        void VStackView::Remove(View* view)
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
        void VStackView::Clear()
        {
            for(auto& item : this->items)
                if(item.second.second && item.second.first != nullptr) delete item.second.first;
            items = {};
        }
            
        size_t VStackView::ViewCount()
        {
            return this->items.size();
        }
        View* VStackView::GetViewAt(size_t index)
        {
            return this->items.at(index).second.first;
        }

        VStackView::~VStackView()
        {
            for(auto& item : this->items)
                if(item.second.second && item.second.first != nullptr) delete item.second.first; 
        }


    
}
#endif