/*
 *  This file is part of BGX, the Bayesian Gene eXpression program.
 *  Copyright (c) 2003-2004  Graeme Ambler <graeme@ambler.me.uk>
 *                2005-2006  Ernest Turro <ernest.turro@ic.ac.uk>
 *
 *  BGX is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License, version 2, as
 *  published by the Free Software Foundation.
 *
 *  BGX is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <iostream>
#ifndef __linux__
#include <fstream>
#endif
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include "bgx.hh"

#ifdef USING_R
//  #include <R.h> // for flushing console, allowing user interrupts, and printing to console
  #include <Rcpp.h>
  #include <Rinternals.h>
  #include <R_ext/Rdynload.h>
  extern "C" {
  #if ( defined(HAVE_AQUA) || defined(WIN32) )
    #define FLUSH {R_FlushConsole(); R_ProcessEvents();}
  #else
    #define FLUSH R_FlushConsole();
  #endif
  #ifdef WIN32 // Win32 GUI doesn't handle \r properly. Use \b's.
    #define CARRIAGERETURN "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
  #else
    #define CARRIAGERETURN "\r"
  #endif
  #define PRINTFERR(...) Rprintf((char*) __VA_ARGS__)
  /* Register routines, allocate resources. */
  #define R_NO_REMAP
  }
#else
  #define FLUSH fflush(stdout);
  #define PRINTFERR(...) fprintf(stderr, __VA_ARGS__)
  #define CARRIAGERETURN "\r"
#endif

#ifdef __linux__
#include <fstream>
#endif
using namespace std;

enum { MINIMAL, TRACE, ALL}; 

