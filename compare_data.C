// This program compares simulated results to experimental data
//
// 
//	CUTS 

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TH1D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TMath.h"
#include <cstdlib>
#include <cstring>

#include "compare_data.h"

using namespace std;

// Real Runs: 12080 12083 13092 13133 13137 13854
// Dummy Runs: 13094 13136 13857
// run 13137 is broken

#define NORMFAC (737228000)
#define NUM_FILES 5
#define RUN_ALL 0
#define NUM_BINS 500
#define NUM_VARS 12
#define NUM_VARS_PRINT 11
#define NUM_PARAMS 5

// hist name, simulated data var name, experimental data variable name, lower bound, upper bound
const string	info[NUM_VARS][NUM_PARAMS] =
{
    "epsilon",          "epsilon", "H.kin.primary.epsilon",  "0",    "1",
    "Q^2",              "Q2",      "H.kin.primary.Q2",       "0",    "10",
    "W",                "W",       "H.kin.primary.W",        "0",    "2",
    "delta",            "hsdelta", "H.gtr.dp",               "-15",  "15",
    "x focal plane",    "hsxfp",   "H.dc.x_fp",              "-40",  "40",
    "y focal plane",    "hsyfp",   "H.dc.y_fp",              "-40",  "40",
    "xp focal plane",   "hsxpfp",  "H.dc.xp_fp",             "-.25", ".25",
    "yp focal plane",   "hsypfp",  "H.dc.yp_fp",             "-.25", ".25",
    "target hrz angle", "hsxptar", "H.gtr.th",               "-.1", ".1",
    "target vrt angle", "hsyptar", "H.gtr.ph",               "-.1",  ".1",
    "nu",               "nu",      "H.kin.primary.nu",       "0",    "7",
    "Momentum",         "hsdelta", "H.gtr.p",                "-15",  "15",
};
#define HIST_NAMES(i) ((info[i][0]).c_str()) // convers to cstring
#define SIM_VARS(i)   ((info[i][1]).c_str()) // convers to cstring
#define EXP_VARS(i)   ((info[i][2]).c_str()) // convers to cstring
#define LOW_LIM(i)    stod(info[i][3])	// converts to number
#define UP_LIM(i)     stod(info[i][4])	// converts to number

#define SIM_TREE_NAME "h10"
#define EXP_TREE_NAME "T"

#include <iostream>
#include <fstream>
#include <iomanip>

 int		grab_normfac(string file_name)
 {
 	ifstream		ins;
	
	ins.open(file_name);
	char		dump[1000];
	int			normfac = 0;

	for (size_t i = 0; i < 110; ++i)
		ins.getline(dump, 1000);

	if (strncmp("normfac", dump + 19, 7) != 0)
	{
		cout << "error in parsing " << file_name << endl;
		return (-1);
	}

	normfac = atoi(dump + 35) * pow(10, atoi(dump + 42) - 6);

	cout << normfac << endl;
	return (normfac);
 }

/*
 *	Funtion:	compare_data
 *				controlls program which compares data from the HMS to SimC.
 *	@param(run_number):		the run number to compare. by default it will compare
 *							all the runs.
 *	@param(pdf_name):		string with pdf name. (excluding ".pdf" extension.
 *	@return:				N/A
 *
 */
void 			compare_data(int run_number = RUN_ALL)
{
	int		runs[NUM_FILES] = {12080, 12083, 13133, 13136, 13854};
	double	moms[NUM_FILES] = {5.9, 5.9, 5.202, 5.202, 5.202}; // HMS spectrometer momentum settings
	int		normfac = 0;




	for (size_t i = 0; i < NUM_FILES; ++i)
	{
		if (run_number == RUN_ALL || run_number == runs[i])
		{
			normfac = grab_normfac("../sim/outfiles/run_" + to_string(runs[i]) + ".hist");
			if (normfac == -1)
				continue ;
			string	sim_file_name = "../sim/worksim/run_" + to_string(runs[i]) + ".root";
			string	exp_file_name = "hms_files/Pion_hms_replay_production_" + to_string(runs[i]) + "_-1.root";

			int	normfac = grab_normfac("../sim/outfiles/run_" + to_string(runs[i]) + ".hist");

			compare(sim_file_name, exp_file_name, "run_" + to_string(runs[i]) + ".pdf", moms[i], normfac);
		}
	}
}

