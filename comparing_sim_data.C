// This program compares simulated results to experimental data

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TMath.h"

using namespace std;

#define SIM_FILE "sim_outfiles/test3.root"
#define EXP_FILE "hms_files/Pion_hms_replay_production_13092_-1.root"

// All files
// #define EXP_FILE "hms_files/Pion_hms_replay_production_12080_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_12083_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13092_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13133_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13854_-1.root"

// #define EXP_FILE "hms_files/Pion_hms_replay_production_13094_-1.root" DUMMY
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13136_-1.root" DUMMY
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13857_-1.root" DUMMY

#define NUM_VARS 8
#define HIST_NAMES(i)	info[i][0]
#define SIM_VARS(i)		info[i][1]
#define EXP_VARS(i)		info[i][2]
#define CUTS(i)			info[i][3]
#define LOW_LIM(i)		stoi(info[i][4])	// also converts to number
#define UP_LIM(i)		stoi(info[i][5])	// also converts to number

void 			comparing_sim_data()
{
	// hist name, simulated data's variable name, experimental data's variable name, cuts, lower bound, upper bound
	string	info[8][6] = {	"x focal plane",	"hsxfp",	"H.dc.x_fp",				"",			"-20",	"20",
                  			"y focal plane",	"hsyfp",	"H.dc.y_fp",				"",			"-20",	"20",
                  			"xp focal plane",	"hsxpfp",	"H.dc.xp_fp",				"",			"-20",	"20",
                  			"yp focal plane",	"hsypfp",	"H.dc.yp_fp",				"",			"-20",	"20",
                  			"nu",				"nu",		"H.kin.primary.nu",			"",			"0",	"10",
                  			"Q^2",				"Q2",		"H.kin.primary.Q2",			"",			"0",	"10",
                  			"W",				"W",		"H.kin.primary.W",			" >= 0 ",	"0",	"10",
              				"epsilon", 			"epsilon",	"H.kin.primary.epsilon",	"",			"0",	"10"};

	// loads the root files
	TFile		*exp_data = TFile::Open(EXP_FILE);
	TFile		*sim_data = TFile::Open(SIM_FILE);

	// load trees from files
	TTree		*exp_tree;
	exp_data->GetObject("T", exp_tree);

	TTree		*sim_tree;
	sim_data->GetObject("h10", sim_tree);

	TCanvas		c1; // for saving drawings

	for (size_t i = 0; i < NUM_VARS; ++i)
	{
		// class to plot multiple histograms together THStack(name, title)
		THStack	*comparison_hist = new THStack("hists", (HIST_NAMES(i)).c_str());

		// draw var with "graph off". " >> hist" writes to a histogram in the gDir
		if (CUTS(i) == "")
			exp_tree->Draw((EXP_VARS(i) + " >> hist").c_str(), "", "goff");
		else
			exp_tree->Draw((EXP_VARS(i) + " >> hist").c_str(), (EXP_VARS(i) + CUTS(i)).c_str(), "goff");
		TH1D	*exp_hist = (TH1D*)gDirectory->Get("hist"); // grab hist
		exp_hist->SetLineColor(kBlue); // set color
		comparison_hist->Add(exp_hist);	// add hist to stack

		if (CUTS(i) == "")
			sim_tree->Draw((SIM_VARS(i) + " >> hist2").c_str(), "", "goff");
		else
			sim_tree->Draw((SIM_VARS(i) + " >> hist2").c_str(), (SIM_VARS(i) + CUTS(i)).c_str(), "goff");
		TH1D	*sim_hist = (TH1D*)gDirectory->Get("hist2");
		sim_hist->SetLineColor(kRed);
		comparison_hist->Add(sim_hist);

		comparison_hist->Draw("NOSTACK"); // don't stack them
		comparison_hist->GetXaxis()->SetLimits(LOW_LIM(i), UP_LIM(i));
	
		if (NUM_VARS == 1)
			c1.Print("c1.pdf");
		else if (i == 0)
			c1.Print("c1.pdf("); // keep pdf open
		else if (i == NUM_VARS - 1)
			c1.Print("c1.pdf)"); // close pdf
		else
			c1.Print("c1.pdf");

		delete comparison_hist;
	}
}
