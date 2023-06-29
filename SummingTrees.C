#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TPeakFitter.h"
#include "TPeak.h"
#include "TRWPeak.h"
#include "TABPeak.h"
#include <iostream>
#include <TSystem.h>
#include <chrono>
#include <ctime>

using namespace std;

//TFile *myfile = TFile::Open("ExampleTree19437_001.root");
//TTree *tt = (TTree*)myfile->Get("ggg_tree");

TFile *myfile;
//TTree *tt;
TChain tt("ggg_tree"); //the trees from the separate root files are added to a TChain!

TH1D *hTotalProj = new TH1D("hTotalProj","Total Projection of g-g-g cube",6000,0.5,6000.5); //user may have to change binning
TH1D *hTimeDiff = new TH1D("hTimeDiff","Total Projection of g-g-g cube",6000,0.5,6000.5);
TH1D *htemp;	TH1D *hTDtemp;	

TH1D *hBkgr; //hist for TSpectrum background

//this function is used to add all of the root files together to form a TChain to add all the trees together
void SumTrees(int FirstRun, int LastRun){

	int counter=0;
	for(int run_number = FirstRun; run_number <= LastRun; run_number++){
		for(int j = 0; j < 99; j++){	
			string temp = Form("subruns/ggg_cube_%d_0%02d.root",run_number,j); //you will probably have to change the names of the path	
			if( !gSystem->AccessPathName( temp.c_str() ) ){
				cout << temp << endl;
				myfile = TFile::Open( temp.c_str() );
				htemp = (TH1D*)myfile->Get("gE");
				hTotalProj->Add(htemp);
				myfile->Close();
				//tt = (TTree*)myfile->Get("ggg_tree");
				tt.Add(temp.c_str());
				counter++;
			}	
		}
	}
	cout << "Total Number of files addad = " << counter << endl;
}

//this function is used to find the scaling parameter for the Compton background
//This function can only fit single peaks i.e No doublet fitting!
//user can also just use default value which will scale to the width of the gates
void GetComptonBgScale(int peaktype, double peak, double fit_low, double fit_high, double gate_low, double gate_high, double Bg_low, double Bg_high){

	TPeakFitter *pf = new TPeakFitter(fit_low,fit_high);
	TRWPeak *p0 = new TRWPeak(peak);
	TF1* peakFunc;
	//p0->FixParameter(GetParNumber("peak"),1.00);
	TABPeak *p1 = new TABPeak(peak);
	//p1->FixParameter(GetParNumber("peak"),1.00);
	if(peaktype == 0 ){ 
		pf->AddPeak(p0);
		peakFunc = p0->GetFitFunction();
	}
	else if(peaktype == 1 ){ 
		pf->AddPeak(p1);
		peakFunc = p1->GetFitFunction();
	}
	else{
		cout << "No Peak Type Given\nUse option 0 for TRWPeak or use option 1 for TABPeak\n";
		return;
	}
	
	peakFunc->FixParameter(1, peak);
	pf->Fit(hTotalProj);
	peakFunc->ReleaseParameter(1);
	pf->Fit(hTotalProj);

	pf->GetFitFunction()->Integral(gate_low,gate_high);
	pf->GetBackground()->Integral(gate_low,gate_high);
	
	double BackGroundFull = hTotalProj->Integral(Bg_low,Bg_high);
	
	cout << "Peak Info\n";
	if(peaktype == 0 ) p0->Print();
	else if(peaktype == 1 ) p1->Print();
	cout << "Peak Intensity within peak gate:\t" << pf->GetFitFunction()->Integral(gate_low,gate_high) << endl;
	cout << "Background Intensity under peak gate:\t" << pf->GetBackground()->Integral(gate_low,gate_high) << endl;
	cout << "Intensity of Background energy gate:\t" << BackGroundFull << endl;
	cout << "Background Scaling Factor = " << pf->GetBackground()->Integral(gate_low,gate_high) / BackGroundFull << endl;
	
}

