#include "TessesFramework/HiddenField.hpp"

namespace Tesses::Framework
{

HiddenFieldData::~HiddenFieldData()
{
    
}



HiddenField::HiddenField()
{
    this->ptr = nullptr;
}
HiddenField::HiddenField(HiddenFieldData* data)
{
    this->ptr = data;
}
void HiddenField::SetField(HiddenFieldData* data)
{
    if(this->ptr != nullptr) delete this->ptr;
    this->ptr = data;
}
HiddenField::~HiddenField()
{
    if(this->ptr != nullptr) delete this->ptr;
}
}