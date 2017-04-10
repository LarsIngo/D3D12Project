#pragma once

#include <string>
#include <fstream>

// Profiler.
class Profiler {
    public:
        // Constructor.
        Profiler(unsigned int width, unsigned int height)
        {
            mFileStream.open("GraphScript.m");

            mFileStream << "clear all;\nclose all;\n";

            mFileStream << "h = figure;\n";

            mFileStream << "set(gcf, 'units', 'points', 'position', [0, 0, " + std::to_string(width) + ", " + std::to_string(height) + "]);\n";

            //mStrStream >> "xlim([0 10]);\n";
            //mStrStream >> "ylim([0 2]);\n";

            mFileStream << "xlabel('x');\n";
            mFileStream << "ylabel('y');\n";
        }

        // Destructor.
        ~Profiler()
        {
            mFileStream << "saveas(h, 'file', 'png');\n";

            mFileStream.close();

        }

        void Rectangle( UINT64 x, UINT64 y, UINT64 width, UINT64 height,
            float r, float g, float b)
        {
            mFileStream << "rectangle('Position', [" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(width) + ", " + std::to_string(height) + "], 'FaceColor', [" + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + "]);\n";
        }

    private:
        std::ofstream mFileStream;
};
