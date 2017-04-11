#include "SGraphItemInfo.h"

SGraphItemInfo::SGraphItemInfo()
{
    //Do Nothing
}

SGraphItemInfo::SGraphItemInfo(string type, float x, float y, string name)

{
    node_type = type;
    draw_x = x;
    draw_y = y;
    node_name = name;
}


