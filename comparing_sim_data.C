// This program compares simulated results to experimental data

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TMath.h"

using namespace std;

#define SIM_FILE "../sim/worksim/run_13133.root"
#define EXP_FILE "hms_files/Pion_hms_replay_production_13133_-1.root"

// All files
// #define EXP_FILE "hms_files/Pion_hms_replay_production_12080_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_12083_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13092_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13133_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13854_-1.root"

// #define EXP_FILE "hms_files/Pion_hms_replay_production_13094_-1.root" DUMMY
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13136_-1.root" DUMMY
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13857_-1.root" DUMMY

#define NUM_BINS 500
#define NUM_VARS 11
#define NUM_VARS_PRINT 11
#define NUM_PARAMS 5
#define HIST_NAMES(i)	info[i][0]
#define SIM_VARS(i)	info[i][1]
#define EXP_VARS(i)	info[i][2]
#define LOW_LIM(i)	stod(info[i][3])	// also converts to number
#define UP_LIM(i)	stod(info[i][4])	// also converts to number

// root 'comparing_sim_data("filename") <- to run from commandline
void 			comparing_sim_data(string pdf_name = "c1")
{
	// hist name, simulated data's variable name, experimental data's variable name, min cut, max cut, lower bound, upper bound
	string	info[NUM_VARS][NUM_PARAMS] =
	{
        "W",                "W",       "H.kin.primary.W",       "0",    "2",
        "x focal plane",    "hsxfp",   "H.dc.x_fp",             "-40",  "40",
        "y focal plane",    "hsyfp",   "H.dc.y_fp",             "-40",  "40",
        "xp focal plane",   "hsxpfp",  "H.dc.xp_fp",            "-.25", ".25",
        "yp focal plane",   "hsypfp",  "H.dc.yp_fp",            "-.25", ".25",
        "target hrz angle", "hsxptar", "H.gtr.ph",              "-1",   "1",
        "target vrt angle", "hsyptar", "H.gtr.th",              "-1.5", "1.5",
        "delta",            "hsdelta", "H.gtr.dp",              "-15",  "15",
        "nu",               "nu",      "H.kin.primary.nu",      "0",    "10",
        "Q^2",              "Q2",      "H.kin.primary.Q2",      "0",    "10",
        "epsilon",          "epsilon", "H.kin.primary.epsilon", "0",    "1"
	};

	// string	exp_cut = "";
	// string	sim_cut = "";
	string	exp_cut = "(H.kin.primary.W - 0.938) < 0.04";
	string	sim_cut = "(W - 0.938) < 0.04";

	// loads the root files
	TFile		*exp_data = TFile::Open(EXP_FILE);
	TFile		*sim_data = TFile::Open(SIM_FILE);

	// load trees from files
	TTree		*exp_tree;
	exp_data->GetObject("T", exp_tree);

	TTree		*sim_tree;
	sim_data->GetObject("h10", sim_tree);

	TCanvas		c1; // for saving drawings
	for (size_t i = 0; i < NUM_VARS_PRINT; ++i)
	{
		// class to plot multiple histograms together THStack(name, title)
		THStack	*comparison_hist = new THStack("hists", (HIST_NAMES(i)).c_str());

		// draw vars with "graph off". " >> hist" writes to a histogram in the gDir
		exp_tree->Draw((EXP_VARS(i) + " >> hist(NUM_BINS)").c_str(), exp_cut.c_str(), "goff");
		sim_tree->Draw((SIM_VARS(i) + " >> hist2(NUM_BINS)").c_str(), sim_cut.c_str(), "goff");

		// grab hists
		TH1D	*exp_hist = (TH1D*)gDirectory->Get("hist");
		TH1D	*sim_hist = (TH1D*)gDirectory->Get("hist2");

		// set colors
		exp_hist->SetLineColor(kBlue);
		sim_hist->SetLineColor(kRed);

		// add hists to stack
		comparison_hist->Add(exp_hist);
		comparison_hist->Add(sim_hist);

		// draw
		comparison_hist->Draw("NOSTACK");
		comparison_hist->GetXaxis()->SetLimits(LOW_LIM(i), UP_LIM(i));
		c1.SetLogy();		

		// finding and drawing the peaks
		double		sim_peak = sim_hist->GetBinCenter(sim_hist->GetMaximumBin());
		double		exp_peak = exp_hist->GetBinCenter(exp_hist->GetMaximumBin());
		TLine		*exp_max = new TLine(exp_peak, 0, exp_peak, exp_hist->GetMaximum() * 1.2);
		TLine		*sim_max = new TLine(sim_peak, 0, sim_peak, sim_hist->GetMaximum() * 1.2);
		sim_max->Draw();
		exp_max->Draw();

		TLegend		*leg = new TLegend();
		string		num_events("# events: ");
		leg->AddEntry(exp_hist, (num_events + to_string((int)exp_hist->GetEntries()) + " (exp)").c_str());
		leg->AddEntry(sim_hist, (num_events + to_string((int)sim_hist->GetEntries()) + " (sim)").c_str());

		string		peak("peak: ");
		leg->AddEntry(exp_hist, (peak + to_string(exp_peak)).c_str());
		leg->AddEntry(sim_hist, (peak + to_string(sim_peak)).c_str());

		leg->Draw();

		if (NUM_VARS_PRINT == 1) // edge case
			c1.Print((pdf_name + ".pdf").c_str());
		else if (i == 0)
			c1.Print((pdf_name + ".pdf(").c_str()); // keep pdf open
		else if (i == NUM_VARS - 1)
			c1.Print((pdf_name + ".pdf)").c_str()); // close pdf
		else
			c1.Print((pdf_name + ".pdf").c_str());

		delete comparison_hist;
	}
}
