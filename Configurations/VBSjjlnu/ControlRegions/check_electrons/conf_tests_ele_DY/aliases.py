#aliases = {}

configurations = os.getenv("CMSSW_BASE") + "/src/PlotsConfigurations/Configurations/"
conf_folder = configurations +"/VBSjjlnu/ControlRegions/check_electrons/conf_tests_ele_DY/"

mc = [skey for skey in samples if skey not in ('Fake_ee', 'Fake_mm', 'Fake', 'DATA')]

bAlgo = 'DeepB'
bWP = '0.1241'

aliases['PromptGenLepMatch2l'] = {
    'expr': 'Alt$(Lepton_promptgenmatched[0]*Lepton_promptgenmatched[1], 0)',
    'samples': mc
}

aliases['bVeto'] = {
'expr': '(Sum$(CleanJet_pt > 20. && abs(CleanJet_eta)<2.5 && Jet_btagDeepB[CleanJet_jetIdx] > 0.1241) == 0)'
}

aliases['btag0'] = {
'expr': '( Alt$(CleanJet_pt[0],0)<30  \
           && Sum$(CleanJet_pt > 20. && abs(CleanJet_eta)<2.5 && Jet_btagDeepB[CleanJet_jetIdx] > 0.1241)>0 \
         )'
}

aliases['btag1'] = {
'expr': '(    Alt$(CleanJet_pt[0],0)>30 && Alt$(abs(CleanJet_eta[0]),99)<2.5\
           && Alt$(CleanJet_pt[1],0)<30 \
           && Alt$(Jet_btagDeepB[CleanJet_jetIdx[0]], 0) > 0.1241 )'
}

aliases['btag2'] = {
'expr': '(    Alt$(CleanJet_pt[0],0)>30 \
           && Alt$(CleanJet_pt[1],0)>30 \
           && ( ( Alt$(abs(CleanJet_eta[0]),99)<2.5 && Alt$(Jet_btagDeepB[CleanJet_jetIdx[0]],0) > 0.1241 ) \
             || ( Alt$(abs(CleanJet_eta[1]),99)<2.5 && Alt$(Jet_btagDeepB[CleanJet_jetIdx[1]],0) > 0.1241 ) ) \
         )' 
}

aliases['bVetoSF'] = {
'expr': '( TMath::Exp(Sum$( TMath::Log( (CleanJet_pt>20 && abs(CleanJet_eta)<2.5)*Jet_btagSF_shape[CleanJet_jetIdx]+1*(CleanJet_pt<20 || abs(CleanJet_eta)>2.5) ) ) ) )',
'samples': mc
}


aliases['btag0SF'] = {
'expr': '( TMath::Exp(Sum$( TMath::Log( (CleanJet_pt>20 && CleanJet_pt<30 && abs(CleanJet_eta)<2.5)*Jet_btagSF_shape[CleanJet_jetIdx]+1*(CleanJet_pt<20 || CleanJet_pt>30 || abs(CleanJet_eta)>2.5) ) ) ) )',
'samples': mc
}

aliases['btag1SF'] = {
'expr': '( ( Alt$(CleanJet_pt[0], 0)>30 && Alt$(abs(CleanJet_eta[0]),99)<2.5 )*( Alt$(Jet_btagSF_shape[CleanJet_jetIdx[0]], 1) ) + ( Alt$(CleanJet_pt[0], 0)<30 || Alt$(abs(CleanJet_eta[0]),99)>2.5 ) )',
'samples': mc
}

aliases['btag2SF'] = {
'expr': '( ( ( Alt$(CleanJet_pt[0], 0)>30 && Alt$(abs(CleanJet_eta[0]),99)<2.5 )*( Alt$(Jet_btagSF_shape[CleanJet_jetIdx[0]], 1) ) + ( Alt$(CleanJet_pt[0], 0)<30 || Alt$(abs(CleanJet_eta[0]),99)>2.5 ) )* \
           ( ( Alt$(CleanJet_pt[1], 0)>30 && Alt$(abs(CleanJet_eta[1]),99)<2.5 )*( Alt$(Jet_btagSF_shape[CleanJet_jetIdx[1]], 1) ) + ( Alt$(CleanJet_pt[1], 0)<30 || Alt$(abs(CleanJet_eta[1]),99)>2.5 ) ) )\
        ',
'samples': mc
}

aliases['btagSF'] = {
'expr': '( bVetoSF*bVeto + btag0SF*btag0 + btag1SF*btag1 + btag2SF*btag2 + ( (!bVeto) && (!btag0) && (!btag1) && (!btag2) ) )',
'samples': mc
}


# systs = ['jes','lf','hf','lfstats1','lfstats2','hfstats1','hfstats2','cferr1','cferr2']

# for s in systs:
#   aliases['btagSF'+s+'up'] = { 'expr': 'bVeto*'+aliases['bVetoSF']['expr'].replace('shape','shape_up_'+s)+'+btag0*'+aliases['btag0SF']['expr'].replace('shape','shape_up_'+s)+'+btag1*'+aliases['btag1SF']['expr'].replace('shape','shape_up_'+s)+'+btag2*'+aliases['btag2SF']['expr'].replace('shape','shape_up_'+s), 'samples':mc  }
#   aliases['btagSF'+s+'down'] = { 'expr': 'bVeto*'+aliases['bVetoSF']['expr'].replace('shape','shape_down_'+s)+'+btag0*'+aliases['btag0SF']['expr'].replace('shape','shape_down_'+s)+'+btag1*'+aliases['btag1SF']['expr'].replace('shape','shape_down_'+s)+'+btag2*'+aliases['btag2SF']['expr'].replace('shape','shape_down_'+s), 'samples':mc  }

# aliases['ptll_weight_correction'] = {
#     'class': 'ReweightPtll',
#     'args': ("%s/ptll_reweighting.root" % conf_folder, 6, 490) ,
#     'linesToAdd' : [
#         'gSystem->Load("libLatinoAnalysisMultiDraw.so")',
#         '.L %s/reweight_ptll2.cc+' % conf_folder
#      ],
#      'samples': ["DY"]
# }

lastcopy = (1 << 13)

aliases['topGenPtOTF'] = {
    'expr': 'Sum$((GenPart_pdgId == 6 && TMath::Odd(GenPart_statusFlags / %d)) * GenPart_pt)' % lastcopy,
    'samples': ['top']
}

aliases['antitopGenPtOTF'] = {
    'expr': 'Sum$((GenPart_pdgId == -6 && TMath::Odd(GenPart_statusFlags / %d)) * GenPart_pt)' % lastcopy,
    'samples': ['top']
}

aliases['Top_pTrw'] = {
    'expr': '(TMath::Sqrt(TMath::Exp(-2.02274e-01 + 1.09734e-04*topGenPtOTF - 1.30088e-07*topGenPtOTF*topGenPtOTF + 5.83494e+01/(topGenPtOTF+1.96252e+02)) * TMath::Exp(-2.02274e-01 + 1.09734e-04*antitopGenPtOTF - 1.30088e-07*antitopGenPtOTF*antitopGenPtOTF + 5.83494e+01/(antitopGenPtOTF+1.96252e+02))))',
    'samples': ['top']
}

aliases["nearestEleJet"]= {
    'class': "NearestJetDR",
    'args': (),
    'linesToAdd': [
        'gSystem->Load("libLatinoAnalysisMultiDraw.so")',
        '.L %s/nearest_jet_dR_DY.cc+' % conf_folder
    ]
}