/*
 *	Funtion:	compare
 *				controlls program which compares data from the HMS to SimC.
 *	@param(run_number):		the run number to compare. by default it will compare
 *							all the runs.
 *	@param(pdf_name):		string with pdf name. (excluding ".pdf" extension.
 *	@return:				N/A
 *
 */
void 	compare(string sim_root_file, string exp_root_file, string pdf_name, double momentum, int normfac)
{
	// loads the root files
	TFile	*sim_t_file = TFile::Open(sim_root_file.c_str());
	TFile	*exp_t_file = TFile::Open(exp_root_file.c_str());

	TTree	*sim_tree;
	TTree	*exp_tree;

	sim_t_file->GetObject("h10", sim_tree);
	exp_t_file->GetObject("T", exp_tree);

//	cout << sim_tree->GetEntries() << "   " << sim_tree->GetEntries("(W - .938) < .04") << endl;
	cout << exp_tree->GetEntries() << "   " << exp_tree->GetEntries("(H.kin.primary.W - .938) < .04") << endl;

	TTreeReader		*sim_reader = new TTreeReader(SIM_TREE_NAME, sim_t_file);
	TTreeReader		*exp_reader = new TTreeReader(EXP_TREE_NAME, exp_t_file);
	size_t			n_sim_entries_no_cut = sim_reader->GetEntries();

	// creating arrays for unchanging info needed on each loop
	bool			*exp_cuts;
	bool			*sim_cuts;
	Float_t			*weights;
	size_t			n_sim_entries_cut = 0;
	size_t			n_exp_entries_cut = 0;

	create_sim_cuts_and_weights(sim_reader, sim_cuts, weights, n_sim_entries_cut);
	create_exp_cuts(exp_reader, exp_cuts, n_exp_entries_cut);
	cout << "exp: " << n_exp_entries_cut << endl;
	cout << "sim: " << n_sim_entries_cut << endl;

	// loop over all tree variables to compare
	for (size_t i = 0; i < NUM_VARS_PRINT; ++i)
	{
		TCanvas		*c1 = new TCanvas(); // for saving drawings

		sim_reader->Restart();
		exp_reader->Restart();

		// class to plot multiple histograms together THStack(name, title)
		THStack	*comparison_hist = new THStack("hists", HIST_NAMES(i));

		// create pointers to the tree variables
		TTreeReaderValue<Double_t> exp_var = {*exp_reader, EXP_VARS(i)};
		TTreeReaderValue<Float_t> sim_var = {*sim_reader, SIM_VARS(i)};

		// loading in simulated histogram (needs to handle a weight)
		size_t j = 0;
		size_t k = 0;
		TH1D		*sim_hist = new TH1D(HIST_NAMES(i), HIST_NAMES(i), NUM_BINS, LOW_LIM(i), UP_LIM(i));
		while (sim_reader->Next())
		{
			if (sim_cuts[j])
				sim_hist->Fill((*sim_var), weights[i]*normfac/n_sim_entries_no_cut);
			//	sim_hist->Fill((*sim_var));
			++j;
			sim_hist->Fill((*sim_var), weights[i]*normfac/n_sim_entries_no_cut);

		}
		sim_hist->SetLineColor(kRed);

		// loading in experimental histogram
		j = 0;
		TH1D		*exp_hist = new TH1D(HIST_NAMES(i), HIST_NAMES(i), NUM_BINS, LOW_LIM(i), UP_LIM(i));
		while (exp_reader->Next())
		{
			if (exp_cuts[j])
				exp_hist->Fill(*exp_var);
			++j;
		}
		exp_hist->SetLineColor(kBlue);

		// add hists to stack, draw, and zoom
		comparison_hist->Add(exp_hist);
		comparison_hist->Add(sim_hist);
		comparison_hist->Draw("NOSTACK");
		comparison_hist->GetXaxis()->SetRangeUser(LOW_LIM(i), UP_LIM(i));

		// finding and drawing the peaks
		double		sim_peak = sim_hist->GetBinCenter(sim_hist->GetMaximumBin());
		double		exp_peak = exp_hist->GetBinCenter(exp_hist->GetMaximumBin());
		TLine		*exp_max = new TLine(exp_peak, 0, exp_peak, exp_hist->GetMaximum() * 1.2);
		TLine		*sim_max = new TLine(sim_peak, 0, sim_peak, sim_hist->GetMaximum() * 1.2);
		sim_max->Draw();
		exp_max->Draw();

		// creating and drawing a legend for quick info
		TLegend		*leg = new TLegend();
		string		num_events("# events: ");
		leg->AddEntry(exp_hist, (num_events + to_string(n_exp_entries_cut) + " (exp)").c_str());
		leg->AddEntry(sim_hist, (num_events + to_string(n_sim_entries_cut) + " (sim)").c_str());
		string		peak("peak: ");
		leg->AddEntry(exp_hist, (peak + to_string(exp_peak)).c_str());
		leg->AddEntry(sim_hist, (peak + to_string(sim_peak)).c_str());
		leg->Draw();

		// save drawings to a file
		if (NUM_VARS_PRINT == 1) // edge case
			c1->Print(pdf_name.c_str());
		else if (i == 0)
			c1->Print((pdf_name + "(").c_str()); // keep pdf open
		else if (i == NUM_VARS_PRINT - 1)
			c1->Print((pdf_name + ")").c_str()); // close pdf
		else
			c1->Print(pdf_name.c_str());

		// clean up
		delete comparison_hist;
		delete exp_max;
		delete sim_max;
		delete leg;
		delete sim_hist;
		delete exp_hist;
		delete c1;
	}
}

