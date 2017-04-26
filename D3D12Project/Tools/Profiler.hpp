#pragma once

#include <string>
#include <fstream>
#include <sstream>

// Profiler.
class Profiler {
public:
    // Constructor.
    Profiler(unsigned int width, unsigned int height)
    {
        mWidth = width;
        mHeight = height;
    }

    // Destructor.
    ~Profiler()
    {
        mFileStream.open("GraphScript.m");

        mFileStream << "clear all;\nclose all;\n";

        mFileStream << "set(gcf, 'units', 'points', 'position', [0, 0, " + std::to_string(mWidth) + ", " + std::to_string(mHeight) + "]);\n";

        mFileStream << "xlabel('x');\n";
        mFileStream << "ylabel('y');\n";

        mFileStream << "subplot(2,1,2);\n";

        mFileStream << mRectangleStream.str();

        mFileStream << "subplot(2,1,1);\n";
        mFileStream << "hold on;\n";

        mFileStream << mPointStream.str();

        mFileStream.close();

    }

    void Rectangle(UINT64 x, UINT64 y, UINT64 width, UINT64 height,
        float r, float g, float b)
    {
        mRectangleStream << "rectangle('Position', [" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(width) + ", " + std::to_string(height) + "], 'FaceColor', [" + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + "]);\n";
    }

    void Point(UINT64 x, UINT64 y, char* cmd = "'-ro'")
    {
        mPointStream << "plot(" + std::to_string(x) + ", " + std::to_string(y) + ", " + cmd + ");\n";
    }

private:
    std::ofstream mFileStream;
    std::stringstream mRectangleStream;
    std::stringstream mPointStream;
    unsigned int mWidth;
    unsigned int mHeight;
};
