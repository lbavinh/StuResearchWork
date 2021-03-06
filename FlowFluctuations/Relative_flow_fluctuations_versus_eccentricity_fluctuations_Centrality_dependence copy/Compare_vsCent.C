void Compare_vsCent()
{
  // const int nbinBimp = 20; // 0-80 %
  // const double bin_b[nbinBimp] = {0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5,12.5,13.5,14.5,15.5,16.5,17.5,18.5,19.5};
  // const double bin_bE[nbinBimp] = {0};


  const TString modelInitialEccentricity = "Glauber";
  // const TString modelInitialEccentricity = "Glissando";
  const int nmethod = 3; // 22, 24, 22-gapped
  const int ratioToMethod = 2;
  const float markerSize = 1.5;
  const float maxY = 1.3;
  const float minY = 0.4;
  const float minX = 0;
  const float maxX = 80;
  const TString title[] = {"v_{2}{2}","v_{2}{4}","v_{2}{2,#eta-gap}"};
  int marker[]={21,20,22};
  TFile *fV2   = new TFile("v2_UrQMD_7.7GeV_8PID.root","read");
  TFile *fEcc2 = new TFile(Form("ecc_%s_7.7GeV.root",modelInitialEccentricity.Data()),"read");
  TGraphErrors *grRatioEcc2 = (TGraphErrors*) fEcc2 -> Get("gr_e24e22");
  grRatioEcc2 -> SetMarkerStyle(23);
  grRatioEcc2 -> SetTitle("#epsilon_{2}{4}/#epsilon_{2}{2};centrality, %");
  grRatioEcc2 -> SetMarkerColor(kRed+2);
  grRatioEcc2 -> SetLineColor(kRed+2);
  TGraphErrors *grV2[nmethod], *grRatioV2[nmethod];
  for (int m=0; m<nmethod; m++)
  {
    grV2[m] = (TGraphErrors*) fV2 -> Get(Form("grRF_%i",m));
  }
  for (int m=0; m<nmethod; m++){
    grV2[m] -> SetMarkerStyle(marker[m]);
    grV2[m] -> SetMarkerSize(markerSize);
    grV2[m] -> SetTitle(title[m].Data());
  }
  grV2[0] -> SetMarkerColor(kRed+2);
  grV2[1] -> SetMarkerColor(kBlue+3);
  grV2[2] -> SetMarkerColor(kGreen+3);

  // ratio graph
  Double_t *vx_gr[nmethod], *vy_gr[nmethod], *ex_gr[nmethod], *ey_gr[nmethod];
  Int_t nbins[nmethod];

  for (int m=0;m<nmethod;m++){
    // Read points
    vx_gr[m]=(Double_t*)grV2[m]->GetX();
    vy_gr[m]=(Double_t*)grV2[m]->GetY();

    // Read errors
    ex_gr[m]=(Double_t*)grV2[m]->GetEX();
    ey_gr[m]=(Double_t*)grV2[m]->GetEY();

    nbins[m]=(Int_t) grV2[m]->GetN();
  }

  for (int m=0;m<nmethod;m++){
    std::vector<Double_t> vRatio, vRatioErr;
    for (int i=0;i<nbins[ratioToMethod];i++){
      Double_t ratio = vy_gr[m][i]/vy_gr[ratioToMethod][i];
      Double_t ratioErr = ratio*(TMath::Sqrt(TMath::Power(ey_gr[ratioToMethod][i]/vy_gr[ratioToMethod][i],2)+TMath::Power(ey_gr[m][i]/vy_gr[m][i],2)));
      vRatio.push_back(ratio);
      vRatioErr.push_back(ratioErr);
    }
    grRatioV2[m] = new TGraphErrors(nbins[ratioToMethod],vx_gr[ratioToMethod],&vRatio[0],ex_gr[ratioToMethod],&vRatioErr[0]);
    grRatioV2[m] -> SetMarkerStyle(grV2[m]->GetMarkerStyle());
    grRatioV2[m] -> SetMarkerColor(grV2[m]->GetMarkerStyle());
    grRatioV2[m] -> SetLineColor(grV2[m]->GetMarkerStyle());
    grRatioV2[m] -> SetMarkerSize(markerSize);

    grRatioV2[m]->SetTitle("v_{2}{4}/v_{2}{2};centrality, %");
    grRatioV2[m]->GetXaxis()->SetRangeUser(minX,maxX);
    grRatioV2[m]->SetMinimum(minY);
    grRatioV2[m]->SetMaximum(maxY);

    vRatio.clear();
    vRatioErr.clear();
  }
  grRatioV2[0] -> SetMarkerColor(kRed+2);   // v22/v22gap
  grRatioV2[1] -> SetMarkerColor(kBlue+3);  // v24/v22gap
  grRatioV2[2] -> SetMarkerColor(kGreen+3); // v22gap/v22gap


  TGraphErrors *grRatioV2STAR;

  FILE *fp1;
  char path1[800];
  sprintf(path1,"v2methods77Cent.txt");
  if((fp1=fopen(path1,"r"))== NULL) {
    cout << "\n Can't open file v2methods77Cent.txt !\n"<< endl;
    fflush(stdin);
    return;
  }
  int ncent = 9;
  double cent[ncent], v2eta[ncent], v22[ncent], v24[ncent];
  double centE[ncent], v2etae[ncent], v22e[ncent], v24e[ncent];
  double ratiov24v22[ncent], ratiov24v22e[ncent];
  for (int ipt=0; ipt<ncent; ipt++) {
    float  mCent,s2,  s2e,  s2eta,  s2etae,    s22,s22e,s24,s24e,s2b, s2be;
    //    cent,v2EP,v2EPe,v2etasub,v2etasube,v22,v22e,v24,v24e,v2bbc,v2bbce
    fscanf(fp1, "%f  %f  %f  %f  %f  %f  %f  %f  %f %f %f\n",&mCent,&s2,&s2e,&s2eta,&s2etae,&s22,&s22e,&s24,&s24e,&s2b,&s2be);

    cent[ipt]     = mCent;
    v2eta[ipt]  = s2eta;
    v2etae[ipt] = s2etae;
    v22[ipt]    = s22;
    v22e[ipt]   = s22e;
    v24[ipt]    = s24;
    v24e[ipt]   = s24e;
    centE[ipt]    = 0.;
    Double_t ratio = s24/s22;
    Double_t ratioErr = ratio*(TMath::Sqrt(TMath::Power(s24e/s24,2)+TMath::Power(s22e/s22,2)));
    ratiov24v22[ipt] = ratio;
    ratiov24v22e[ipt] = ratioErr;
  }
  // STAR
  grRatioV2STAR = new TGraphErrors(ncent,cent,ratiov24v22,centE,ratiov24v22e);
  grRatioV2STAR -> SetMarkerStyle(kFullStar);
  grRatioV2STAR -> SetMarkerSize(markerSize+1);
  grRatioV2STAR -> SetMarkerColor(kRed+1);
  grRatioV2STAR -> SetLineColor(kRed+1);
  grRatioV2STAR -> SetTitle("v_{2}{4}/v_{2}{2};centrality, %");
  // grRatioV2STAR[iener] = new TGraphErrors(ncent,cent,ratiov24v22,centE,ratiov24v22e);
  

  TCanvas c;
  grRatioV2[1] -> Draw("AP");
  grRatioEcc2 -> Draw("P");
  grRatioV2STAR->Draw("P");
  TLegend *l = new TLegend(0.3,0.7,0.6,0.85);
  l->SetBorderSize(0);
  l->SetBorderSize(0);
  l->SetTextFont(42);
  l->SetTextSize(0.04);
  l->AddEntry(grRatioV2[1],Form("%s (UrQMD)",grRatioV2[1]->GetTitle()),"p");
  l->AddEntry(grRatioV2STAR,Form("%s (STAR)",grRatioV2STAR->GetTitle()),"p");
  l->AddEntry(grRatioEcc2,Form("%s (%s)",grRatioEcc2->GetTitle(),modelInitialEccentricity.Data()),"p");
  l->Draw();
  TLine lineOne;
  lineOne.SetLineStyle(2);
  lineOne.DrawLine(minX,1.,maxX,1.);
  grRatioV2[1]->SetTitle(";centrality, %;#epsilon_{2}{4}/#epsilon_{2}{2}, v_{2}{4}/v_{2}{2}");
  c.SaveAs(Form("Flow_Eccentricity_(%s)_Fluctuations_vs_Centrality.png",modelInitialEccentricity.Data()));





}