void		create_sim_cuts_and_weights(TTreeReader *reader, bool *&cuts, Float_t *&weights, size_t &n_entries_cut)
{
	cuts = new bool [reader->GetEntries()];
	weights = new Float_t [reader->GetEntries()];

	size_t i = 0;

	TTreeReaderValue<Float_t> W = {*reader, "W"};
	TTreeReaderValue<Float_t> weight = {*reader, "Weight"};
	TTreeReaderValue<Float_t> delta = {*reader, "hsdelta"};
//	TTreeReaderValue<Float_t> hsxpfp = {*reader, "hsxpfp"};
//	TTreeReaderValue<Float_t> hsypfp = {*reader, "hsypfp"};

	while (reader->Next())
	{
		if (abs(*W - 0.938) < 0.040 &&
			abs(*delta) < 8)
//			abs(*hsxpfp) < 0.08 &&
//			abs(*hsypfp) < 0.045)
		{
			cuts[i] = true;
			++n_entries_cut;
		}
		else
		{
			cuts[i] = false;
		}
		weights[i] = *weight;
		++i;
	}
}

void		create_exp_cuts(TTreeReader *reader, bool *&cuts, size_t &n_entries_cut)
{
	cuts = new bool [reader->GetEntries()];

	size_t i = 0;

	TTreeReaderValue<Double_t> W = {*reader, "H.kin.primary.W"};
	TTreeReaderValue<Double_t> beta = {*reader, "H.gtr.beta"};
	TTreeReaderValue<Double_t> npe = {*reader, "H.cer.npeSum"};
	TTreeReaderValue<Double_t> dp = {*reader, "H.gtr.dp"};
//	TTreeReaderValue<Double_t> xp_fp = {*reader, "H.dc.xp"};
//	TTreeReaderValue<Double_t> yp_fp = {*reader, "H.dc.yp"};

	cout << "entries from reader: " << reader->GetEntries() << endl;
	int j = 0;
	while (reader->Next())
	{
		++j;
		if (abs(*W - 0.938) < 0.040 &&
			abs(*beta - 1) < 0.3 && 
			abs(*dp) < 8 &&
//			abs(*xp_fp) < 0.08 &&
//			abs(*yp_fp) < 0.045 &&
			*npe > 1.5)
		{
			cuts[i] = true;
			++n_entries_cut;
		}

		else
		{
			cuts[i] = false;
		}
		++i;
	}
	cout << "entries from reader next: " << j << endl;
}
		// handling momentum special case
//		if (strcmp(HIST_NAMES(i), "Momentum")) // special loop for handling simulated momentum
//		{
//			while (sim_reader.Next())
//				sim_hist->Fill((((*sim_var)/100)+1)*momtm, (*sim_weight)*NORMFAC/n_sim_entries);
//		}
//		else
//		{
