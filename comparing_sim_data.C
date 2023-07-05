// This program draws the pre and post cut histograms on the same plot with
// different colors

#include "TFile.h"
#include "TH1D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#define SYM_FILE "../sim/worksim/test.root"

#define EXP_FILE "hms_files/Pion_hms_replay_production_12080_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_12083_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13092_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13094_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13133_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13136_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13854_-1.root"
// #define EXP_FILE "hms_files/Pion_hms_replay_production_13857_-1.root"

void 			comparing_sim_data()
{
	// loads the root files
	TFile		*exp_data = TFile::Open(EXP_FILE);
	TFile		*sym_data = TFile::Open(SYM_FILE);

	// load tree from files
	TTree		*exp_tree;
	exp_data->GetObject("T", exp_tree);

	TTree		*sym_tree;
	sym_data->GetObject("h10", sym_tree);

	// "draw" desired variable and save that drawing to the "hist" histogram
	// stored in the gDirectory. 
	// middle string is for cuts. needs ">> hist" too if a cut is present
	// goff will keep it from actually drawing
	exp_tree->Draw("H.dc.x_fp >> hist", "", "goff");
	// grab the histogram from the gDir
	TH1D	*H_dc_x_fp_hist = (TH1D*)gDirectory->Get("hist");

	exp_tree->Draw("hsxfp >> hist2", "", "goff");
	TH1D	*hsxfp_hist = (TH1D*)gDirectory->Get("hist2");

//	// color and print
//	H_dc_x_fp_hist->SetLineColor(kRed);
//	H_dc_x_fp_hist->Draw();

	// set color
	H_dc_x_fp_hist->SetLineColor(kRed);
	hsxfp_hist->SetLineColor(kBlue);

	// class to plot multiple histograms together THStack(name, title)
	THStack	*hs = new THStack("hists", "Run 12080 yptar comparison");

	// add hists to the stack and plot together
	hs->Add(H_dc_x_fp_hist);
	hs->Add(hsxfp_hist);
	hs->Draw("NOSTACK"); // don't stack them
}
