#include "DrawTGraph.C"

TFile *outFile = new TFile("../CompareResult/VinhPID_test_TProfile_pt_filled.root","recreate");
bool bDrawPlots1040 = 0;
bool drawDistributions = 0;
bool bSaveCanvas = 0;
double Covariance(TProfile *const &hcovXY, TProfile *const &hX, TProfile *const &hY){
  double statsXY[6], statsX[6], statsY[6];
  double meanXY, meanX, meanY, sumWX, sumWY;
  hcovXY -> GetStats(statsXY);
  hX -> GetStats(statsX);
  hY -> GetStats(statsY);
  
  double mSumWXY = statsXY[0];
  sumWX = statsX[0];
  sumWY = statsY[0];

  meanXY = hcovXY -> GetBinContent(1);
  meanX = hX -> GetBinContent(1);
  meanY = hY -> GetBinContent(1);
  // mVal = (meanXY-meanX*meanY)/(1-mSumW/sumWX/sumWY); // Cov(x,y) // formula (C.12)
  double mVal = (meanXY-meanX*meanY)/(sumWX*sumWY/mSumWXY-1.); // Cov(x,y)/(sumWX*sumWY/sumWXY)
  return mVal;
}

struct term{
  term(){
    mVal = 0;
    mSumW = 0;
    mNeff = 0;
    mS2 = 0;
    mMSE = 0;
  }
  term(TProfile *const &pr){
    double stats[6];
    pr->GetStats(stats);
    mSumW = stats[0];
    double sumW2 = stats[1];
    
    mNeff = pr -> GetBinEffectiveEntries(1); // Number of effective entries
    // mNeff = mSumW*mSumW/sumW2;
    mVal = pr -> GetBinContent(1);
    pr -> SetErrorOption("s");
    double stdevW = pr -> GetBinError(1);
    mS2 = stdevW*stdevW/(1-sumW2/mSumW/mSumW); // formula (C.3)
    // mS2 = pr -> GetBinError(1);
    mMSE = mS2/mNeff;
  };
 public: 
  double mVal; // weithted mean value
  double mSumW; // sum of weights
  double mNeff; // Number of effective entries
  double mS2; // Unbiased estimator for the root of variance, (C.3) in Ante's dissertation
  double mMSE; // Mean squared error of mean, https://en.wikipedia.org/wiki/Mean_squared_error

};

// static const double maxpt = 3.0; // max pt
// static const double minpt = 0.1; // min pt
// static const int npt = 8; // 0.5 - 3.6 GeV/c - number of pT bins
// static const double bin_pT[npt+1]={0.1, 0.3, 0.6, 0.9, 1.2, 1.5, 1.9, 2.4, 3.};

static const int npid = 8; // charged hadrons, pions, kaons, protons
static const int nmethod = 3; // 2QC, 4QC, EP

static const int npt = 9; // 0.5 - 3.6 GeV/c - number of pT bins
static const double bin_pT[npt+1]={0.2,0.4,0.6,0.8,1.,1.2,1.5,1.8,2.5,3.};
static const double maxpt = 2.5; // max pt
static const double minpt = 0.2; // min pt

static const int ncent = 8; // 0-80 %
static const double bin_cent[ncent] = {5,15,25,35,45,55,65,75};
static const double Bin_cent[ncent+1]={0.,10.,20.,30.,40.,50.,60.,70.,80.};
static const double bin_centE[ncent] = {0};

const std::vector<double> pt_binning = {0.2, 0.4, 0.6, 0.8, 1., 1.2, 1.5, 1.8, 2.5, 3.};
// const std::vector<double> pt_binning = {0.2, 0.6, 1., 1.5, 2.5, 3.}; // for kaons specifically
const int n_pt_bins = pt_binning.size() - 1;

const std::vector<TString> pidNames = {"hadron_pos", "pion_pos", "kaon_pos", "proton_pos", "hadron_neg", "pion_neg", "kaon_neg", "proton_neg"};
const std::vector<TString> pidFancyNames = {"h+", "#pi+", "K+", "p", "h-", "#pi-", "K-", "#bar{p}"};

TProfile *prV22int[ncent][npid], *prV24int[ncent][npid], *prV2EPint[ncent][npid]; // TProfile for integrated flow 
TProfile *prV22dif1040[npt][npid], *prV24dif1040[npt][npid], *prV2EPdif1040[npt][npid], *pt1040[npt][npid]; // TProfile for differential flow of 10-40% centrality bin

