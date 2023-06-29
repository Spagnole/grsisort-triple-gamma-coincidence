#define MyGGGCubeSelector_cxx
// The class definition in MyGGGCubeSelector.h has been generated automatically
#include "MyGGGCubeSelector.h"

int kValue_grif = 379; // default k-value for not piled-up hits

void MyGGGCubeSelector::CreateHistograms() {
	// define Trees
	fTree["ggg_tree"] = new TTree("ggg_tree", "tripple coincident events");
	// set branch addresses for output tree (these can be different for different trees)
	// we save the entry number as well to detect possible double counting
	// four coincident gammas a,b,c,d will be saved as a,b,c, a,b,d, a,c,d, and b,c,d
	fTree["ggg_tree"]->Branch("gE", &gE,"gE[3]/F");
	//fTree["ggg_tree"]->Branch("gTD", &gTD,"gTD[3]/F");
	fTree["ggg_tree"]->Branch("prompt_time", &prompt_time,"prompt_time[3]/O");
	//fTree["ggg_tree"]->Branch("gamma_mult", &gamma_mult);
	fTree["ggg_tree"]->Branch("CycleTime", &CycleTime, "CycleTime/F");
	//fTree["ggg_tree"]->Branch("BeamOff", &BeamOff);
	//We can also create histograms at the same time, maybe for some diagnostics or simple checks
	fH1["gE"] = new TH1D("gE","#total projection triple coincidence gamma-energy", 6000, 0.5, 6000.5); 
	fH1["ggTD"] = new TH1D("ggTD","#gamma-#gamma time differences prompt or random", 10000, -5000, 5000); 
	fH1["ggTD_full"] = new TH1D("ggTD_full","#gamma-#gamma time differences all", 10000, -5000, 5000);
	
	fH2["ggTD_matrix_12vs23"] = new TH2D("ggTD_matrix_12vs23","g-g td (gamma 2 - gamma 1), (gamma 3 - gamma 2)", 5000, -2500, 2500, 5000, -2500, 2500);
	fH2["ggTD_matrix_12vs13"] = new TH2D("ggTD_matrix_12vs13","g-g td (gamma 2 - gamma 1), (gamma 3 - gamma 1)", 5000, -2500, 2500, 5000, -2500, 2500);
	
	fH1["ggTD_p12_13"] = new TH1D("ggTD_p12_13","g-g td prompt 1-2 td 1-3", 4000, -1000, 3000);
	fH1["ggTD_p12_23"] = new TH1D("ggTD_p12_23","g-g td prompt 1-2 td 2-3", 4000, -1000, 3000);
	fH1["ggTD_r12_13"] = new TH1D("ggTD_r12_13","g-g td random 1-2 td 1-3", 4000, -1000, 3000);
	fH1["ggTD_r12_23"] = new TH1D("ggTD_r12_23","g-g td random 1-2 td 2-3", 4000, -1000, 3000);
	
	fH1["ggTD_p13_12"] = new TH1D("ggTD_p13_12","g-g td prompt 1-3 td 1-2", 4000, -1000, 3000);
	fH1["ggTD_p13_23"] = new TH1D("ggTD_p13_23","g-g td prompt 1-3 td 2-3", 4000, -1000, 3000);
	fH1["ggTD_r13_12"] = new TH1D("ggTD_r13_12","g-g td random 1-3 td 1-2", 4000, -1000, 3000);
	fH1["ggTD_r13_23"] = new TH1D("ggTD_r13_23","g-g td random 1-3 td 2-3", 4000, -1000, 3000);
	
	fH1["ggTD_p23_12"] = new TH1D("ggTD_p23_12","g-g td prompt 2-3 td 1-2", 4000, -1000, 3000);
	fH1["ggTD_p23_13"] = new TH1D("ggTD_p23_13","g-g td prompt 2-3 td 1-3", 4000, -1000, 3000);
	fH1["ggTD_r23_12"] = new TH1D("ggTD_r23_12","g-g td random 2-3 td 1-2", 4000, -1000, 3000);
	fH1["ggTD_r23_13"] = new TH1D("ggTD_r23_13","g-g td random 2-3 td 1-3", 4000, -1000, 3000);
	
	fH1["ggTD_p12p13_td23"] = new TH1D("ggTD_p12p13_td23","g-g td 2-3: 1-2 prompt 1-3 prompt td 1-3", 4000, -1000, 3000);
	fH1["ggTD_p12r13_td23"] = new TH1D("ggTD_p12r13_td23","g-g td 2-3: 1-2 prompt 1-3 random td 1-3", 4000, -1000, 3000);

	fH1["ggTD_r12p13_td23"] = new TH1D("ggTD_r12p13_td23","g-g td 2-3: 1-2 random 1-3 prompt td 1-3", 4000, -1000, 3000);
	fH1["ggTD_r12r13_td23"] = new TH1D("ggTD_r12r13_td23","g-g td 2-3: 1-2 random 1-3 random td 1-3", 4000, -1000, 3000);

	fH1["ggTD_p13p23_td12"] = new TH1D("ggTD_p13p23_td12","g-g td 1-2: 1-3 prompt 2-3 prompt td 1-3", 4000, -1000, 3000);
	fH1["ggTD_p13r23_td12"] = new TH1D("ggTD_p13r23_td12","g-g td 1-2: 1-3 prompt 2-3 random td 1-3", 4000, -1000, 3000);

	fH1["ggTD_r13p23_td12"] = new TH1D("ggTD_r13p23_td12","g-g td 1-2: 1-3 random 2-3 prompt td 1-3", 4000, -1000, 3000);
	fH1["ggTD_r13r23_td12"] = new TH1D("ggTD_r13r23_td12","g-g td 1-2: 1-3 random 2-3 random td 1-3", 4000, -1000, 3000);
	
	fH1["ggTD_p23p12_td13"] = new TH1D("ggTD_p23p12_td13","g-g td 1-3: 2-3 prompt 1-2 prompt td 1-2", 4000, -1000, 3000);
	fH1["ggTD_p23r12_td13"] = new TH1D("ggTD_p23r12_td13","g-g td 1-3: 2-3 prompt 1-2 random td 1-2", 4000, -1000, 3000);

	fH1["ggTD_r23p12_td13"] = new TH1D("ggTD_r23p12_td13","g-g td 1-3: 2-3 random 1-2 prompt td 1-2", 4000, -1000, 3000);
	fH1["ggTD_r23r12_td13"] = new TH1D("ggTD_r23r12_td13","g-g td 1-3: 2-3 random 1-2 random td 1-2", 4000, -1000, 3000);
		
	//Send histograms to Output list to be added and written.
	for(auto it : fH1) {
		GetOutputList()->Add(it.second);
	}
	for(auto it : fH2) {
		GetOutputList()->Add(it.second);
	}
	for(auto it : fTree) {
		GetOutputList()->Add(it.second);
	}
}

