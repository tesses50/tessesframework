#pragma once

#include <functional>

namespace Tesses::Framework {
class HiddenFieldData {
    public:
        virtual ~HiddenFieldData();
};

class HiddenField {
    private:
    HiddenFieldData* ptr;
    public:
        HiddenField();
        HiddenField(HiddenFieldData* data);
        void SetField(HiddenFieldData* data);
        template<typename T>
        T GetField()
        {
            return dynamic_cast<T>(ptr);
        }
        
        ~HiddenField();
};
}