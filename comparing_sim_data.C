// This program draws the pre and post cut histograms on the same plot with
// different colors

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
static const char	*hist_names[] = {	"x focal plane",
										"y focal plane",
										"xp focal plane",
										"yp focal plane",
										"nu",
										"Q^2",
										"W",
										"epsilon"};

//"hsdelta",
//"hsyptar",
//"hsxptar",
//"hsytar"
static const char	*sim_var_names[] = {"hsxfp",
										"hsyfp",
										"hsxpfp",
										"hsypfp",
										"nu",
										"Q2",
										"W",
										"epsilon"};

//"H.dc.x"
//"H.dc.y"
static const char	*exp_var_names[] = {"H.dc.x_fp",
										"H.dc.y_fp",
										"H.dc.xp_fp",
										"H.dc.yp_fp",
										"H.kin.primary.nu",
										"H.kin.primary.Q2",
										"H.kin.primary.W",
										"H.kin.primary.epsilon"};

static THStack	*load_comparison_hist(TTree *exp_tree, TTree *sim_tree, const char *hist_name, string exp_var, string sim_var)
{
	// draw var with "graph off". " >> hist" writes to a histogram in the gDir
	// middle string is for cuts. needs ">> hist" too if a cut is present
	exp_tree->Draw((exp_var + " >> hist").c_str(), "", "goff");
	TH1D	*exp_hist = (TH1D*)gDirectory->Get("hist"); // grab hist

	sim_tree->Draw((sim_var + " >> hist2").c_str(), "", "goff");
	TH1D	*sim_hist = (TH1D*)gDirectory->Get("hist2");

	// set color
	exp_hist->SetLineColor(kBlue);
	sim_hist->SetLineColor(kRed);

	// class to plot multiple histograms together THStack(name, title)
	THStack	*hs = new THStack("hists", hist_name);

	// add hists to the stack and plot together
	hs->Add(exp_hist);
	hs->Add(sim_hist);
	return (hs);
}

void 			comparing_sim_data()
{
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
		THStack		*hist = load_comparison_hist(exp_tree, sim_tree, hist_names[i], exp_var_names[i], sim_var_names[i]);

		hist->Draw("NOSTACK"); // don't stack them
		if (i == 0)
			c1.Print("c1.pdf(");
		else if (i == NUM_VARS - 1)
			c1.Print("c1.pdf)");
		else
			c1.Print("c1.pdf");
	}
}