void TSpectrumBackground(TH1D *h, int iterations = 50, int decreasewindow = 1, int backorder = 2, bool smoothing = false, int smoothwindow = 3, bool compton = 0){

	//extract binning information from the experimental spectrum
	//we want to use the same binning to compare
	const int Nbins = h->GetXaxis()->GetNbins();
	double x_low = h->GetXaxis()->GetBinLowEdge(1);
	double x_max = h->GetXaxis()->GetBinUpEdge(Nbins);
	double source[Nbins];
	if( hBkgr == NULL ) hBkgr = new TH1D("hBkgr","Simulated Background",Nbins,x_low,x_max);
	
	TSpectrum *s = new TSpectrum();
	for (int i = 0; i < Nbins; i++) source[i]=h->GetBinContent(i + 1);
	//s->Background(source,Nbins,75,TSpectrum::kBackDecreasingWindow, TSpectrum::kBackOrder2,kFALSE, TSpectrum::kBackSmoothing3,kFALSE);
	s->Background(source,Nbins,iterations,decreasewindow,backorder,smoothing,smoothwindow,compton);
	for (int i = 0; i < Nbins; i++) hBkgr->SetBinContent(i + 1,source[i]);      
	h->Draw("hist");
	hBkgr->SetLineColor(kOrange+1);
	hBkgr->Draw("SAME L");
}

void GetBackgroundScaling(int Gate_Low, int Gate_high, int BG_low, int BG_high){

	if( hBkgr == NULL ){
		cout << "TSpectrum background is emply! Run TSpectrumBackground() function!\n";
		return;
	}
	double PeakGateInt = hBkgr->Integral(Gate_Low,Gate_high);
	double BkgrGateInt = hBkgr->Integral(BG_low,BG_high);
	double BkgrScalineRatio = PeakGateInt / BkgrGateInt;
	cout << "Scaling parameter for Background gate = " << BkgrScalineRatio << endl;
	const int Nbins = hBkgr->GetXaxis()->GetNbins();
	double x_low = hBkgr->GetXaxis()->GetBinLowEdge(1);
	double x_max = hBkgr->GetXaxis()->GetBinUpEdge(Nbins);
	TH1D *hGates = new TH1D("hGates","Gates",Nbins,x_low,x_max);
	hGates->SetLineColor(kRed);
	hGates->SetFillColor(kRed);
	hGates->SetFillStyle(3000);
	for(int i = Gate_Low; i <= Gate_high; i++) hGates->SetBinContent(i, hBkgr->GetBinContent(i) );
	for(int i = BG_low; i <= BG_high; i++) hGates->SetBinContent(i, hBkgr->GetBinContent(i) );
	
	hTotalProj->Draw("hist");
	hBkgr->Draw("SAME L");
	hGates->Draw("same");
}