//information regarding tape cycle
double TapeMove = 1.5;
double BackgroundTime = 0.5;
double Implantation = 45.;
double Decay = 13.;
double MyCycleLength = TapeMove+BackgroundTime+Implantation+Decay;

bool PromptCoincidence(TGriffinHit* g, TZeroDegreeHit* z){
	//Check if hits are less then 300 ns apart.
	return std::fabs(g->GetTime() - z->GetTime()) < 300.;
}

bool PromptCoincidence(TGriffinHit* g, TSceptarHit* s){
	//Check if hits are less then 300 ns apart.
	return std::fabs(g->GetTime() - s->GetTime()) < 300.;
}

bool PromptCoincidence(TGriffinHit* g1, TGriffinHit* g2){
	//Check if hits are less then 500 ns apart.
	//return std::fabs(g1->GetTime() - g2->GetTime()) < 300.;
	return -100. < (g2->GetTime() - g1->GetTime()) && (g2->GetTime() - g1->GetTime()) < 300.;
}

bool TimeRandom(TGriffinHit *h1, TGriffinHit *h2){  //Check if hits are less then 1000 ns apart.
   //return 500. < std::fabs(h1->GetTime() - h2->GetTime()) && std::fabs(h1->GetTime() - h2->GetTime()) < 1500.;
   return 500. < (h2->GetTime() - h1->GetTime()) && (h2->GetTime() - h1->GetTime()) < 1500.;
}

//flag to check event occured when beam is implanted or decay is being measured i.e excludes events recorded during tape move and background measurement
bool InCyle(TGriffinHit *h1){
	return  fmod(h1->GetTimeStampNs()/1E9, MyCycleLength) > (TapeMove+BackgroundTime) && fmod(h1->GetTimeStampNs()/1E9, MyCycleLength) < (TapeMove+BackgroundTime+Implantation+Decay);
}

