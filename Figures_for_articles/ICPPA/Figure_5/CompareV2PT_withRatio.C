void CompareV2PT_withRatio(){

  // const
  const int ntype = 3; // Good acceptance, Bad acceptance w/o correction, corrected
  const int nmethod = 4; // v22, v24, v2 TPC, v22(eta-gap)
  const int npid = 4; // CH, pions, kaons, protons
  const float minpt = -0.05;
  const float maxpt = 2.35;
  const float maxV2 = 0.16;
  const float minV2 = -0.01;
  const float minV2Ratio = 0.82;
  const float maxV2Ratio = 1.18;
  const float leg_coordinate[4]={0.45,0.03,0.99,0.3}; // top left: 0.28,0.7,0.6,0.99
  const float labelSizeUpperPlot = 0.095;
  const float titleSizeUpperPlot = 0.115;
  const float labelSizeLowerPlot = 0.08;
  const float titleSizeLowerPlot = 0.1;
  const float markerSize = 1.8;
  const float titleOffset = 0.8;
  const float textFont = 42;
  const int npad = 4;
  // flags
  const int ratioToType = 0; // Good Acceptance
  // some stuffs
  TString typeOfGraphName[ntype]={"Uniform acceptance","Corrected","Non-corrected"};
  TString padName[]={"(a)","(b)","(c)","(d)","(e)","(f)"};
  TString xAxisName = {"p_{T} (GeV/c)"};
  // TString ratioName[nmethod-1]={"v2FHCal/v22","v24/v22","v2TPC/v22"};
  // TString legendEntries[nmethod]={"v_{2}{#Psi_{1,FHCal}}","v_{2}{4}","v_{2}{#Psi_{2,TPC}^{}}","v_{2}{2}"}; // ,"v_2{EP} FHCal
  // TString pidInPads[npad]={"Ch. hadrons","#pi","p","h^{#pm}","#pi^{+}","p"};
  TString fileName[ntype]={"","_AC","_NAC"};
  TFile *input[ntype];

  TGraphErrors *grV2[ntype][npid][nmethod];
  TGraphErrors *grRatioV2[ntype][npid][nmethod];
  for(int it=0;it<ntype;it++){
    input[it] = new TFile(Form("v2_UrQMD_7.7GeV_Reco_MotherID%s.root",fileName[it].Data()),"read");
    for (int imeth=0;imeth<nmethod;imeth++){
      for(int id=0;id<npid;id++){
        grV2[it][id][imeth] = (TGraphErrors*) input[it] -> Get(Form("gr_cent10-40_%i_%i",imeth,id));
      }
    }
  }
  
  for (int imeth=0;imeth<nmethod;imeth++){
    for (int id=0;id<npid;id++){

      grV2[0][id][imeth] -> SetMarkerColor(kBlack);
      grV2[0][id][imeth] -> SetLineColor(kBlack);
      grV2[0][id][imeth] -> SetMarkerStyle(kOpenSquare);

      grV2[1][id][imeth] -> SetMarkerColor(kBlue+1);
      grV2[1][id][imeth] -> SetLineColor(kBlue+1);
      grV2[1][id][imeth] -> SetMarkerStyle(kFullCircle);  

      grV2[2][id][imeth] -> SetMarkerColor(kRed+1);
      grV2[2][id][imeth] -> SetLineColor(kRed+1);
      grV2[2][id][imeth] -> SetMarkerStyle(kFullTriangleUp);

      for(int it=0;it<ntype;it++){
        grV2[it][id][imeth] -> SetMarkerSize(markerSize);
      }
    }
  }
  
  Double_t *vx_gr[ntype][npid][nmethod], *vy_gr[ntype][npid][nmethod], *ex_gr[ntype][npid][nmethod], *ey_gr[ntype][npid][nmethod];
  Int_t nbins[ntype][npid][nmethod];
  for (int itype=0;itype<ntype;itype++){
    for (int id=0;id<npid;id++){
      for (int imeth=0;imeth<nmethod;imeth++){
        // Read points
        vx_gr[itype][id][imeth]=(Double_t*)grV2[itype][id][imeth]->GetX();
        vy_gr[itype][id][imeth]=(Double_t*)grV2[itype][id][imeth]->GetY();

        // Read errors
        ex_gr[itype][id][imeth]=(Double_t*)grV2[itype][id][imeth]->GetEX();
        ey_gr[itype][id][imeth]=(Double_t*)grV2[itype][id][imeth]->GetEY();

        nbins[itype][id][imeth]=(Int_t) grV2[itype][id][imeth]->GetN();
      }
    }
  }
  for (int imeth=0;imeth<nmethod;imeth++){
  
    for (int id=0;id<npid;id++){
      for (int itype=0;itype<ntype;itype++){
        std::vector<Double_t> vRatioToUniformAcceptance, vRatioToUniformAcceptanceErr;
        for (int i=0;i<nbins[ratioToType][id][imeth];i++){
          Double_t ratio = vy_gr[itype][id][imeth][i]/vy_gr[ratioToType][id][imeth][i];
          Double_t ratioErr = ratio*(TMath::Sqrt(TMath::Power(ey_gr[ratioToType][id][imeth][i]/vy_gr[ratioToType][id][imeth][i],2)+TMath::Power(ey_gr[itype][id][imeth][i]/vy_gr[itype][id][imeth][i],2)));
          vRatioToUniformAcceptance.push_back(ratio);
          vRatioToUniformAcceptanceErr.push_back(ratioErr);
        }
        grRatioV2[itype][id][imeth] = new TGraphErrors(nbins[ratioToType][id][imeth],vx_gr[ratioToType][id][imeth],&vRatioToUniformAcceptance[0],ex_gr[ratioToType][id][imeth],&vRatioToUniformAcceptanceErr[0]);
        // grRatioV2[itype][id][imeth] -> SetTitle(Form("%s",ratioName[imeth].Data()));
        vRatioToUniformAcceptance.clear();
        vRatioToUniformAcceptanceErr.clear();
      }
    }
  }
  for (int imeth=0;imeth<nmethod;imeth++){
    for (int id=0;id<npid;id++){
      grRatioV2[0][id][imeth] -> SetMarkerColor(kBlack);
      grRatioV2[0][id][imeth] -> SetLineColor(kBlack);
      grRatioV2[0][id][imeth] -> SetMarkerStyle(kOpenSquare);

      grRatioV2[1][id][imeth] -> SetMarkerColor(kBlue+1);
      grRatioV2[1][id][imeth] -> SetLineColor(kBlue+1);
      grRatioV2[1][id][imeth] -> SetMarkerStyle(kFullCircle);  

      grRatioV2[2][id][imeth] -> SetMarkerColor(kRed+1);
      grRatioV2[2][id][imeth] -> SetLineColor(kRed+1);
      grRatioV2[2][id][imeth] -> SetMarkerStyle(kFullTriangleUp);
      for(int it=0;it<ntype;it++){
        grRatioV2[it][id][imeth] -> SetMarkerSize(markerSize);
      }
    }
  }

  TCanvas *can = new TCanvas("can","",200,10,1500,1000);
  can->SetLeftMargin(0.15);
  can->SetRightMargin(0.01);
  can->SetBottomMargin(0.2);
  // gROOT->SetStyle("Pub");
  gStyle->SetErrorX(0);
  // can->SetFillColor(0);
  // can->SetFrameFillStyle(0);
  // can->SetBorderSize(1);
  // gStyle->SetPalette(kDarkRainBow);

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptStat(0);
  can->Divide(2,2,0,0);
  TH2F *h[npad];
  for (int ipad=0;ipad<npad;ipad++){
    can->cd(ipad+1);
    if (ipad==0) h[ipad] = new TH2F(Form("pad_%i",ipad+1),Form(";%s;v_{2} ",xAxisName.Data()),1,minpt,maxpt,1,minV2,maxV2);
    if (ipad==1) h[ipad] = new TH2F(Form("pad_%i",ipad+1),"",1,minpt,maxpt,1,minV2,maxV2);
    if (ipad==2) h[ipad] = new TH2F(Form("pad_%i",ipad+1),";;v_{2}/v_{2,Uniform}",1,minpt,maxpt,1,minV2Ratio,maxV2Ratio);
    if (ipad==3) h[ipad] = new TH2F(Form("pad_%i",ipad+1),Form(";%s     ;",xAxisName.Data()),1,minpt,maxpt,1,minV2Ratio,maxV2Ratio);
    if (ipad>=npad/2){
      h[ipad]->GetXaxis()->SetLabelSize(labelSizeLowerPlot);
      h[ipad]->GetXaxis()->SetTitleSize(titleSizeLowerPlot);
      h[ipad]->GetYaxis()->SetLabelSize(labelSizeLowerPlot);
      h[ipad]->GetYaxis()->SetTitleSize(titleSizeLowerPlot);
    }else{
      h[ipad]->GetXaxis()->SetLabelSize(labelSizeUpperPlot);
      h[ipad]->GetXaxis()->SetTitleSize(titleSizeUpperPlot);
      h[ipad]->GetYaxis()->SetLabelSize(labelSizeUpperPlot);
      h[ipad]->GetYaxis()->SetTitleSize(titleSizeUpperPlot);
    }

    h[ipad]->GetXaxis()->SetNdivisions(504);
    h[ipad]->GetXaxis()->SetTitleOffset(titleOffset);

    h[ipad]->GetXaxis()->SetLabelFont(textFont);
    h[ipad]->GetYaxis()->SetLabelFont(textFont);


    h[ipad]->GetYaxis()->SetNdivisions(504);
    h[ipad]->GetYaxis()->SetTitleOffset(titleOffset);
    h[ipad]->Draw();
    
    TLatex tex;
    tex.SetTextFont(textFont);
    tex.SetTextAlign(33); 
    if (ipad<2) {
      tex.SetTextSize(labelSizeUpperPlot); 
      tex.DrawLatex(0.2,maxV2*0.9,padName[ipad].Data());
    }else {
      tex.SetTextSize(labelSizeLowerPlot);
      tex.DrawLatex(0.2,maxV2Ratio*0.97,padName[ipad].Data());
    }
    
    if (ipad==0) {
      tex.DrawLatex(maxpt*0.9,maxV2*0.9,Form("Au+Au at #sqrt{s_{NN}}=7.7 GeV"));
      tex.DrawLatex(maxpt*0.9,maxV2*0.9-0.02,Form("v_{2}{4}")); 
      TLegend *leg_pt = new TLegend(leg_coordinate[0],leg_coordinate[1],leg_coordinate[2],leg_coordinate[3]);
      leg_pt->SetBorderSize(0);
      leg_pt->SetTextSize(labelSizeUpperPlot);
      leg_pt->SetTextFont(textFont);
      for(int it=0;it<ntype;it++){
        leg_pt->AddEntry(grV2[it][0][0],typeOfGraphName[it].Data(),"p");
      }
      leg_pt->Draw();
    }
    if (ipad==1) {
      tex.DrawLatex(maxpt*0.9,maxV2*0.9,Form("UrQMD, 10-40%%, Ch. hadrons"));
      tex.DrawLatex(maxpt*0.9,maxV2*0.9-0.02,Form("v_{2}{2}")); 
      // TLegend *leg_pt = new TLegend(leg_coordinate[0],leg_coordinate[1],leg_coordinate[2],leg_coordinate[3]);
      // leg_pt->SetBorderSize(0);
      // leg_pt->SetTextSize(labelSizeUpperPlot);
      // leg_pt->SetTextFont(textFont);
      // leg_pt->AddEntry(grV2[0][0][1],legendEntries[1].Data(),"p");
      // leg_pt->AddEntry(grV2[0][0][3],legendEntries[3].Data(),"p");
      // leg_pt->Draw();
    }

    
    for(int it=0;it<ntype;it++){
      if (ipad==0) grV2[it][0][1]->Draw("P"); // PLC PMC // PLC (Palette Line Color) and PMC (Palette Marker Color)
      if (ipad==1) grV2[it][0][3]->Draw("P");
    }
    for(int it=1;it<ntype;it++){
      if (ipad==2) grRatioV2[it][0][1]->Draw("P");
      if (ipad==3) grRatioV2[it][0][3]->Draw("P");
    }
    TLine lineOne;
    lineOne.SetLineStyle(2);
    if (ipad>1)lineOne.DrawLine(minpt,1.,maxpt,1.);
  }
  


  can->SaveAs(Form("Figure_5_AcceptanceCorrection_v2pt.pdf"));
  gROOT->SetStyle("Pub");
  can->SaveAs(Form("Figure_5_AcceptanceCorrection_v2pt.png"));

}