void v2plot_differential_flow(){
  TFile *inFile = new TFile("../ROOTFile/PID_test_TProfile_pt_filled.root","read");
  // TFile *inFile = new TFile("../ROOTFile/PID_PeterCode.root","read");


  // Temporary variables
  char hname[800]; // histogram hname
  double stats[6]; // stats of TProfile
  // char analysis[20]={"pure"};

  // Input hist
  TProfile *hv22[ncent];        // profile <<2>> from 2nd Q-Cumulants
  TProfile *hv24[ncent];        // profile <<4>> from 4th Q-Cumulants
  TProfile *hPT[ncent][npt][npid];       // profile pt 
  TProfile *hv22pt[ncent][npt][npid];    // profile <<2'>> from 2nd Q-Cumulants
  TProfile *hv24pt[ncent][npt][npid];    // profile <<4'>> from 4th Q-Cumulants
  TProfile *hcov24[ncent];       // <2>*<4>
  TProfile *hcov22prime[ncent][npt][npid]; // <2>*<2'>
  TProfile *hcov24prime[ncent][npt][npid]; // <2>*<4'>
  TProfile *hcov42prime[ncent][npt][npid]; // <2>*<4'>
  TProfile *hcov44prime[ncent][npt][npid]; // <4>*<4'>
  TProfile *hcov2prime4prime[ncent][npt][npid]; // <2'>*<4'>
  // TProfile *hv2EP[ncent][npt][npid];	  // elliptic flow from EP method
  TProfile2D *hv2EP[npid];	  // elliptic flow from EP method
  // TProfile *HRes[ncent];
  TProfile *pv2EP[ncent][npid];	  // elliptic flow from EP method
  // OUTPUT
  TGraphErrors *grDifFl[3][ncent][npid];    // v2(pt); 3 = {2QC, 4QC, EP}
  TGraphErrors *grDifFl1040[3][npid];
  
  // Get TProfile histograms from ROOTFile

  for (int id=0;id<npid;id++){
    hv2EP[id]= (TProfile2D*)inFile->Get(Form("hv2EP_%i",id));
  }
  for (int id=0;id<npid;id++){
    for (int icent=0;icent<ncent;icent++){
      pv2EP[icent][id] = (TProfile*)(hv2EP[id])->ProfileX(Form("%s_cent_%i",hv2EP[id]->GetName(),icent),icent+1,icent+1);
      pv2EP[icent][id] = (TProfile *)pv2EP[icent][id]->Rebin(n_pt_bins, Form("rebinned_%s", pv2EP[icent][id]->GetName()), &pt_binning[0]);
    }
  }

  for (int icent=0; icent<ncent; icent++){ // loop over centrality classes
    // HRes[icent] = (TProfile*)inFile->Get(Form("HRes_%i",icent));
    hv22[icent] = (TProfile*)inFile->Get(Form("hv22_%i",icent));
    hv24[icent] = (TProfile*)inFile->Get(Form("hv24_%i",icent));
    hcov24[icent] = (TProfile*)inFile->Get(Form("hcov24_%i",icent));
    for(int ipt=0; ipt<npt; ipt++){ // loop over pt bin
      for (int id=0;id<npid;id++){
        // hv2EP[icent][ipt][id]=(TProfile*)inFile->Get(Form("hv2EP_%i_%i_%i",icent,ipt,id));
        hPT[icent][ipt][id]=(TProfile*)inFile->Get(Form("hPT_%i_%i_%i",icent,ipt,id));
        hv22pt[icent][ipt][id]=(TProfile*)inFile->Get(Form("hv22pt_%i_%i_%i",icent,ipt,id));
        hv24pt[icent][ipt][id]=(TProfile*)inFile->Get(Form("hv24pt_%i_%i_%i",icent,ipt,id));
        hcov22prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov22prime_%i_%i_%i",icent,ipt,id));
        hcov24prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov24prime_%i_%i_%i",icent,ipt,id));
        hcov42prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov42prime_%i_%i_%i",icent,ipt,id));
        hcov44prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov44prime_%i_%i_%i",icent,ipt,id));
        hcov2prime4prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov2prime4prime_%i_%i_%i",icent,ipt,id));
      }
    } // end of loop over pt bin
  } // end of loop over centrality classes

  //==========================================================================================================================
  // if (bDrawPlots1040){
  //   // Add
  //   for (int icent=2; icent<4; icent++){ // loop over centrality classes
  //     HRes[1] -> Add(HRes[icent]);
  //     hv22[1] -> Add(hv22[icent]);
  //     hv24[1] -> Add(hv24[icent]);
  //     hcov24[1] -> Add(hcov24[icent]);
  //     for(int ipt=0; ipt<npt; ipt++){ // loop over pt bin
  //       for (int id=0;id<npid;id++){ // loop over pid
  //         hv2EP[1][ipt][id]-> Add(hv2EP[icent][ipt][id]);
  //         hPT[1][ipt][id]-> Add(hPT[icent][ipt][id]);
  //         hv22pt[1][ipt][id]-> Add(hv22pt[icent][ipt][id]);
  //         hv24pt[1][ipt][id]-> Add(hv24pt[icent][ipt][id]);
  //         hcov22prime[1][ipt][id]-> Add(hcov22prime[icent][ipt][id]);
  //         hcov24prime[1][ipt][id]-> Add(hcov24prime[icent][ipt][id]);
  //         hcov42prime[1][ipt][id]-> Add(hcov42prime[icent][ipt][id]);
  //         hcov44prime[1][ipt][id]-> Add(hcov44prime[icent][ipt][id]);
  //         hcov2prime4prime[1][ipt][id]-> Add(hcov2prime4prime[icent][ipt][id]);
  //       }
  //     } // end of loop over pt bin
  //   }
  // }
  //==========================================================================================================================
  /*
  // Filling pT bin
  double pt[ncent][npt];
  double ept[ncent][npt]={{0}}; // error bin pT = 0.0
  for (int icent=0; icent<ncent; icent++){
    for (int ipt=0; ipt<npt; ipt++){
      // pt[icent][ipt] = hPT[icent][ipt] -> GetBinContent(1);
      pt[icent][ipt] = ( bin_pT[ipt] + bin_pT[ipt+1] ) / 2.;
    }
  }
  */
  //==========================================================================================================================
  for (int ipt=0;ipt<npt;ipt++){
    for (int id=0;id<npid;id++){
      sprintf(hname,"prV22dif1040_%i_%i",ipt,id);
      prV22dif1040[ipt][id]=new TProfile(hname,hname,1,0.,1.);
      sprintf(hname,"prV24dif1040_%i_%i",ipt,id);
      prV24dif1040[ipt][id]=new TProfile(hname,hname,1,0.,1.);
      sprintf(hname,"prV2EPdif1040_%i_%i",ipt,id);
      prV2EPdif1040[ipt][id]=new TProfile(hname,hname,1,0.,1.);
      sprintf(hname,"pt1040_%i_%i",ipt,id);
      pt1040[ipt][id]=new TProfile(hname,hname,1,0.,1.);
    }
  }
  for (int icent=0; icent<ncent; icent++){ // loop over centrality classes
    // double v2EP = hv22EP[icent]->GetBinContent(1)/sqrt(HRes[icent]->GetBinContent(1));
    // double ev2EP = hv22EP[icent]->GetBinError(1)/sqrt(HRes[icent]->GetBinContent(1));
    // 2QC
    term cor2 = term(hv22[icent]);
    double v22 = sqrt(cor2.mVal);
    double ev22 = sqrt(1./(4.*cor2.mVal)*cor2.mMSE);
    // 4QC
    term cor4 = term(hv24[icent]);
    double cov24 = Covariance(hcov24[icent],hv22[icent],hv24[icent]);
    double v24 = pow(2*pow(cor2.mVal,2)-cor4.mVal,0.25);
    double ev24 = sqrt( 1./pow(v24,6)*(cor2.mVal*cor2.mVal*cor2.mMSE+1./16*cor4.mMSE-0.5*cor2.mVal*cov24) );
    for (int id=0;id<npid;id++){
      sprintf(hname,"prV22int_%i_%i",icent,id);
      prV22int[icent][id]=new TProfile(hname,hname,1,0.,1.);
      sprintf(hname,"prV24int_%i_%i",icent,id);
      prV24int[icent][id]=new TProfile(hname,hname,1,0.,1.);

      if (icent==1 && bDrawPlots1040) cout << "PID: "<< id << endl;
      vector <double> vV2EPDif, vV22Dif, vV24Dif, vPt;
      vector <double> eV2EPDif, eV22Dif, eV24Dif, ePt;
      // Differential flow calculation
      for(int ipt=0; ipt<npt; ipt++){ // loop for all pT bin
        // vPt.push_back(hPT[icent][ipt][id] -> GetBinContent(1));
        vPt.push_back((bin_pT[ipt]+bin_pT[ipt+1])/2.);
        ePt.push_back(0);
        // v2EP
        // double res2 = sqrt(HRes[icent]->GetBinContent(1));
        // double v2obs = hv2EP[icent][ipt][id]->GetBinContent(1);
        double v2obs = pv2EP[icent][id]->GetBinContent(1+ipt);
        
        // double v2EPDif = v2obs / res2;
        double v2EPDif = v2obs;
        // double ev2EP = hv2EP[icent][ipt][id]->GetBinError(1);
        double ev2EP = pv2EP[icent][id]->GetBinError(1+ipt);
        vV2EPDif.push_back(v2EPDif);
        eV2EPDif.push_back(ev2EP);
        
        // v22
        term cor2red = term(hv22pt[icent][ipt][id]);
        double v22Dif = cor2red.mVal/v22;
        double cov22prime = Covariance(hcov22prime[icent][ipt][id],hv22[icent],hv22pt[icent][ipt][id]);
        double ev22Dif = sqrt(0.25*pow(cor2.mVal,-3)*(pow(cor2red.mVal,2)*cor2.mMSE
                            + 4*pow(cor2.mVal,2)*cor2red.mMSE - 4*cor2.mVal*cor2red.mVal*cov22prime));
        vV22Dif.push_back(v22Dif);
        eV22Dif.push_back(ev22Dif);
        
        // v24
        term cor4red = term(hv24pt[icent][ipt][id]);
        double cov24prime = Covariance(hcov24prime[icent][ipt][id],hv22[icent],hv24pt[icent][ipt][id]);
        double cov42prime = Covariance(hcov42prime[icent][ipt][id],hv24[icent],hv22pt[icent][ipt][id]);
        double cov44prime = Covariance(hcov44prime[icent][ipt][id],hv24[icent],hv24pt[icent][ipt][id]);
        double cov2prime4prime = Covariance(hcov2prime4prime[icent][ipt][id],hv22pt[icent][ipt][id],hv24pt[icent][ipt][id]);
        double v24Dif = (2.*cor2.mVal*cor2red.mVal-cor4red.mVal)*pow(v24,-3);
        double ev24Dif = sqrt( pow(v24,-14)
            * (pow(2*cor2.mVal*cor2.mVal*cor2red.mVal-3*cor2.mVal*cor4red.mVal+2*cor4.mVal*cor2red.mVal,2.)
            * cor2.mMSE
            + 9./16*pow(2.*cor2.mVal*cor2red.mVal-cor4red.mVal,2.)*cor4.mMSE
            + 4*pow(cor2.mVal,2)*pow(v24,8)*cor2red.mMSE
            + pow(v24,8)*cor4red.mMSE
            - 1.5*(2*cor2.mVal*cor2red.mVal-cor4red.mVal)
            * (2*cor2.mVal*cor2.mVal*cor2red.mVal-3*cor2.mVal*cor4red.mVal+2*cor4.mVal*cor2red.mVal)
            * cov24
            - 4*cor2.mVal*pow(v24,4)
            * (2*cor2.mVal*cor2.mVal*cor2red.mVal-3*cor2.mVal*cor4red.mVal+2*cor4.mVal*cor2red.mVal)
            * cov22prime
            + 2*pow(v24,4)
            * (2*cor2.mVal*cor2.mVal*cor2red.mVal-3*cor2.mVal*cor4red.mVal+2*cor4.mVal*cor2red.mVal)
            * cov24prime
            + 3*cor2.mVal*pow(v24,4)*(2*cor2.mVal*cor2red.mVal-cor4red.mVal)
            * cov42prime
            - 1.5*pow(v24,4)*(2*cor2.mVal*cor2red.mVal-cor4red.mVal)
            * cov44prime
            - 4*cor2.mVal*pow(v24,8)*cov2prime4prime));
        vV24Dif.push_back(v24Dif);
        eV24Dif.push_back(ev24Dif);
        prV22int[icent][id] -> Fill(0.5,v22Dif,hPT[icent][ipt][id] -> GetBinEntries(1));
        prV24int[icent][id] -> Fill(0.5,v24Dif,hPT[icent][ipt][id] -> GetBinEntries(1));
        if (icent>=1 && icent <=3) {
          prV2EPdif1040[ipt][id]->Fill(0.5,v2EPDif,hPT[icent][ipt][id] -> GetBinEntries(1));
          prV22dif1040[ipt][id]->Fill(0.5,v22Dif,hPT[icent][ipt][id] -> GetBinEntries(1));
          prV24dif1040[ipt][id]->Fill(0.5,v24Dif,hPT[icent][ipt][id] -> GetBinEntries(1));
          pt1040[ipt][id]->Fill(0.5,hPT[icent][ipt][id] -> GetBinContent(1),hPT[icent][ipt][id] -> GetBinEntries(1));
        }
        // if (icent==1 && bDrawPlots1040) cout << ev22Dif <<", ";
        if (icent==1 && bDrawPlots1040) cout << ev24Dif <<", ";
        // if (icent==1 && bDrawPlots1040) cout << ev2EP <<", ";
      } // end of loop for all pT bin
      if (icent==1 && bDrawPlots1040) cout << endl;
      // 2QC differential flow
      grDifFl[0][icent][id] = new TGraphErrors(npt,&vPt[0],&vV22Dif[0],&ePt[0],&eV22Dif[0]);
      grDifFl[0][icent][id] -> SetMarkerColor(kRed);
      grDifFl[0][icent][id] -> SetMarkerStyle(25);
      // 4QC differential flow
      grDifFl[1][icent][id] = new TGraphErrors(npt,&vPt[0],&vV24Dif[0],&ePt[0],&eV24Dif[0]);
      grDifFl[1][icent][id] -> SetMarkerColor(kGreen+1);
      grDifFl[1][icent][id] -> SetMarkerStyle(20);
      // EP differential flow
      grDifFl[2][icent][id] = new TGraphErrors(npt,&vPt[0],&vV2EPDif[0],&ePt[0],&eV2EPDif[0]);
      grDifFl[2][icent][id] -> SetMarkerColor(kAzure+2);
      grDifFl[2][icent][id] -> SetMarkerStyle(22);
      for (int i=0; i<3; i++){
        grDifFl[i][icent][id] -> SetMarkerSize(1.5);
        grDifFl[i][icent][id] -> SetDrawOption("P");
      }
    } // end of loop over PID
  } // end of loop over centrality classes

  //==========================================================================================================================
  if (bDrawPlots1040) return;
  const char *grTitleDF[4]={"[1] v_{2}{2,QC};p_{T}, GeV/c;v_{2}","[2] v_{2}{4,QC};p_{T}, GeV/c;v_{2}","[3] v_{2}{#eta sub-event};p_{T}, GeV/c;v_{2}","v_{2}{MC};p_{T}, GeV/c;v_{2}"};
  
  outFile -> cd();
  for (int imethod=0; imethod<3; imethod++){
    for (int icent=0;icent<ncent;icent++){
      for (int id=0;id<npid;id++){
        grDifFl[imethod][icent][id] -> SetTitle(grTitleDF[imethod]);
        grDifFl[imethod][icent][id] -> Write(Form("gr_cent%i_%i_%i",icent,imethod,id));
      }
    }
  }

  std::vector<TGraphErrors*> vgrv2pt[ncent][npid];
  for (int icent=0; icent<ncent; icent++){
    for (int id=0;id<npid;id++){
      for (int i=0; i<3; i++){
        vgrv2pt[icent][id].push_back(grDifFl[i][icent][id]);
      }
    }
  }
  TCanvas *cV2PT[ncent][npid];
  for (int icent=0; icent<6; icent++){
    for (int id=0;id<npid;id++){
      sprintf(hname,"%s, centrality %i-%i%%",pidFancyNames.at(id).Data(),icent*10,(icent+1)*10);
      cV2PT[icent][id] = (TCanvas*) DrawTGraph(vgrv2pt[icent][id],"",0.77, 1.23, 0., maxpt, -0.01, 0.2, 0.18, 0.56, 0.5, 0.8, hname);
      cV2PT[icent][id] -> SetName(hname);
      sprintf(hname,"../Graphics/%sDFCent%i-%i.png",pidNames.at(id).Data(),icent*10,(icent+1)*10);
      cV2PT[icent][id] -> SaveAs(hname);
    }
  }

  double eV22Dif1040CHp[npt]={0.000287985, 0.000346998, 0.000429414, 0.000541804, 0.0007074, 0.000829631, 0.00129287, 0.00174203, 0.00538854};
  double eV22Dif1040Pionp[npt]={0.000312526, 0.000438508, 0.00066387, 0.000992462, 0.00141259, 0.00178265, 0.00300932, 0.00452045, 0.0161991};
  double eV22Dif1040Kaonp[npt]={0.00129967, 0.00115888, 0.00132715, 0.00166258, 0.00217445, 0.00264019, 0.00441676, 0.00665323, 0.0244402};
  double eV22Dif1040Protonp[npt]={0.000982349, 0.000689308, 0.0006369, 0.000698007, 0.000846397, 0.000949636, 0.00142479, 0.0018894, 0.00584226};
  double eV22Dif1040CHm[npt]={0.000295261, 0.000407241, 0.000607424, 0.000906665, 0.00129327, 0.00162468, 0.00271712, 0.00407054, 0.0146936};
  double eV22Dif1040Pionm[npt]={0.000299178, 0.000419966, 0.000636206, 0.000955824, 0.00136474, 0.00172153, 0.002902, 0.00436158, 0.0158048};
  double eV22Dif1040Kaonm[npt]={0.00177733, 0.00161408, 0.00191686, 0.00251957, 0.0034679, 0.00438097, 0.00760223, 0.0117106, 0.0414045};
  double eV22Dif1040Protonm[npt]={0.0160834, 0.00978732, 0.00805651, 0.00819944, 0.00967801, 0.011032, 0.0186514, 0.0281518, 0.0556309};

  double eV24Dif1040CHp[npt]={0.00393944, 0.0046574, 0.00580832, 0.00754447, 0.00962995, 0.0111394, 0.0162147, 0.0217968, 0.062402};
  double eV24Dif1040Pionp[npt]={0.00427627, 0.00589614, 0.00888263, 0.0127643, 0.0176608, 0.021809, 0.0359699, 0.0536001, 0.187989};
  double eV24Dif1040Kaonp[npt]={0.0159767, 0.014355, 0.0161911, 0.0203085, 0.0262709, 0.0317771, 0.0521675, 0.0777483, 0.278757};
  double eV24Dif1040Protonp[npt]={0.0122489, 0.00866283, 0.00811008, 0.00931179, 0.0111635, 0.012543, 0.017628, 0.023264, 0.0674229};
  double eV24Dif1040CHm[npt]={0.00402892, 0.00562555, 0.0079661, 0.0116505, 0.0162427, 0.0198523, 0.0324849, 0.0485143, 0.175717};
  double eV24Dif1040Pionm[npt]={0.00407939, 0.00576103, 0.00829019, 0.012316, 0.0171313, 0.0210563, 0.0349607, 0.0521695, 0.190774};
  double eV24Dif1040Kaonm[npt]={0.0217772, 0.0197707, 0.0230765, 0.0296323, 0.0399422, 0.0507466, 0.0842217, 0.128386, 0.443462};
  double eV24Dif1040Protonm[npt]={0.223339, 0.129618, 0.101039, 0.103635, 0.11948, 0.130162, 0.217291, 0.337854, 0.412331};

  double eV2EPDif1040CHp[npt]={0.000241064, 0.000286511, 0.000350603, 0.000435495, 0.000559585, 0.000651876, 0.00106183, 0.00150943, 0.00523379};
  double eV2EPDif1040Pionp[npt]={0.000260159, 0.000356126, 0.000529311, 0.000794359, 0.00118338, 0.0015558, 0.00281157, 0.0043393, 0.0158445};
  double eV2EPDif1040Kaonp[npt]={0.00108166, 0.000949999, 0.00110731, 0.00143954, 0.00196732, 0.00244518, 0.00425293, 0.00648211, 0.0238246};  
  double eV2EPDif1040Protonp[npt]={0.000795849, 0.000559932, 0.000516356, 0.000558544, 0.000670997, 0.000751035, 0.00119085, 0.00166205, 0.00570161};
  double eV2EPDif1040CHm[npt]={0.000245449, 0.000331495, 0.000486003, 0.000721979, 0.00106788, 0.00139695, 0.00251906, 0.00389362, 0.0144634};
  double eV2EPDif1040Pionm[npt]={0.000248607, 0.000341562, 0.000508223, 0.000762961, 0.00113662, 0.00149381, 0.00270315, 0.00417821, 0.0154637};
  double eV2EPDif1040Kaonm[npt]={0.0015534, 0.00139149, 0.00170338, 0.00233115, 0.00331287, 0.00424726, 0.00752357, 0.0116479, 0.0445369};  
  double eV2EPDif1040Protonm[npt]={0.0146491, 0.00910568, 0.0075653, 0.0077716, 0.00922676, 0.0106228, 0.0180493, 0.0276428, 0.103005};

  double eV22Dif1040[npid][npt], eV24Dif1040[npid][npt], eV2EPDif1040[npid][npt];

  for (int ipt=0;ipt<npt;ipt++){
    eV22Dif1040[0][ipt] = eV22Dif1040CHp[ipt];
    eV22Dif1040[1][ipt] = eV22Dif1040Pionp[ipt];
    eV22Dif1040[2][ipt] = eV22Dif1040Kaonp[ipt];
    eV22Dif1040[3][ipt] = eV22Dif1040Protonp[ipt];
    eV22Dif1040[4][ipt] = eV22Dif1040CHm[ipt];
    eV22Dif1040[5][ipt] = eV22Dif1040Pionm[ipt];
    eV22Dif1040[6][ipt] = eV22Dif1040Kaonm[ipt];
    eV22Dif1040[7][ipt] = eV22Dif1040Protonm[ipt];    

    eV24Dif1040[0][ipt] = eV24Dif1040CHp[ipt];
    eV24Dif1040[1][ipt] = eV24Dif1040Pionp[ipt];
    eV24Dif1040[2][ipt] = eV24Dif1040Kaonp[ipt];
    eV24Dif1040[3][ipt] = eV24Dif1040Protonp[ipt];
    eV24Dif1040[4][ipt] = eV24Dif1040CHm[ipt];
    eV24Dif1040[5][ipt] = eV24Dif1040Pionm[ipt];
    eV24Dif1040[6][ipt] = eV24Dif1040Kaonm[ipt];
    eV24Dif1040[7][ipt] = eV24Dif1040Protonm[ipt];    

    eV2EPDif1040[0][ipt] = eV2EPDif1040CHp[ipt];
    eV2EPDif1040[1][ipt] = eV2EPDif1040Pionp[ipt];
    eV2EPDif1040[2][ipt] = eV2EPDif1040Kaonp[ipt];
    eV2EPDif1040[3][ipt] = eV2EPDif1040Protonp[ipt];
    eV2EPDif1040[4][ipt] = eV2EPDif1040CHm[ipt];
    eV2EPDif1040[5][ipt] = eV2EPDif1040Pionm[ipt];
    eV2EPDif1040[6][ipt] = eV2EPDif1040Kaonm[ipt];
    eV2EPDif1040[7][ipt] = eV2EPDif1040Protonm[ipt];
  }
    
  

  // double eV22Dif1040[npt]={4.16021e-05, 4.57347e-05, 6.66802e-05, 0.000102774, 0.00017121, 0.000272493, 0.000500597};
  // double eV24Dif1040[npt]={0.000514427, 0.000644468, 0.000887692, 0.00131467, 0.00206218, 0.00311487, 0.005409};
  // double eV2EPDif1040[npt]={3.68069e-05, 3.8137e-05, 5.68007e-05, 8.74456e-05, 0.000142745, 0.000228144, 0.000461732};
  double ePT[npt]={0.};
  TCanvas *cV2PT1040[npid];
  for (int id=0;id<npid;id++){
    vector <double> vV22Dif1040, vV24Dif1040, vV2EPDif1040, vPT;
    for (int ipt=0;ipt<npt;ipt++){
      vV22Dif1040.push_back(prV22dif1040[ipt][id]->GetBinContent(1));
      vV24Dif1040.push_back(prV24dif1040[ipt][id]->GetBinContent(1));
      vV2EPDif1040.push_back(prV2EPdif1040[ipt][id]->GetBinContent(1));
      // vPT.push_back(pt1040[ipt][id]->GetBinContent(1));
      vPT.push_back((bin_pT[ipt]+bin_pT[ipt+1])/2.);
    }
    grDifFl1040[0][id] = new TGraphErrors(npt,&vPT[0],&vV22Dif1040[0],&ePT[0],eV22Dif1040[id]);
    grDifFl1040[0][id] -> SetMarkerColor(kRed);
    grDifFl1040[0][id] -> SetMarkerStyle(25);
    // 4QC differential flow
    grDifFl1040[1][id] = new TGraphErrors(npt,&vPT[0],&vV24Dif1040[0],&ePT[0],eV24Dif1040[id]);
    grDifFl1040[1][id] -> SetMarkerColor(kGreen+1);
    grDifFl1040[1][id] -> SetMarkerStyle(20);
    // EP differential flow
    grDifFl1040[2][id] = new TGraphErrors(npt,&vPT[0],&vV2EPDif1040[0],&ePT[0],eV2EPDif1040[id]);
    grDifFl1040[2][id] -> SetMarkerColor(kAzure+2);
    grDifFl1040[2][id] -> SetMarkerStyle(22);
    for (int i=0; i<3; i++){
      grDifFl1040[i][id] -> SetMarkerSize(1.5);
      grDifFl1040[i][id] -> SetDrawOption("P");
      grDifFl1040[i][id] -> SetLineWidth(1);
    }

    std::vector<TGraphErrors*> vgrv2pt1040;
    for (int imeth=0; imeth<3; imeth++){
      grDifFl1040[imeth][id] -> SetTitle(grTitleDF[imeth]);
      vgrv2pt1040.push_back(grDifFl1040[imeth][id]);
      grDifFl1040[imeth][id] -> Write(Form("gr_cent10-40_%i_%i",imeth,id));
    }
    
    
    sprintf(hname,"%s, centrality 10-40%%",pidFancyNames.at(id).Data());
    cV2PT1040[id] = (TCanvas*) DrawTGraph(vgrv2pt1040,"",0.67,1.33,    0.,maxpt,-0.01,0.2,     0.18,0.56,0.5,0.8,hname);
    cV2PT1040[id] -> SetName(hname);
    cV2PT1040[id] -> SaveAs(Form("../Graphics/%sDFCent10-40.png",pidNames.at(id).Data()));
  }
}
/*
void v2plot_integrated_flow(){
  if (bDrawPlots1040) return;
  char hname[400];
  TFile *inFile = new TFile("../ROOTFile/PID_Peter_2ndIteration.root","read");
  // Input histograms
  TProfile *hv22[ncent][npid];        // profile of integrated flow from v2{2}
  TProfile *hv24[ncent][npid];        // profile of integrated flow from v2{4}
  TProfile *hPT[ncent][npt][npid];       // profile pt 
  TProfile *hv22pt[ncent][npt][npid];    // profile <<2'>> from 2nd Q-Cumulants
  TProfile *hv24pt[ncent][npt][npid];    // profile <<4'>> from 4th Q-Cumulants
  TProfile *hcov24[ncent][npid];       // <2>*<4>
  TProfile *hcov22prime[ncent][npt][npid]; // <2>*<2'>
  TProfile *hcov24prime[ncent][npt][npid]; // <2>*<4'>
  TProfile *hcov42prime[ncent][npt][npid]; // <2>*<4'>
  TProfile *hcov44prime[ncent][npt][npid]; // <4>*<4'>
  TProfile *hcov2prime4prime[ncent][npt][npid]; // <2'>*<4'>
  TProfile *hv2EP[ncent][npt][npid];	// elliptic flow from EP method
  TProfile *hv22EP[ncent][npid];      
  // TProfile *HRes[ncent];
  // Get histograms
  for (int icent=0; icent<ncent; icent++){ // loop over centrality classes
    // hv22EP[icent] = (TProfile*)inFile->Get(Form("hv22EP_%i",icent));
    // HRes[icent] = (TProfile*)inFile->Get(Form("HRes_%i",icent));
    // hv22[icent] = (TProfile*)inFile->Get(Form("hv22_%i",icent));
    // hv24[icent] = (TProfile*)inFile->Get(Form("hv24_%i",icent));
    // hcov24[icent] = (TProfile*)inFile->Get(Form("hcov24_%i",icent));
    for(int ipt=0; ipt<npt; ipt++){ // loop over pt bin
      for (int id=0;id<npid;id++){
        hv2EP[icent][ipt][id]=(TProfile*)inFile->Get(Form("hv2EP_%i_%i_%i",icent,ipt,id));
        hPT[icent][ipt][id]=(TProfile*)inFile->Get(Form("hPT_%i_%i_%i",icent,ipt,id));
        hv22pt[icent][ipt][id]=(TProfile*)inFile->Get(Form("hv22pt_%i_%i_%i",icent,ipt,id));
        hv24pt[icent][ipt][id]=(TProfile*)inFile->Get(Form("hv24pt_%i_%i_%i",icent,ipt,id));
        // hcov22prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov22prime_%i_%i_%i",icent,ipt,id));
        // hcov24prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov24prime_%i_%i_%i",icent,ipt,id));
        // hcov42prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov42prime_%i_%i_%i",icent,ipt,id));
        // hcov44prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov44prime_%i_%i_%i",icent,ipt,id));
        hcov2prime4prime[icent][ipt][id]=(TProfile*)inFile->Get(Form("hcov2prime4prime_%i_%i_%i",icent,ipt,id));
      }
    } // end of loop over pt bin
  } // end of loop over centrality classes
  // inFile -> Close();
  for (int icent=0;icent<ncent;icent++){
    for (int id=0;id<npid;id++){
      hv22EP[icent][id] = (TProfile*) hv2EP[icent][0][id]->Clone();
      hv22[icent][id] = (TProfile*) hv22pt[icent][0][id]->Clone();
      hv24[icent][id] = (TProfile*) hv24pt[icent][0][id]->Clone();
      hcov24[icent][id] = (TProfile*) hcov2prime4prime[icent][0][id]->Clone();
      for (int ipt=1;ipt<npt;ipt++){
        hv22EP[icent][id]->Add(hv2EP[icent][ipt][id]);
        hv22[icent][id]->Add(hv22pt[icent][ipt][id]);
        hv24[icent][id]->Add(hv24pt[icent][ipt][id]);
        hcov24[icent][id]->Add(hcov2prime4prime[icent][ipt][id]);
      }
    }
  }
  TGraphErrors *grIntFlowVsCent[nmethod][npid];
  TCanvas *can[npid];
  for (int id=0;id<npid;id++){
    std::vector<double> vV2EP, vV22, vV24, vV22int, vV24int;
    std::vector<double> eV2EP, eV22, eV24, eV22int, eV24int;

    for (int icent=0;icent<ncent;icent++){

      // EP
      vV2EP.push_back( hv22EP[icent][id]->GetBinContent(1) );
      eV2EP.push_back( hv22EP[icent][id]->GetBinError(1) );
      // vV2EP.push_back( hv22EP[icent]->GetBinContent(1));
      // eV2EP.push_back( hv22EP[icent]->GetBinError(1)  );    
      // 2QC
      term cor2 = term(hv22[icent][id]);
      vV22.push_back(sqrt(cor2.mVal));
      eV22.push_back(sqrt(1./(4.*cor2.mVal)*cor2.mMSE));
      // 4QC
      term cor4 = term(hv24[icent][id]);
      double cov24 = Covariance(hcov24[icent][id],hv22[icent][id],hv24[icent][id]);
      double v24 = pow(2*pow(cor2.mVal,2)-cor4.mVal,0.25);
      vV24.push_back(v24);
      eV24.push_back( sqrt( 1./pow(v24,6)*(cor2.mVal*cor2.mVal*cor2.mMSE+1./16*cor4.mMSE-0.5*cor2.mVal*cov24) ) );
  
      vV22int.push_back(prV22int[icent][id]->GetBinContent(1));
      vV24int.push_back(prV24int[icent][id]->GetBinContent(1));
      eV22int.push_back(prV22int[icent][id]->GetBinError(1));
      eV24int.push_back(prV24int[icent][id]->GetBinError(1));
    }
    
    // grIntFlowVsCent[0][id] = new TGraphErrors(ncent,bin_cent,&vV22[0],bin_centE,&eV22[0]);
    // grIntFlowVsCent[0][id] -> SetMarkerColor(kRed);
    // grIntFlowVsCent[0][id] -> SetMarkerStyle(25);  

    // grIntFlowVsCent[1][id] = new TGraphErrors(ncent,bin_cent,&vV24[0],bin_centE,&eV24[0]);
    // grIntFlowVsCent[1][id]->SetMarkerColor(kGreen+1);
    // grIntFlowVsCent[1][id]->SetMarkerStyle(20);

    grIntFlowVsCent[2][id] = new TGraphErrors(ncent,bin_cent,&vV2EP[0],bin_centE,&eV2EP[0]);
    grIntFlowVsCent[2][id] -> SetMarkerColor(kAzure+2);
    grIntFlowVsCent[2][id] -> SetMarkerStyle(22);

    grIntFlowVsCent[0][id] = new TGraphErrors(ncent,bin_cent,&vV22int[0],bin_centE,&eV22[0]);
    grIntFlowVsCent[0][id] -> SetMarkerColor(kRed);
    grIntFlowVsCent[0][id] -> SetMarkerStyle(25); 

    grIntFlowVsCent[1][id] = new TGraphErrors(ncent,bin_cent,&vV24int[0],bin_centE,&eV24[0]);
    grIntFlowVsCent[1][id] -> SetMarkerColor(kGreen+1);
    grIntFlowVsCent[1][id] -> SetMarkerStyle(20);

    for (int i=0;i<nmethod;i++){
      grIntFlowVsCent[i][id] -> SetMarkerSize(1.5);
      grIntFlowVsCent[i][id] -> SetDrawOption("P");
    }
    // const char *grTitle[5]={"[1] v_{2}{2,QC};cent, %;v_{2}",
    //                         "[2] v_{2}{4,QC};cent, %;v_{2}",
    //                         "[3] v_{2}{#eta sub-event};cent, %;v_{2}",
    //                         "[4] v_{2}{2,QC fix};cent, %;v_{2}",
    //                         "[5] v_{2}{4,QC fix};cent, %;v_{2}"};
    const char *grTitle[3]={"[1] v_{2}{2,QC};cent, %;v_{2}",
                            "[2] v_{2}{4,QC};cent, %;v_{2}",
                            "[3] v_{2}{#eta sub-event};cent, %;v_{2}",};
    outFile -> cd();
    for (int imeth=0; imeth<nmethod; imeth++){
      grIntFlowVsCent[imeth][id] -> SetTitle(grTitle[imeth]);
      grIntFlowVsCent[imeth][id] -> Write(Form("grRF_%i_%i",imeth,id));
    }


    std::vector<TGraphErrors*> vgr;
    for (int imeth=0; imeth<nmethod; imeth++){
      vgr.push_back(grIntFlowVsCent[imeth][id]);
    }

    
    can[id] = (TCanvas*) DrawTGraph(vgr,"",0.48,1.22,   0,60,-0.005,0.1,    0.18,0.56,0.5,0.8,pidFancyNames.at(id).Data()); // DCA<0.5: 0.88, 1.12 // wo DCA: 0.76, 1.03
    can[id] -> SetName(pidNames.at(id).Data());
    can[id] -> SaveAs(Form("../Graphics/%sV2vsCent.png",pidNames.at(id).Data()));
  } // end of loop over particle ID
}
*/
void v2plotPID_Peter(){
  v2plot_differential_flow();
  // v2plot_integrated_flow();
}