//Here is where the user gates on the tripple-gamma coincidence tree to produces gamma-gamma matrices
// double BG_scale = -1. is the default value, if this value is used scaling of the compton background gate is performed using the gate widths
void MakeMatrix(int Gate_Low, int Gate_high, int BG_low, int BG_high, double BG_scale = -1.){
	
	auto Begining = std::chrono::system_clock::now(); //I have included a small clock to track how long it takes to build the matrices

	TFile *newfile = TFile::Open(Form("ggg_cube_Gate%d.root",(Gate_high+Gate_Low)/2),"RECREATE"); //creates a new file to write the matrices to
	
	//this is used to scale the time-random coincidences
	double prompt_time_gate = 325.; //should this value be set to 300?
	double random_time_gate = 1000;
	double ScaleTimeWindow = prompt_time_gate / random_time_gate;
	if(BG_scale < 0.){
		BG_scale = (double)(Gate_high - Gate_Low + 1 ) / (double)(BG_high - BG_low + 1 );
	}
	
	cout << "Time random subtraction scaling: " << ScaleTimeWindow << endl;
	cout << "Comptom Background subtraction scaling: " << BG_scale << endl;
	
	//ScaleTimeWindow = 0.1696+0.004;
	//this sets limits to cut the tree down to the events of interest
	int Min = std::min({Gate_Low,Gate_high,BG_low,BG_high});
	int Max = std::max({Gate_Low,Gate_high,BG_low,BG_high});
	

	//here we define the matrices for the peak and the comptom background gate
	int NBins = 7000; double first_bin = 0.5; double last_bin = 7000.5;
	string binning = Form("%d,%f,%f,%d,%f,%f", NBins, first_bin, last_bin, NBins, first_bin, last_bin);
	//first matrix pmr is to perform the correct time-random coincidences
	TH2F *mat_pmr_sum = new TH2F(Form("gg_mat_E%d_pmr_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, prompt minus random, time-gates prompt = %d, time-random = %d", (Gate_high+Gate_Low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	//g1-g2 and g1-g3 are in prompt coincidence
	TH2F *mat_pp_sum = new TH2F(Form("gg_mat_E%d_pp_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, g1-g2 prompt, g1-g3 prompt, prompt-time-gate = %d,time-random-gate = %d",(Gate_high+Gate_Low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	//g1-g2 are in prompt coincidence and g1-g3 are in time-random coindence
	TH2F *mat_pr_sum = new TH2F(Form("gg_mat_E%d_pr_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, g1-g2 prompt, g1-g3 random, prompt-time-gate = %d,time-random-gate = %d",(Gate_high+Gate_Low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	//g1-g2 are in time-random coincidence and g1-g3 are in prompt coindence
	TH2F *mat_rp_sum = new TH2F(Form("gg_mat_E%d_rp_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, g1-g2 random, g2-g3 prompt, prompt-time-gate = %d,time-random-gate = %d",(Gate_high+Gate_Low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	//g1-g2 and g1-g3 are in time-random coincidence
	TH2F *mat_rr_sum = new TH2F(Form("gg_mat_E%d_rr_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, g1-g2 random, g2-g3 random, prompt-time-gate = %d,time-random-gate = %d",(Gate_high+Gate_Low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	
	//repeats but for Compton background energy gate
	TH2F *matBG_pmr_sum = new TH2F(Form("ggBG_mat_E%d_pmr_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, prompt minus random, time-gates prompt = %d, time-random = %d", (BG_high+BG_low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	TH2F *matBG_pp_sum = new TH2F(Form("ggBG_mat_E%d_pp_sum",(BG_high+BG_low)/2), Form("g-g matrix Compt. Bg gated on %d, g1-g2 prompt, g1-g3 prompt, prompt-time-gate = %d,time-random-gate = %d",(BG_high+BG_low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	TH2F *matBG_pr_sum = new TH2F(Form("ggBG_mat_E%d_pr_sum",(BG_high+BG_low)/2), Form("g-g matrix Compt. Bg gated on %d, g1-g2 prompt, g1-g3 random, prompt-time-gate = %d,time-random-gate = %d",(BG_high+BG_low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	TH2F *matBG_rp_sum = new TH2F(Form("ggBG_mat_E%d_rp_sum",(BG_high+BG_low)/2), Form("g-g matrix Compt. Bg gated on %d, g1-g2 random, g2-g3 prompt, prompt-time-gate = %d,time-random-gate = %d",(BG_high+BG_low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	TH2F *matBG_rr_sum = new TH2F(Form("ggBG_mat_E%d_rr_sum",(BG_high+BG_low)/2), Form("g-g matrix Compt. Bg gated on %d, g1-g2 random, g2-g3 random, prompt-time-gate = %d,time-random-gate = %d",(BG_high+BG_low)/2, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);

	////final matrix summing all matrices together
	TH2F *mat_PeakmBg_pmr_sum = new TH2F(Form("gg_mat_E%d_PmBg_pmr_sum",(Gate_high+Gate_Low)/2), Form("g-g matrix gated on %d, Bg Sub Gate at %d, Bg Sub Scale = %f, prompt minus random, time-gates prompt = %d, time-random = %d", (Gate_high+Gate_Low)/2,(BG_high+BG_low)/2, BG_scale, (int)prompt_time_gate, (int)random_time_gate), NBins,first_bin,last_bin, NBins,first_bin,last_bin);
	
	//this is the new tree, there are three trees to deal with all projections
	TTree *temptree[3];
	
	//here the TChain is cut into three separate trees where with energy condition is met 
	//again we have to look at the first TBranch gE[0] then ge[1] then gE[2] to ensure we recover all projections of the asymmetric tree
	auto start = std::chrono::system_clock::now();
	cout << "Cutting tree down into three smaller trees" << endl;
	for(int i = 0; i < 3; i++){
		temptree[i] = tt.CopyTree(Form("gE[%d]>%d&&gE[%d]<%d",i,Min,i,Max));
		temptree[i]->SetName(Form("ggg_tree_%d",i));	
	}
	auto finish = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = finish - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s" << endl;
	cout << "Finished tree down into three smaller trees" << endl;

	//this is for drawing the all projections of the tree 
	//e.g. for gating on the transition with gE[0] we make a matrix with gE[1] vs gE[2]
	//then draw a matrix with gE[2] vs gE[1]
	int neworder[3][2][2] = { {{1,2}, {2,1}}, {{2,0}, {0,2}}, {{0,1}, {1,0}} };

	//these matrices are to sum all the different projections together
	TH2F *mat_Peak_pp[3][2];
	TH2F *mat_Peak_rp[3][2];
	TH2F *mat_Peak_pr[3][2];
	TH2F *mat_Peak_rr[3][2];
	
	TH2F *matBG_Peak_pp[3][2];
	TH2F *matBG_Peak_rp[3][2];
	TH2F *matBG_Peak_pr[3][2];
	TH2F *matBG_Peak_rr[3][2];
	
	//drawing the matrices
	for(int i = 0; i < 3; i++){ //going through the three trees
		cout << "Drawing " << i+1 << "-th Projections\n";
		for(int j = 0; j < 2; j++){ //drawing both the x-y and y-x matrices
			//drawing trees
			//prompt-prompt coincidence
			//Step 1 draw matrix from tree
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>gg_mat_E%d_pp_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==1&&prompt_time[%d]==1",i,Gate_Low,i,Gate_high,0,1) ); 
			//Step 2 gets matrix to add to sum of projections matrix
			mat_Peak_pp[i][j] = (TH2F*)newfile->Get(Form("gg_mat_E%d_pp_%d%d",(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1])); 			
			//Step 3 adds matrix to sum
			mat_pp_sum->Add(mat_Peak_pp[i][j]);
			//repeats from here with different time-coincidence condition
			
			//random-prompt coincidence			
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>gg_mat_E%d_rp_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==0&&prompt_time[%d]==1",i,Gate_Low,i,Gate_high,0,2) );
			mat_Peak_rp[i][j] = (TH2F*)newfile->Get(Form("gg_mat_E%d_rp_%d%d",(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1]));
			mat_rp_sum->Add(mat_Peak_rp[i][j]);
			//prompt random coincidence						
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>gg_mat_E%d_pr_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==1&&prompt_time[%d]==0",i,Gate_Low,i,Gate_high,0,1) );	
			mat_Peak_pr[i][j] = (TH2F*)newfile->Get(Form("gg_mat_E%d_pr_%d%d",(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1]));
			mat_pr_sum->Add(mat_Peak_pr[i][j]);
			//random-random coincidence									
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>gg_mat_E%d_rr_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==0&&prompt_time[%d]==0",i,Gate_Low,i,Gate_high,0,2) );
			mat_Peak_rr[i][j] = (TH2F*)newfile->Get(Form("gg_mat_E%d_rr_%d%d",(Gate_high+Gate_Low)/2,neworder[i][j][0],neworder[i][j][1]));
			mat_rr_sum->Add(mat_Peak_rr[i][j]);						
			//cout << i << " " << neworder[i][j][0] << endl;
			
			//REPEAT GATING PROCEDURE FOR COMPTON BACKGROUND
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>ggBG_mat_E%d_pp_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==1&&prompt_time[%d]==1",i,BG_low,i,BG_high,0,1) );
			matBG_Peak_pp[i][j] = (TH2F*)newfile->Get(Form("ggBG_mat_E%d_pp_%d%d",(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1]));
			matBG_pp_sum->Add(matBG_Peak_pp[i][j]);
			
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>ggBG_mat_E%d_rp_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==0&&prompt_time[%d]==1",i,BG_low,i,BG_high,0,2) );
			matBG_Peak_rp[i][j] = (TH2F*)newfile->Get(Form("ggBG_mat_E%d_rp_%d%d",(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1]));
			matBG_rp_sum->Add(matBG_Peak_rp[i][j]);						
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>ggBG_mat_E%d_pr_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==1&&prompt_time[%d]==0",i,BG_low,i,BG_high,0,1) );	
			matBG_Peak_pr[i][j] = (TH2F*)newfile->Get(Form("ggBG_mat_E%d_pr_%d%d",(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1]));
			matBG_pr_sum->Add(matBG_Peak_pr[i][j]);									
			temptree[i]->Draw(Form("gE[%d]:gE[%d]>>ggBG_mat_E%d_rr_%d%d(%s)",neworder[i][j][0],neworder[i][j][1],(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1], binning.c_str()),
				Form("gE[%d]>%d&&gE[%d]<%d&&prompt_time[%d]==0&&prompt_time[%d]==0",i,BG_low,i,BG_high,0,2) );
			matBG_Peak_rr[i][j] = (TH2F*)newfile->Get(Form("ggBG_mat_E%d_rr_%d%d",(BG_high+BG_low)/2,neworder[i][j][0],neworder[i][j][1]));
			matBG_rr_sum->Add(matBG_Peak_rr[i][j]);	

			
			//deleting matrices that are not longer needed
			mat_Peak_pp[i][j]->Delete();
			mat_Peak_rp[i][j]->Delete();
			mat_Peak_pr[i][j]->Delete();
			mat_Peak_rr[i][j]->Delete();		
			matBG_Peak_pp[i][j]->Delete();
			matBG_Peak_pr[i][j]->Delete();
			matBG_Peak_rp[i][j]->Delete();
			matBG_Peak_rr[i][j]->Delete();	
			

		}
		//cout << neworder[i][0][0] << " " <<  neworder[i][0][1] << " " <<  neworder[i][1][0] << " " <<  neworder[i][1][1] << " " << endl;				
	}
	
	//here the matrices are all added together to produce a peak minus compton background time-random subtracted matrix
	//dealing with peak-energy gate
	mat_pmr_sum->Add(mat_pp_sum);
	mat_pmr_sum->Add(mat_pr_sum,-1*ScaleTimeWindow);	
	//mat_rp_sum->Add(mat_rr_sum,-1*ScaleTimeWindow);	
	mat_pmr_sum->Add(mat_rp_sum,-1*ScaleTimeWindow);
	mat_pmr_sum->Add(mat_rr_sum,1*ScaleTimeWindow*ScaleTimeWindow);
	//dealing with comptom background gate
	matBG_pmr_sum->Add(matBG_pp_sum);
	matBG_pmr_sum->Add(matBG_rp_sum,-1*ScaleTimeWindow);
	//matBG_pr_sum->Add(matBG_rr_sum,-1*ScaleTimeWindow);
	matBG_pmr_sum->Add(matBG_pr_sum,-1*ScaleTimeWindow);
	matBG_pmr_sum->Add(matBG_rr_sum,1*ScaleTimeWindow*ScaleTimeWindow);
	
	//mat_pmr_sum->Add(matBG_pmr_sum,-1*BG_scale);
	
	//making peak minus compton matrix
	mat_PeakmBg_pmr_sum->Add(mat_pmr_sum);
	mat_PeakmBg_pmr_sum->Add(matBG_pmr_sum,-1*BG_scale);
	
	//writing matrices
	hTotalProj->Write();
	mat_PeakmBg_pmr_sum->Write();
	mat_pmr_sum->Write();
	matBG_pmr_sum->Write();
	
	///this is just stating how long everything has taken	
	auto Ending = std::chrono::system_clock::now();
	std::chrono::duration<double> TotalTime = Ending - Begining;
	std::cout << "Total time taken: " << TotalTime.count() << " s" << endl;
	
	//writing matrices to file (preserving all matrices to allow for corrections in case of errors without having to draw tree again
	//I have not written anything to do this yet. It should be simple enough however, it is just adding matrices together
	mat_pp_sum->Write();
	mat_pr_sum->Write();
	mat_rp_sum->Write();
	mat_rr_sum->Write();
	matBG_pp_sum->Write();
	matBG_pr_sum->Write();
	matBG_rp_sum->Write();
	matBG_rr_sum->Write();

}

	
