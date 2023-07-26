// This program compares simulated results to experimental data

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TMath.h"

using namespace std;

// Real Runs: 12080 12083 13092 13133 13137 13854
// Dummy Runs: 13094 13136 13857
// run 13137 is broken

#define NUM_FILES 5
#define RUN_ALL 0
#define NUM_BINS 500
#define NUM_VARS 12
#define NUM_VARS_PRINT 1
#define NUM_PARAMS 5
#define HIST_NAMES(i) info[i][0]
#define SIM_VARS(i)   info[i][1]
#define EXP_VARS(i)   info[i][2]
#define LOW_LIM(i)    stod(info[i][3])	// also converts to number
#define UP_LIM(i)     stod(info[i][4])	// also converts to number

// root 'comparing_sim_data("filename") <- to run from commandline
static void 	compare(string run_number, string pdf_name)
{
	string	moms((stoi(run_number) > 12083) ? "5.202" : "5.9"); // momentums from logbook

	// hist name, simulated data's variable name, experimental data's variable name, min cut, max cut, lower bound, upper bound
	string	info[NUM_VARS][NUM_PARAMS] =
	{
        "P",                "((hsdelta/100) + 1)*" + moms, "((H.gtr.dp/100)+1)*" + moms,      "0",  "10",
        "W",                "W",               "H.kin.primary.W",       "0",    "2",
        "delta",            "hsdelta",         "H.gtr.dp",              "-15",  "15",
        "x focal plane",    "hsxfp",           "H.dc.x_fp",             "-40",  "40",
        "y focal plane",    "hsyfp",           "H.dc.y_fp",             "-40",  "40",
        "xp focal plane",   "hsxpfp",          "H.dc.xp_fp",            "-.25", ".25",
        "yp focal plane",   "hsypfp",          "H.dc.yp_fp",            "-.25", ".25",
        "target hrz angle", "hsxptar",         "H.gtr.ph",              "-1",   "1",
        "target vrt angle", "hsyptar",         "H.gtr.th",              "-1.5", "1.5",
        "nu",               "nu",              "H.kin.primary.nu",      "0",    "10",
        "Q^2",              "Q2",              "H.kin.primary.Q2",      "0",    "10",
        "epsilon",          "epsilon",         "H.kin.primary.epsilon", "0",    "1"
	};

	string	exp_cut = "(H.kin.primary.W - 0.938) < 0.04";
	string	sim_cut = "(W - 0.938) < 0.04";

	// loads the root files
	string	sim_file_path("../sim/worksim/run_");
	string	exp_file_path("hms_files/Pion_hms_replay_production_");
	TFile	*exp_data = TFile::Open((exp_file_path + run_number + "_-1.root").c_str());
	TFile	*sim_data = TFile::Open((sim_file_path + run_number + ".root").c_str());

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

void 			comparing_sim_data(string run_number, string pdf_name = "c1")
{
	string	runs[NUM_PARAMS] = {"12080", "12083", "13133", "13136", "13854"};
	if (stod(run_number) == RUN_ALL)
	{
		for(size_t i = 0; i < NUM_FILES; ++i)
		{
			compare(runs[i], "run_" + runs[i]);
		}
	}
	else
		compare(run_number, pdf_name);
}
