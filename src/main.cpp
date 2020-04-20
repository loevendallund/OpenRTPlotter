#include "OpenRTPlotter.h"
#include <iostream>

void TestFunctions(std::vector<OpenRTP::Plot> Plot);

int main() {
	OpenRTP::InitStruct Init 
	{
		"Window",
		"Y",
		"X"
	};

	std::vector<OpenRTP::Plot> Plot;

	Plot.push_back({"x1"});
    Plot.push_back({"x2"});

    //std::vector<Point> x1, x2;
    for (int i = 0; i < 4000; i++) {
        //Sinus
		float x = i / 10.0;

        GLfloat vx = x;
        GLfloat vy = sin(x) * 2 + 10;

        OpenRTP::Point v = {vx, vy};

        Plot[0].Function.push_back(v);

        vx = i / 100;
        vy = i / 100;
        v = {vx, vy};

        Plot[1].Function.push_back(v);
  	}

    Plot[0].Color = glm::vec4(1, 0, 0, 1);
    Plot[1].Color = glm::vec4(1, 0, 1, 1);

	OpenRTP::OpenRTPlotter *mPlot = new OpenRTP::OpenRTPlotter(Init, Plot);

	for (int i = 2000; i < 3000; i++)
	{
        //Sinus
		float x = i / 10.0;

        GLfloat vx = x / 4;
        GLfloat vy = sin(x * 10.0) / (1.0 + x * x) + 1;

        OpenRTP::Point v = {vx, vy};

        Plot[0].Function.push_back(v);

		vx = x / 4;
        vy = x / 4;
        v = {vx, vy};

        Plot[1].Function.push_back(v);
	}

	//mPlot->InsertPointRangeByPlot(Plot, 2000, 2999);

	int ret = mPlot->OpenRTPlotterInit();

	return ret;
}