//
// Created by connor on 9/1/18.
//

#include "IsoBox.h"

namespace DsprGameServer
{
    IsoBox::IsoBox(unsigned int width, unsigned int height)
    {
        this->width = width;
        this->height = height;
        
        int startRow = height;
        startRow /= -2;
        int endRow = height + startRow;

        int startColumn = width;
        startColumn /= -2;
        int endColumn = width + startColumn;
        
        for (int row = startRow; row < endRow; row++)
        {
            for (int column = startColumn; column < endColumn; column++)
            {
                //column+1 == x+1 & y-1
                //row+1 == x+1 & y+1
                auto coordX = row + column;
                auto coordY = row - column;
                auto newCoord = new Point(coordX, coordY);
                this->coordList.push_back(newCoord);
            }
        }
    }

    IsoBox::~IsoBox()
    {
        for(auto coord : coordList)
            delete coord;
    }
}