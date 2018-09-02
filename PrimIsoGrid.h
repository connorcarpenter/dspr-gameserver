//
// Created by connor on 9/2/18.
//

#include <assert.h>
#include <functional>

namespace DsprGameServer {
    template<class T>
    class PrimIsoGrid
    {
    public:

        PrimIsoGrid() {};

        ~PrimIsoGrid()
        {
            delete[] this->gridArrayA;
            delete[] this->gridArrayB;
        }

        T* initialize(int width, int height)
        {
            assert(!this->initialized);
            assert(width % 2 == 0);
            assert(height % 2 == 0);

            this->initialized = true;
            this->width = width;
            this->height = height;
            this->halfWidth = width/2;
            this->halfHeight = height/2;

            this->gridArrayA = initGridArray(this->halfWidth, this->halfHeight);
            this->gridArrayB = initGridArray(this->halfWidth, this->halfHeight);
        }

        void set(int x, int y, T obj)
        {
            int gridIndex = getGridIndex(x, y);
            if (gridIndex == -1) return;
            int tileIndex = getTileIndex(gridIndex, x, y);
            if (gridIndex == 0)
            {
                this->gridArrayA[tileIndex] = obj;
            }
            else
            {
                this->gridArrayB[tileIndex] = obj;
            }
        }

        T get(int x, int y)
        {
            int gridIndex = getGridIndex(x, y);
            if (gridIndex == -1) return T();
            int tileIndex = getTileIndex(gridIndex, x, y);
            if (gridIndex == 0)
            {
                return this->gridArrayA[tileIndex];
            }
            else
            {
                return this->gridArrayB[tileIndex];
            }
        }

        void forEachCoord(std::function<void(int x, int y)> coordFunc) {
            for (int j = 0; j < halfHeight; j += 1)
            {
                for (int i = 0;i< halfWidth; i+=1)
                {
                    coordFunc(i*2, j*2);
                }

                for (int i = 0;i< halfWidth; i+=1)
                {
                    coordFunc((i*2)+1, (j*2)+1);
                }
            }
        }

        void forEachElement(std::function<void(T obj, int x, int y)> elFunc) {
            for (int j = 0; j < halfHeight; j += 1)
            {
                for (int i = 0;i< halfWidth; i+=1)
                {
                    elFunc(this->get(i*2, j*2), i*2, j*2);
                }

                for (int i = 0;i< halfWidth; i+=1)
                {
                    elFunc(this->get((i*2)+1, (j*2)+1), (i*2)+1, (j*2)+1);
                }
            }
        }

        int width = -1;
        int height = -1;
        bool initialized = false;

    private:

        int getGridIndex(int x, int y)
        {
            if (x < 0 || y < 0 || x >= this->width || y >= this->width) return -1;
            if (x % 2 == 0 && y % 2 == 0) return 0;
            if ((x+1) % 2 == 0 && (y+1) % 2 == 0) return 1;
            return -1;
        }

        int getTileIndex(int gridIndex, int x, int y)
        {
            if (gridIndex == 0)
            {
                int xsmall = x / 2;
                int ysmall = y / 2;
                return (ysmall * (this->width/2)) + xsmall;
            }
            else
            {
                int xsmall = (x-1) / 2;
                int ysmall = (y-1) / 2;
                return (ysmall * (this->width/2)) + xsmall;
            }
        }

        T* initGridArray(int width, int height)
        {
            T* output = new T[width * height];
            for (int j = 0; j < height; j += 1)
            {
                for (int i = 0;i< width; i+=1)
                {
                    output[(j*width)+i] = T();
                }
            }
            return output;
        }

        T* gridArrayA = nullptr;
        T* gridArrayB = nullptr;
        int halfWidth;
        int halfHeight;
    };
}