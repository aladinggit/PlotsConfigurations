# Snowmass VBF

Configuration for the VBF analysis targeting the Snowmass conference. 

## Instructions to run the analysis

### Produce distributions using mkShapesMulti.py in batch mode

    mkShapesMulti.py --pycfg=configuration.py --doBatch=1 --batchSplit=Samples,Files --batchQueue=testmatch

Resubmit failed jobs:

    cd $HOME/scripts/jobs/mkShapes__vbf_snowmass/
    for i in *jid; do condor_submit ${i/jid/jds}; done
    cd -

Or, if they failed because the wall clock time have been exceeded, resubmit them on a longer-time queue:

    cd $HOME/scripts/jobs/mkShapes__vbf_snowmass/
    for i in *jid; do sed -i "s/longlunch/workday/g" ${i/jid/jds}; condor_submit ${i/jid/jds}; done
    cd -

### Merge rootfiles using hadd

    mkShapesMulti.py --pycfg=configuration.py --doHadd=1 --batchSplit=Samples,Files --doNotCleanup --nThreads=8

### Plot distributions

Differential analysis:

    mkPlot.py --inputFile=rootFile/plots_vbf_snowmass.root --fileFormats=png --onlyPlot=cratio --showIntegralLegend=1 --minLogCratio=0.01 --maxLogCratio=1000
    mkPlot.py --plotFile=plot_blind.py --onlyCut=hww2l2v_13TeV_of2j_vbf --inputFile=rootFile/plots_vbf_snowmass.root --fileFormats=png --onlyPlot=cratio --showIntegralLegend=1 --minLogCratio=0.01 --maxLogCratio=1000
    mkPlot.py --plotFile=plot_blind.py --onlyCut=hww2l2v_13TeV_of2j_test_mll --inputFile=rootFile/plots_vbf_snowmass.root --fileFormats=png --onlyPlot=cratio --showIntegralLegend=1 --minLogCratio=0.01 --maxLogCratio=1000

CP-violation analysis:

    mkPlot.py --plotFile=plot_CPV.py --inputFile=rootFile/plots_vbf_snowmass.root --fileFormats=png --onlyPlot=cratio --showIntegralLegend=1 --minLogCratio=0.01 --maxLogCratio=1000 --outputDirPlots=plots_vbf_snowmass_CPV
    mkPlot.py --plotFile=plot_CPV_blind.py --onlyCut=hww2l2v_13TeV_of2j_vbf --inputFile=rootFile/plots_vbf_snowmass.root --fileFormats=png --onlyPlot=cratio --showIntegralLegend=1 --minLogCratio=0.01 --maxLogCratio=1000 --outputDirPlots=plots_vbf_snowmass_CPV
    mkPlot.py --plotFile=plot_CPV_blind.py --onlyCut=hww2l2v_13TeV_of2j_test_mll --inputFile=rootFile/plots_vbf_snowmass.root --fileFormats=png --onlyPlot=cratio --showIntegralLegend=1 --minLogCratio=0.01 --maxLogCratio=1000 --outputDirPlots=plots_vbf_snowmass_CPV

### Create datacards

    mkDatacards.py --pycfg=configuration.py --inputFile=rootFile/plots_vbf_snowmass.root    

### Combine datacards and create workspace

    ./combinecards.sh
    
### Perform the fit

    ./fitdiagnostics.sh

### Make impact plots

Create impact json file for all gaussian nuisances:

    combineTool.py -M Impacts -d Combination/class0_05_10_ext_try_multicut.root -m 125 --doInitialFit -t -1 --setParameters r_vbf=1,r_ggH=1 --redefineSignalPOIs=r_vbf
    combineTool.py -M Impacts -d Combination/class0_05_10_ext_try_multicut.root -m 125 -t -1 --doFits --setParameters r_vbf=1,r_ggH=1 --redefineSignalPOIs=r_vbf --job-mode=condor
    combineTool.py -M Impacts -d Combination/class0_05_10_ext_try_multicut.root -m 125 -o Combination/impacts_VBF.json -t -1 --setParameters r_vbf=1,r_ggH=1 --redefineSignalPOIs=r_vbf

Create impact json file for rateParam nuisances (WW, Top and DY normalization parameters):

    combineTool.py -M Impacts -d Combination/class0_05_10_ext_try_multicut.root -m 125 --doInitialFit -t -1 --named CMS_hww_WWnorm2j,CMS_hww_Topnorm2j,CMS_hww_DYttnorm2j --setParameterRanges CMS_hww_WWnorm2j=-3,4:CMS_hww_Topnorm2j=-3,4:CMS_hww_DYttnorm2j=-3,4
    combineTool.py -M Impacts -d Combination/class0_05_10_ext_try_multicut.root -m 125 -t -1 --doFits --named CMS_hww_WWnorm2j,CMS_hww_Topnorm2j,CMS_hww_DYttnorm2j --setParameterRanges CMS_hww_WWnorm2j=-3,4:CMS_hww_Topnorm2j=-3,4:CMS_hww_DYttnorm2j=-3,4 --job-mode=interactive --parallel=3
    combineTool.py -M Impacts -d Combination/class0_05_10_ext_try_multicut.root -m 125 -o Combination/impacts_VBF_rateParam.json -t -1 --named CMS_hww_WWnorm2j,CMS_hww_Topnorm2j,CMS_hww_DYttnorm2j --setParameterRanges CMS_hww_WWnorm2j=-3,4:CMS_hww_Topnorm2j=-3,4:CMS_hww_DYttnorm2j=-3,4

Merge the two json files:

    cat Combination/impacts_VBF.json Combination/impacts_VBF_rateParam.json > Combination/impacts_VBF_total.json

Open the Combination/impacts_VBF_total.json file and modify it so that there is only one set of 'POIs', as by merging the two json files as they are, two different sets are created. Then:    

    plotImpacts.py -i Combination/impacts_VBF_total.json -o Combination/impacts_VBF_total.json
