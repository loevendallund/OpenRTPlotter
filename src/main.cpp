#include "OpenRTPlotter.h"

int main() {
	OpenRTP::OpenRTPlotter *mPlot = new OpenRTP::OpenRTPlotter();

	int ret = mPlot->OpenRTPlotterInit();

	return ret;
}
