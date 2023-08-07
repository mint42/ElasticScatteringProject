#ifndef COMPARE_DATA_H
# define COMPARE_DATA_H

void 	compare(string sim_root_file, string exp_root_file, string pdf_name, double momentum, int normfac);
void	create_exp_cuts(TTreeReader *reader, bool *&cuts, size_t &n_entries_cut);
void	create_sim_cuts_and_weights(TTreeReader *reader, bool *&cuts, Float_t *&weights, size_t &n_entries_cut);

#endif