int main(int argc, const char* argv[])
{
  int samples,conditions,probes,genes,numberOfCategories,numberOfGenesToWatch,seed,iter,burnin,
    output,*samplesets,*probesets,*categories,*unknownProbeSeqs, 
    numberOfUnknownProbeSeqs, *genesToWatch, *firstProbeInEachGeneToWatch, subsample, adaptive, batch_size;
  double s_jmp,h_jmp,mu_jmp,tau_jmp,lambda_jmp,eta_jmp,*pm,*mm, optimalAR;
  char *dirname, *basepath;

  if(argc<2){
    PRINTFERR("No input file specified\n");
    return 1;
  }
  ifstream infile(argv[1]);
  if(!infile){
    PRINTFERR("Cannot open input file\n");
    return 2;
  }
  
  samples = conditions = probes = genes = seed = iter = burnin = numberOfCategories = numberOfGenesToWatch = numberOfUnknownProbeSeqs 
    = adaptive = batch_size = -1;
  s_jmp = h_jmp = mu_jmp = tau_jmp = lambda_jmp = eta_jmp = optimalAR = -1.0;
  
  string samplesets_file="";
  string probesets_file="";
  string categories_file="";
  string unknownProbeSeqs_file="";
  string PM_file="";
  string MM_file="";
  string genesToWatch_file=""; 
  string firstProbeInEachGeneToWatch_file="";
  char sampleNames_[1024]; // will hold the names of the CEL files as one C string
  string affinityPlotFile= "";
  string geneNamesFile= "";
  
  string temp_str;
  while(infile >> temp_str){
    if(temp_str=="samples") infile >> samples;
    else if(temp_str=="conditions") infile >> conditions;
    else if(temp_str=="probes") infile >> probes;
    else if(temp_str=="genes") infile >> genes;
    else if(temp_str=="numberOfCategories") infile >> numberOfCategories;
    else if(temp_str=="numberOfGenesToWatch") infile >> numberOfGenesToWatch;
    else if(temp_str=="SampleSets") infile >> samplesets_file;
    else if(temp_str=="ProbeSets") infile >> probesets_file;
    else if(temp_str=="Categories") infile >> categories_file;
    else if(temp_str=="UnknownProbeSeqs") infile >> unknownProbeSeqs_file;
    else if(temp_str=="numberOfUnknownProbeSeqs") infile >> numberOfUnknownProbeSeqs;
    else if(temp_str=="genesToWatch") infile >> genesToWatch_file;
    else if(temp_str=="firstProbeInEachGeneToWatch") infile >> firstProbeInEachGeneToWatch_file;
    else if(temp_str=="PM") infile >> PM_file;
    else if(temp_str=="MM") infile >> MM_file;
    else if(temp_str=="seed") infile >> seed;
    else if(temp_str=="sweeps") infile >> iter;
    else if(temp_str=="burn-in") infile >> burnin;
    else if(temp_str=="S_jmp") infile >> s_jmp;
    else if(temp_str=="H_jmp") infile >> h_jmp;
    else if(temp_str=="Mu_jmp") infile >> mu_jmp;
    else if(temp_str=="Tau_jmp") infile >> tau_jmp;
    else if(temp_str=="Lambda_jmp") infile >> lambda_jmp;
    else if(temp_str=="Eta_jmp") infile >> eta_jmp;
    else if(temp_str=="Adaptive") infile >> adaptive;
    else if(temp_str=="BatchSize") infile >> batch_size;
    else if(temp_str=="OptimalAR") infile >> optimalAR;
    else if(temp_str=="Output"){
      infile >> temp_str;
      output = MINIMAL; // minimal is default
      if(temp_str=="trace") output=TRACE;
      else if(temp_str=="all") output=ALL;
    } else if(temp_str=="CELfiles") infile.getline(sampleNames_, 1024);
    else if(temp_str=="affinityPlotFile") infile >> affinityPlotFile;
    else if(temp_str=="geneNamesFile") infile >> geneNamesFile;
    else if(temp_str[0]=='#') getline(infile,temp_str);
    else{
        PRINTFERR("Malformed input file at unrecognised token %s\n", temp_str.c_str());
	      return 3;
    }
  }

  if(samples==-1){
    PRINTFERR("Number of samples not specified in input file\n");
    return 11;
  }
  if(conditions==-1){
    PRINTFERR("Number of conditions not specified in input file\n");
    return 11;
  }
  if(probes==-1){
    PRINTFERR("Number of probes not specified in input file\n");
    return 11;
  }
  if(genes==-1){
    PRINTFERR("Number of genes not specified in input file\n");
    return 11;
  }
  if(numberOfCategories==-1){
    PRINTFERR("Number of categories not specified in input file\n");
    return 11;
  }
  if(numberOfGenesToWatch==-1){
    PRINTFERR("Number of genes to watch not specified in input file\n");
    return 11;
  }
  if(numberOfUnknownProbeSeqs==-1){
    PRINTFERR("Number of unknown probe sequences not specified in input file\n");
    return 11;
  }
  if(samplesets_file==""){
    PRINTFERR("SampleSets file not specified in input file\n");
    return 11;
  }
  if(probesets_file==""){
    PRINTFERR("ProbeSets file not specified in input file\n");
    return 11;
  }
  if(categories_file==""){
    PRINTFERR("categories file not specified in input file\n");
    return 11;
  }
  if(unknownProbeSeqs_file==""){
    PRINTFERR("unknown probe sequences file not specified in input file\n");
    return 11;
  }
  if(genesToWatch_file==""){
    PRINTFERR("genesToWatch file not specified in input file\n");
    return 11;
  }
  if(firstProbeInEachGeneToWatch_file==""){
    PRINTFERR("firstProbeInEachGeneToWatch file not specified in input file\n");
    return 11;
  }
  if(PM_file==""){
    PRINTFERR("PM file not specified in input file\n");
    return 11;
  }
  if(MM_file==""){
    PRINTFERR("MM file not specified in input file\n");
    return 11;
  }
  if(seed==-1){
    PRINTFERR("Random number seed not specified in input file\n");
    return 11;
  }
  if(iter==-1){
    PRINTFERR("Number of sampling sweeps not specified in input file\n");
    return 11;
  }
  if(burnin==-1){
    PRINTFERR("Number of burn-in sweeps not specified in input file\n");
    return 11;
  }
  if(s_jmp==-1){
    PRINTFERR("Spread of jumps for RWM on S not specified in input file\n");
    return 11;
  }
  if(h_jmp==-1){
    PRINTFERR("Spread of jumps for RWM on H not specified in input file\n");
    return 11;
  }
  if(mu_jmp==-1){
    PRINTFERR("Spread of jumps for RWM on Mu not specified in input file\n");
    return 11;
  }
  if(tau_jmp==-1){
    PRINTFERR("Spread of jumps for RWM on Tau not specified in input file\n");
    return 11;
  }
  if(lambda_jmp==-1){
    PRINTFERR("Spread of jumps for RWM on Lambda not specified in input file\n");
    return 11;
  }
  if(eta_jmp==-1){
    PRINTFERR("Spread of jumps for RWM on Eta not specified in input file\n");
    return 11;
  }
  if(adaptive==-1){
    PRINTFERR("You have not specified whether to use adaptive MCMC\n");
    return 11;
  }
  if(adaptive){
    if(batch_size==-1 || optimalAR==-1){
      PRINTFERR("You have not specified a batch size and/or optimal acceptance ratio\n");
      return 11;
    }
  }

  vector<char *> sampleNames;
  char *pch = strtok(sampleNames_, " ,");
  while(pch != NULL){
    sampleNames.push_back(pch);
    pch = strtok(NULL, " ,");
  }
  
  ifstream SampleSets(samplesets_file.c_str());
  if(!SampleSets.good()){
    PRINTFERR("Could not open the Sample Sets file\n");
    return 4;
  }
  samplesets = new int[conditions];
  for(int i=0; i<conditions; ++i)
    SampleSets >> samplesets[i];
  SampleSets.close();

  ifstream ProbeSets(probesets_file.c_str());
  if(!ProbeSets.good()){
    PRINTFERR("Could not open the Probe Sets file\n");
    return 5;
  }
  probesets = new int[genes];
  for(int i=0; i<genes; ++i)
    ProbeSets >> probesets[i];
  ProbeSets.close();

  ifstream Categories(categories_file.c_str());
  if(!Categories.good()){
    PRINTFERR("Could not open the Categories file\n");
    return 5;
  }
  categories = new int[probes];
  for(int i=0; i < probes; i++)
    Categories >> categories[i];
  Categories.close();

  ifstream UnknownProbeSeqs(unknownProbeSeqs_file.c_str());
  if(!UnknownProbeSeqs.good()){
    PRINTFERR("Could not open the Unknown Probe Sequences fil\n");
    return 5;
  }
  unknownProbeSeqs = new int[numberOfUnknownProbeSeqs];
  for(int i=0; i < numberOfUnknownProbeSeqs;i++)
    UnknownProbeSeqs >> unknownProbeSeqs[i];
  UnknownProbeSeqs.close();
 
  ifstream GenesToWatch(genesToWatch_file.c_str());
  if(!GenesToWatch.good()){
    PRINTFERR("Could not open the Genes To Watch file\n");
    return 5;
  }
  genesToWatch = new int[numberOfGenesToWatch];
  for(int i=0; i<numberOfGenesToWatch; ++i)
    GenesToWatch >> genesToWatch[i];
  GenesToWatch.close();

  ifstream FirstProbeInEachGeneToWatch(firstProbeInEachGeneToWatch_file.c_str());
  if(!FirstProbeInEachGeneToWatch.good()){
    PRINTFERR("Could not open the Probe Sets To Watch file\n");
    return 5;
  }
  firstProbeInEachGeneToWatch = new int[numberOfGenesToWatch];
  for(int i=0; i<numberOfGenesToWatch; ++i)
    FirstProbeInEachGeneToWatch >> firstProbeInEachGeneToWatch[i];
  FirstProbeInEachGeneToWatch.close();
 
  ifstream PM(PM_file.c_str());
  if(!PM.good()){
    PRINTFERR("Could not open the PM file\n");
    return 6;
  }
  ifstream MM(MM_file.c_str());
  if(!MM.good()){
    PRINTFERR("Could not open the MM file\n");
    return 7;
  }
  pm = new double[probes*samples];
  mm = new double[probes*samples];
  for(int i=0; i<probes*samples; ++i){
    PM >> pm[i];
    MM >> mm[i];
  }
  PM.close();
  MM.close();

  subsample=iter/1024;
  if(subsample<1) subsample=1;

  dirname = new char[100];
  basepath = new char[2];
  strcpy(basepath,"."); // We create the run directories in current directory

  bool adaptive_ = static_cast<bool>(adaptive);

  bgx(pm, mm, &samples, &conditions, &probes, &genes, &numberOfCategories, &numberOfGenesToWatch, samplesets, 
      probesets, categories, unknownProbeSeqs,&numberOfUnknownProbeSeqs, genesToWatch, 
      firstProbeInEachGeneToWatch, &iter, &burnin, &s_jmp, &h_jmp, &mu_jmp, &tau_jmp, &lambda_jmp, &eta_jmp,
      &adaptive_, &batch_size, &optimalAR,
      &subsample, &output, &dirname, &basepath, &seed, &sampleNames[0], &affinityPlotFile, &geneNamesFile);

  delete[] samplesets;
  delete[] probesets;
  delete[] genesToWatch;
  delete[] firstProbeInEachGeneToWatch;
  delete[] pm;
  delete[] mm;
  delete[] categories;
  delete[] unknownProbeSeqs;
  delete[] dirname;
  delete[] basepath;
  return 0;
}
