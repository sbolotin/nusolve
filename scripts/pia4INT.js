//
// make initial set up of a session:
//
function makeSetup()
{
    print(' - makeSetup(): making initial setup.');
    var baselines = session.baselines;
    var primaryBand = session.bands[session.primaryBandIdx];
    //
    if (primaryBand.getInputFileVersion > 3) // clear previous editings:
          session.resetAllEditings();
    //
    // turn off everything that has QC<8:
    session.suppressNotSoGoodObs();
    // set up a reference clock station:
    session.pickupReferenceClocksStation();
    // set up a reference station station:
    session.pickupReferenceCoordinatesStation();
    // scan correlator's report and search for applied manual phase calibrations:
    session.checkUseOfManualPhaseCals();
    //
    print(' - makeSetup(): Setting the number of polynomial terms for station clocks to 2:');
    session.setNumOfClockPolynoms4Stations(2);
    print(' + makeSetup(): done.');
    //
    print(' - makeSetup(): Setting the additional std.devs to 20ps for each baseline:');
    for (var i=0; i<baselines.length; i++)
        baselines[i].sigma2add = 20.0e-12;
    print(' + makeSetup(): done.');
    //
    print(' - makeSetup(): Calculating the ionosphere corrections for single band delays only:');
    session.calcIono(true);
    print(' + makeSetup(): done.');
    print(' + makeSetup(): finished.');
};



