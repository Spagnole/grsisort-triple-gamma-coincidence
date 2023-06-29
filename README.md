# grsisort-triple-gamma-coincidence
grsiproof selector to produce triple coincidence TTree and code to build gated gamma-gamma matrices to evaluate triple coincdinces

The g-g-g coincidences are built from the grsiproof selector. 
TTree("ggg_tree", "tripple coincident events");
The ggg_tree is asymmetric to reduce the size of the TTree as a symmetric tree may be too large to write the file.
Four coincident gammas a,b,c,d will be saved as a,b,c, a,b,d, a,c,d, and b,c,d
The time-coincidence is given as a boolean. The value is 1 for prompt-coincidence and 0 for time-random-coincidence

It is best to sort the subruns individually which will later be added together in a TChain by the SummingTrees.C code.

Now that you have produced the g-g-g trees, you can use the SummingTrees.C code to make gated-gamma-gamma matrices
You must first compile the code in a grsisort session i.e.
> grsisort -l
> .L SummingTrees.C++

Then give the run number you want to use
> SumTrees(int FirstRun, int LastRun)

Look at the total projection to decide your gates
> hTotalProj->Draw()

You can build the matricesby doing
> MakeMatrix(int Gate_Low, int Gate_high, int BG_low, int BG_high, double BG_scale = -1.)

BG_scale = -1. is the default value. If you use the default value, the scaling for the background subtraction is determined by the width of the two gates.

A user can determine their own background scaling parameter with the GetComptonBgScale() or the TSpectrumBackground() + GetBackgroundScaling() funtions.

First Option: GetComptonBgScale()
> GetComptonBgScale(int peaktype, double peak, double fit_low, double fit_high, double gate_low, double gate_high, double Bg_low, double Bg_high)

This funtion will fit a single peak.
> Define your peak type, give the centroid of the peak, the range of the fit, and both the peak gates and the backround gates

Second Option: TSpectrumBackground(TH1D *h, int iterations = 50, int decreasewindow = 1, int backorder = 2, bool smoothing = false, int smoothwindow = 3, bool compton = 0)
> TSpectrumBackground(TH1D *h, int iterations = 50, int decreasewindow = 1, int backorder = 2, bool smoothing = false, int smoothwindow = 3, bool compton = 0)

This will extract a backround  from the total projection spectrum
h is the total projection spectrum
You have some option on how to fit the background. This set can be performed multiple times to get the best fit!
> GetBackgroundScaling(int Gate_Low, int Gate_high, int BG_low, int BG_high)

This function will then determine the background scaling and draw a nice canvas showing your Total projection, extracted background and gates to be used.

Now you can just run MakeMatrix() but give your value for the background scaling rather than using the default.

Pleae Note!!!
The time-random scaling is determined in the MakeMatrix() funxtion. User should decide on an appropriate value based on the timing conditions chosen for the grsiproof selector.