void MyGGGCubeSelector::FillHistograms() {
	// we could check multiplicities here and skip events where we do not have at least
	// three suppressed addback energies and a beta, but we want to fill some general 
	// histograms without these cuts.
	//fSuppressedAddback.resize(3, 0.);
	
	if( fGrif->GetSuppressedAddbackMultiplicity(fGriffinBgo) > 2 && fGrif->GetSuppressedAddbackMultiplicity(fGriffinBgo) < 8){	
		for(auto g1 = 0; g1 < fGrif->GetSuppressedAddbackMultiplicity(fGriffinBgo); ++g1) {
			auto grif1 = fGrif->GetSuppressedAddbackHit(g1);
			gE[0] = 0.; gE[1] = 0.; gE[2] = 0.;
			gE[0] = grif1->GetEnergy();
			gamma_mult = fGrif->GetSuppressedAddbackMultiplicity(fGriffinBgo);
			if(grif1->GetKValue() != kValue_grif) continue;
			if(!InCyle(grif1)) continue;
			for( auto g2 = g1+1; g2 < fGrif->GetSuppressedAddbackMultiplicity(fGriffinBgo); ++g2){
				if( g1 == g2 ) continue;
				auto grif2 = fGrif->GetSuppressedAddbackHit(g2);
				if(grif2->GetKValue() != kValue_grif) continue;
				if( grif1->GetDetector() == grif2->GetDetector() ) continue;
				if(!InCyle(grif2)) continue;
				gE[1] = grif2->GetEnergy();
				for( auto g3 = g2+1; g3 < fGrif->GetSuppressedAddbackMultiplicity(fGriffinBgo); ++g3){
					if( g1 == g3 || g2 == g3 ) continue;
					auto grif3 = fGrif->GetSuppressedAddbackHit(g3);
					if(!InCyle(grif3)) continue;
					if(grif3->GetKValue() != kValue_grif) continue;
					if( grif3->GetDetector() == grif2->GetDetector() || grif3->GetDetector() == grif1->GetDetector() ) continue;
					gE[2] = grif3->GetEnergy();
					prompt_time[0] = 0; prompt_time[1] = 0; prompt_time[2] = 0;
					CycleTime = fmod(grif1->GetTimeStampNs()/1E9, MyCycleLength);
					//gTD[0] = grif1->GetTime() - grif2->GetTime();
					//gTD[1] = grif1->GetTime() - grif3->GetTime();
					//gTD[2] = grif2->GetTime() - grif3->GetTime();
					fH1.at("ggTD_full")->Fill( grif2->GetTime() - grif1->GetTime() );
					fH1.at("ggTD_full")->Fill( grif3->GetTime() - grif1->GetTime() );
					fH1.at("ggTD_full")->Fill( grif3->GetTime() - grif2->GetTime() );
					fH2.at("ggTD_matrix_12vs23")->Fill( grif2->GetTime() - grif1->GetTime(), grif3->GetTime() - grif2->GetTime() );
					fH2.at("ggTD_matrix_12vs13")->Fill( grif2->GetTime() - grif1->GetTime(), grif3->GetTime() - grif1->GetTime() );
					
					if( PromptCoincidence(grif1,grif2) ){
						fH1.at("ggTD_p12_13")->Fill( grif3->GetTime() - grif1->GetTime() );
						fH1.at("ggTD_p12_23")->Fill( grif3->GetTime() - grif2->GetTime() );
					}
					if( TimeRandom(grif1,grif2) ){
						fH1.at("ggTD_r12_13")->Fill( grif3->GetTime() - grif1->GetTime() );
						fH1.at("ggTD_r12_23")->Fill( grif3->GetTime() - grif2->GetTime() );
					}

					if( PromptCoincidence(grif1,grif3) ){
						fH1.at("ggTD_p13_12")->Fill( grif2->GetTime() - grif1->GetTime() );
						fH1.at("ggTD_p13_23")->Fill( grif3->GetTime() - grif2->GetTime() );
					}
					if( TimeRandom(grif1,grif3) ){
						fH1.at("ggTD_r13_12")->Fill( grif2->GetTime() - grif1->GetTime() );
						fH1.at("ggTD_r13_23")->Fill( grif3->GetTime() - grif2->GetTime() );
					}					

					if( PromptCoincidence(grif2,grif3) ){
						fH1.at("ggTD_p23_12")->Fill( grif2->GetTime() - grif1->GetTime() );
						fH1.at("ggTD_p23_13")->Fill( grif3->GetTime() - grif1->GetTime() );
					}
					if( TimeRandom(grif2,grif3) ){
						fH1.at("ggTD_r23_12")->Fill( grif2->GetTime() - grif1->GetTime() );
						fH1.at("ggTD_r23_13")->Fill( grif3->GetTime() - grif1->GetTime() );
					}		
					
					if( PromptCoincidence(grif1,grif2)	&&	PromptCoincidence(grif1,grif3) )	fH1.at("ggTD_p12p13_td23")->Fill( grif3->GetTime() - grif2->GetTime() );
					if( PromptCoincidence(grif1,grif2)	&&	TimeRandom(grif1,grif3) )			fH1.at("ggTD_p12r13_td23")->Fill( grif3->GetTime() - grif2->GetTime() );		
					if( TimeRandom(grif1,grif2)			&&	PromptCoincidence(grif1,grif3) )	fH1.at("ggTD_r12p13_td23")->Fill( grif3->GetTime() - grif2->GetTime() );
					if( TimeRandom(grif1,grif2)			&&	TimeRandom(grif1,grif3) )			fH1.at("ggTD_r12r13_td23")->Fill( grif3->GetTime() - grif2->GetTime() );
					
					if( PromptCoincidence(grif1,grif3) 	&& PromptCoincidence(grif2,grif3) )		fH1.at("ggTD_p13p23_td12")->Fill( grif2->GetTime() - grif1->GetTime() );
					if( PromptCoincidence(grif1,grif3) 	&& TimeRandom(grif2,grif3) )			fH1.at("ggTD_p13r23_td12")->Fill( grif2->GetTime() - grif1->GetTime() );
					if( TimeRandom(grif1,grif3) 		&& PromptCoincidence(grif2,grif3) )		fH1.at("ggTD_r13p23_td12")->Fill( grif2->GetTime() - grif1->GetTime() );
					if( TimeRandom(grif1,grif3) 		&& TimeRandom(grif2,grif3) )			fH1.at("ggTD_r13r23_td12")->Fill( grif2->GetTime() - grif1->GetTime() );

					if( PromptCoincidence(grif2,grif3) 	&& PromptCoincidence(grif1,grif2) )		fH1.at("ggTD_p23p12_td13")->Fill( grif3->GetTime() - grif1->GetTime() );
					if( PromptCoincidence(grif2,grif3) 	&& TimeRandom(grif1,grif2) )			fH1.at("ggTD_p23r12_td13")->Fill( grif3->GetTime() - grif1->GetTime() );
					if( TimeRandom(grif2,grif3) 		&& PromptCoincidence(grif1,grif2) )		fH1.at("ggTD_r23p12_td13")->Fill( grif3->GetTime() - grif1->GetTime() );
					if( TimeRandom(grif2,grif3) 		&& TimeRandom(grif1,grif2) )			fH1.at("ggTD_r23r12_td13")->Fill( grif3->GetTime() - grif1->GetTime() );
					
					if( PromptCoincidence(grif1,grif2) ) prompt_time[0] = 1;
						else if( TimeRandom(grif1,grif2) ) prompt_time[0] = 0;
							else continue;
					if( PromptCoincidence(grif1,grif3) ) prompt_time[1] = 1;
						else if( TimeRandom(grif1,grif3) ) prompt_time[1] = 0;
							else continue;
					if( PromptCoincidence(grif2,grif3) ) prompt_time[2] = 1;
						else if( TimeRandom(grif2,grif3) ) prompt_time[2] = 0;
							else continue;
					
					//CycleTime = fmod(grif1->GetTimeStampNs()/1E9, MyCycleLength)+0.5; 		
					if( gE[0] > 0. && gE[1] > 0. && gE[2] > 0.){
						fTree.at("ggg_tree")->Fill();
						fH1.at("gE")->Fill( gE[0] );	fH1.at("gE")->Fill( gE[1] );	fH1.at("gE")->Fill( gE[2] );
						fH1.at("ggTD")->Fill( grif2->GetTime() - grif1->GetTime() );
						fH1.at("ggTD")->Fill( grif3->GetTime() - grif1->GetTime() );
						fH1.at("ggTD")->Fill( grif3->GetTime() - grif2->GetTime() );

					}
				}
			
			}
		}
	}
}