//
// resolve ambiguities:
//
function try2resolveAmbiguities()
{
    var maxNumOfPasses  = 0.20;         //
    var threshold       = 7.0;          // sigmas
    var upperLimit      = 25.0e-9;      // 25ns
    var bands           = session.bands;
    //
    print(' - try2resolveAmbiguities(): Begin of resolving ambiguities.');
    print(' - try2resolveAmbiguities(): Set up estimated parameters (clocks only):');
    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    print(' + try2resolveAmbiguities(): done.');

    print(' - try2resolveAmbiguities(): Obtaining a solution for SbDelays:');
    config.useDelayType = CFG.VD_SB_DELAY;
    session.process();
    print(' + try2resolveAmbiguities(): done.');
    //
    print(' - try2resolveAmbiguities(): Switching to GrpDelays:');
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.opMode       = CFG.OPM_BASELINE;
    print(' + try2resolveAmbiguities(): done.');
    //
    print(' - try2resolveAmbiguities(): Checking each band:');
    for (var i=0; i<bands.length; i++)
    {
        print('   - try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        config.activeBandIdx = i;
        session.scanAmbiguityMultipliers(i);
        session.process();
        session.eliminateOutliersSimpleMode(i, session.numOfObservations*maxNumOfPasses, threshold, upperLimit);
        print('   + try2resolveAmbiguities(): done.');
    };
    //
    // add parameters and repeat:
    //
    print(' - try2resolveAmbiguities(): Setting the number of polynomial terms for station clocks to 3:');
    session.setNumOfClockPolynoms4Stations(3);
    print(' + try2resolveAmbiguities(): done.');
    print(' - try2resolveAmbiguities(): Set up estimated parameters (plus zenith delays):');
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    print(' + try2resolveAmbiguities(): done.');
    print(' - try2resolveAmbiguities(): Obtaining an intermediate solution:');
    session.process();
    print(' + try2resolveAmbiguities(): done.');
    print(' - try2resolveAmbiguities(): Checking each band again:');
    threshold           = 5.0;          // sigmas
    upperLimit          = 15.0e-9;      // 25ns
    for (var i=0; i<bands.length; i++)
    {
        print('   - try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        config.activeBandIdx = i;
        session.scanAmbiguityMultipliers(i);
        session.process();
        session.eliminateOutliersSimpleMode(i, session.numOfObservations*maxNumOfPasses, threshold, upperLimit);
        print('   + try2resolveAmbiguities(): done.');
    };
    print(' + try2resolveAmbiguities(): finished.');
};





//
// check clock breaks:
//
function checkClockBreaks()
{
    var bands = session.bands;
    print(' - checkClockBreaks(): Begin of clock break checking.');
    config.activeBandIdx = session.primaryBandIdx;
    config.useDelayType  = CFG.VD_GRP_DELAY;
    print(' - checkClockBreaks(): Obtaining an intermediate solution:');
    session.process();
    print(' + checkClockBreaks(): done.');
    for (var i=0; i<bands.length; i++)
    {
        print('   - checkClockBreaks(): Processing the ' + bands[i].key + '-band:');
        session.checkClockBreaks(i);
        print('   + checkClockBreaks(): done.');
    };
    print(' - checkClockBreaks(): finished.');
};





//
// do reweighting and outliers:
//
function doReweightAndOutliers()
{
    // make a full solution:
    print(' - doReweightAndOutliers(): Set up estimated parameters for full solution:');
    session.setNumOfClockPolynoms4Stations(3);
    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.PolusUt1, Parameters.EstimateLocal);
//  parsDescript.setMode4Parameter(Parameters.Bl_Clk,   Parameters.EstimateLocal);
    print(' + doReweightAndOutliers(): done.');
    print(' - doReweightAndOutliers(): Configure the solution:');
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.activeBandIdx= session.primaryBandIdx;
    config.opThreshold  = 3.0;
    config.opMode       = CFG.OPM_BASELINE;
    config.opAction     = CFG.OPA_RESTORE;
    print(' + doReweightAndOutliers(): done.');
    print(' - doReweightAndOutliers(): Reweighting (first run):');
    session.doReWeighting();
    print(' + doReweightAndOutliers(): done.');
    print(' - doReweightAndOutliers(): Begin of reweighting/outlier processing iteration:');
    var numOfRestored = 0;
    var numOfEliminated = 0;
    var roiCounter = 0;
    var mdfData=0;
    do
    {
        if ( (numOfRestored = session.restoreOutliers(session.primaryBandIdx)) )
            session.doReWeighting();
        if ( (numOfEliminated = session.eliminateOutliers(session.primaryBandIdx)) )
            session.doReWeighting();
        mdfData += numOfRestored + numOfEliminated;
        roiCounter++;
    }
    while (numOfRestored + numOfEliminated);
    print(' - doReweightAndOutliers(): End of reweighting/outlier processing iteration. Counter=' +
        roiCounter + ', number of modified observations: ' + mdfData);
    //
    // trun off the flag:
    config.doWeightCorrection = false;
};


//
//
// make some outut:
//
function printInfo()
{
    var primaryBand = session.bands[session.primaryBandIdx];
    print(' -- printInfo(): Session name           : ' + session.name);
    print(' -- printInfo(): Session scheduled by   : ' + session.schedulerName);
    print(' -- printInfo(): Session submitted by   : ' + session.submitterName);
    print(' -- printInfo(): Session correlator name: ' + session.correlatorName);
    print(' -- printInfo(): Session official name  : ' + session.officialName);
    print(' -- printInfo(): Session sessionCode    : ' + session.sessionCode);
    print(' -- printInfo(): Session description    : ' + session.description);
    print(' -- printInfo(): Session suffix         : ' + session.networkSuffix);
    print(' -- printInfo(): Session networkID      : ' + session.networkID);
    print(' -- printInfo(): Session #bands         : ' + session.numOfBands);
    print(' -- printInfo(): Session #stations      : ' + session.numOfStations);
    print(' -- printInfo(): Session #baseline      : ' + session.numOfBaselines);
    print(' -- printInfo(): Session #sources       : ' + session.numOfSources);
    print(' -- printInfo(): Session #observs       : ' + session.numOfObservations);
    print(' -- printInfo(): Session created on     : ' + session.tCreation);
    print(' -- printInfo(): Session started on     : ' + session.tStart);
    print(' -- printInfo(): Session stopped on     : ' + session.tFinis);
    print(' -- printInfo(): Session mean epoch     : ' + session.tMean);

    print(' -- printInfo(): Primary band key       : ' + primaryBand.key);
    print(' -- printInfo(): Epoch of the first obs : ' + primaryBand.tFirst);
    print(' -- printInfo(): Epoch of the last obs  : ' + primaryBand.tLast);
    print(' -- printInfo(): Number of total obs    : ' + primaryBand.numTotal);
    print(' -- printInfo(): Number of usable obs   : ' + primaryBand.numUsable);
    print(' -- printInfo(): Number of used obs     : ' + primaryBand.numProcessed);
    print(' -- printInfo(): WRMS                   : ' + (primaryBand.wrms*1.0e12).toFixed(2) + ' (ps)');
    print(' -- printInfo(): DoF                    : ' + primaryBand.dof);
    print(' -- printInfo(): Chi^2/DoF              : ' + (primaryBand.chi2/primaryBand.dof).toFixed(4));
};




function main()
{
    print('  ==script pia4INT begin==');
    print('');

    // can be full path:
    //handler.fileName = "/home/slb/500/databases/17AUG03XE_V004";
    //
    // or a file name relative to the standard database directory:
    handler.fileName = "17DEC08XU_V004";
    //
    // input format:
    // database handler:
    // handler.inputType = "DBH";
    //
    // vgosDb format:
    handler.inputType = "VDB";

    handler.importSession();

    //
    //
    if (!session.isOk)
    {
        print('Session reading failed.');
        return;
    };

    //
    makeSetup();
    //
    try2resolveAmbiguities();
    //
    checkClockBreaks();
    //
    session.calcIono();
    config.activeBandIdx = session.primaryBandIdx;
    session.process();
    //
    doReweightAndOutliers();
    //
    printInfo();
    //
    print(' -- Saving updated version:');
    handler.saveResults();
    print(' ++ done.');
    //
    print(' -- Generating report:');
    handler.generateReport();
    print(' ++ done.');

    print('');
    print('  ==script pia4INT end==');
};





main();




//for(var key in config) print(key + ' => ' + config[key]);







